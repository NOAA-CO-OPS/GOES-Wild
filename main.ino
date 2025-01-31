/**
 * Project Title: GOES Transmitter Communication and Data Logging System
 * Author: David Ilogho
 * Date: August, 2024
 *
 * Description:
 * This program initializes and manages communication between various components such as
 * Motus Sensorgnome, GOES transmitter, GPS, and an SD card for data logging. It reads configuration settings
 * from an SD card, synchronizes the real-time clock (RTC) using either a transmitter or GPS,
 * and logs incoming messages for transmission at defined intervals. The program also manages
 * display outputs and user interactions through a touch-enabled screen.
 */

#include <Scheduler.h>
#include "DisplayFunctions.h"
#include "RTCFunctions.h"
#include "FileManager.h"
#include "CmdFunctions.h"
#include "TimeLeft.h"
#include "Checksum.h"
#include "TouchHandler.h"
GigaDisplayRGB rgb;

const int chipSelect = 10;
FileManager fileManager(chipSelect, "");
Config config;
bool SDHold = false;
String addLine = "";
signed int charNum;
unsigned long screenTime;
unsigned long previousLED = 0;
unsigned long intervalOn = 100;    // LED on for 100ms
unsigned long intervalOff = 5000;  // LED off for 5 seconds
bool ledState = LOW;
bool lockWrite = false;
unsigned long lockWriteTime = 0;

void setup() {
  Serial.begin(115200);   // Initialize Serial for COMS
  Serial2.begin(9600);    // Initialize Serial2 for GPS communication
  Serial3.begin(9600);    // Initialize Serial3 for GOES Transmitter communication
  Serial4.begin(115200);  // Initialize Serial4 for rPi4 communication
  backlight.begin();      // Initialize the backlight
  initDisplay();          // Initialize the display
  rgb.begin();
  rgb.on(0, 0, 0);
  backlight.set(100);
  rgb.on(255, 0, 0);
  delay(200);
  rgb.on(0, 250, 0);
  delay(200);
  rgb.on(0, 0, 250);
  delay(200);
  rgb.on(255, 0, 0);
  initShow();
  rgb.on(0, 0, 0);
  initTouch();
  // Initialize SD card and create a new file
  displayText("Initializing SD Card...", YELLOW, BLACK, 4, 0, 0);
  delay(5000);
  if (!fileManager.initialize()) {
    displayText("Initializing SD Card failed. Please check SD Card and reboot.", RED, BLACK, 4, 0, 0);
    Serial.println("SD card initialization failed.");

    while (true)
      ;  // Halt program
  }
  displayText("SD Card Initialization successful!", GREEN, BLACK, 4, 0, 0);
  delay(5000);


  // Read configuration from file
  displayText("Reading config.txt file...", YELLOW, BLACK, 4, 0, 0);
  delay(5000);
  config = fileManager.readConfig();

  // Check if config was read successfully
  if (config.configRead) {
    Serial.println("Config read successfully!");
    displayText("config.txt file read successful!", GREEN, BLACK, 4, 0, 0);
    delay(5000);
    displayText("Configuring GOES transmitter...", YELLOW, BLACK, 4, 0, 0);
    delay(5000);
    String response = myCommands(config);
    if (response == "0") {
      displayText("Communication with GOES transmitter failed. Please check connection and reboot.", RED, BLACK, 4, 0, 0);
      while (true)
        ;  // Halt program
    } else if (response == "1") {
      displayText("Please check if the GOES ID in the config.txt file is correct and reboot.", RED, BLACK, 4, 0, 0);
      while (true)
        ;  // Halt program
    } else {
      displayText(response.c_str(), WHITE, BLACK, 2, 0, 0);
    }
    delay(15000);
    unsigned long startTime = millis();
    unsigned long txTimeout = 300000;
    bool firstLoopSuccess = false;
    displayText("Setting RTC using transmitter...", YELLOW, BLACK, 4, 0, 0);
    delay(5000);
    while (millis() - startTime < txTimeout) {
      if (RTCSet(config.GOES)) {
        firstLoopSuccess = true;
        displayText("Setting RTC with transmitter successful!", GREEN, BLACK, 4, 0, 0);
        delay(5000);
        break;
      }
    }
    if (!firstLoopSuccess) {
      displayText("Setting RTC with transmitter failed after five minutes, setting RTC using GPS...", RED, BLACK, 4, 0, 0);
      delay(5000);
      unsigned long countSec = millis();
      while (!RTCSet(9)) {
        String countStr = "Trying to set RTC using GPS...\n" + String((millis() - countSec) / 1000) + " seconds";
        displayText(countStr.c_str(), YELLOW, BLACK, 6, 0, 0);
      }
      displayText("RTC was successfully set using GPS!", GREEN, BLACK, 4, 0, 0);
      delay(5000);
    }

    // Initialize fileManager with the correct file name
    displayText("Initializing file manager and creating file...", YELLOW, BLACK, 4, 0, 0);
    delay(5000);
    fileManager = FileManager(chipSelect, RTCTimeFile());
    fileManager.initialize();

  } else {
    displayText("config.txt file read failed. Please check file and try again.", RED, BLACK, 4, 0, 0);
    while (true)
      ;  // Halt program
  }

  if (!fileManager.createNewFile()) {
    displayText("File creation unsuccessful. Please ensure the SD card is OK and reboot.", RED, BLACK, 4, 0, 0);
    while (true)
      ;  // Halt program
  }


  charNum = numberOfChar(config.Window, config.Baud);
  displayText("SYSTEM\n       INITAILIZATION\n       SUCCESSFUL!!!", GREEN, WHITE, 5, 210, 150);
  delay(5000);
  Serial.println(charNum);
  Scheduler.startLoop(detectTag);
  Scheduler.startLoop(txData);
  Scheduler.startLoop(screenMsg);
  Scheduler.startLoop(screenLED);
  Scheduler.startLoop(updateTime);
  screenTime = millis();
}


void loop() {
  delay(1000);
}


void detectTag() {
  if (Serial4.available()) {
    String incomingLine = Serial4.readStringUntil('\n') + "\n";
    if (checksum(incomingLine)) {
      if (SDHold) {
        addLine += incomingLine;
      } else {
        SDHold = true;
        unsigned long charsWritten = fileManager.writeLine(addLine + incomingLine);
        SDHold = false;
        lockWrite = true;
        lockWriteTime = millis();
        displayText(("TAG(S) DETECTED:\n" + addLine + incomingLine).c_str(), YELLOW, BLACK, 3, 0, 100);  //SIDE DOWN
        Serial.print(addLine + incomingLine);
        addLine = "";
        if (!charsWritten) {
          Serial.println("Error writing to file!");
        }
      }
    }
  }
  yield();
}


void txData() {
  delay(1000 * timeLeft(config.First, config.Period, RTCTimeStruc()));

  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    if (!SDHold) {
      SDHold = true;
      String readData = fileManager.readUpTo(charNum);
      Serial.print(readData);
      if (readData.length() == 0) {
        readData = "No messages available for transmission!\n";
      }
      bool sentStatus = false;

      unsigned int txIntervals = fileManager.getRemaining() / charNum + 1;
      unsigned long txSeconds = txIntervals * totalSeconds(config.Period);
      unsigned int txDays = txSeconds / 86400;
      txSeconds = txSeconds - (txDays * 86400);
      unsigned int txHours = txSeconds / 3600;
      txSeconds = txSeconds - (txHours * 3600);
      unsigned int txMinutes = txSeconds / 60;
      unsigned int txAccept = max(0, charNum - fileManager.getRemaining());
      String txMsg = String(charNum) + "," + String(fileManager.getRemaining()) + "," + String(txAccept);
      String txTime = String(txDays) + "," + String(txHours) + "," + String(txMinutes);

      if (config.GOES == 1) {
        readData = readData + sendSL3Get("!BATT") + "," + txMsg + "," + txTime + "\n";
      } else if (config.GOES == 2) {
        readData = readData + mComGet("RBV") + "," + txMsg + "," + txTime + "\n";
      }
      String response = TXCommands(readData, config.GOES);
      if (response.indexOf("OK") != -1) {
        sentStatus = true;
      }
      Serial.print("Message length: ");
      Serial.println(readData.length());
      Serial.print("Tx response: ");
      Serial.println(response);

      fileManager.writeToFile("Time sent: " + RTCTimeString() + "\nTransmitter response: " + response + "\nMessage:\n" + readData + "\n\n", RTCTimeFile(), sentStatus);
      if (fileManager.getFileSize() >= 1000000 && fileManager.getRemaining() <= 0) {
        fileManager = FileManager(chipSelect, RTCTimeFile());
        fileManager.initialize();
        fileManager.createNewFile();
      }
      SDHold = false;
      Serial.println("Time sent: " + RTCTimeString() + "\nTransmitter response: " + response + "\nMessage:\n" + readData + "\n\n");
      Serial.println(txMsg + "," + txTime + "\n");
      Serial4.println(txMsg + "," + txTime + "\n");
      break;
    }
    delay(500);
  }
}

void screenMsg() {
  if (!lockWrite) {
    displayText((RTCTimeString() + " UTC\n\n\n\n\n\n\n\n\n                          REBOOT").c_str(), BLACK, WHITE, 4, 120, 100);
  }

  if (millis() - lockWriteTime > 10000 && lockWrite) {
    lockWrite = false;
  }

  delay(1000);
}

void screenLED() {
  if (millis() - screenTime > 120000) {
    backlight.set(0);
  } else {
    backlight.set(100);
  }
  unsigned long currentLED = millis();
  // Retrieve the touch coordinates
  TouchPoint point = getTouchPoint();

  if (point.x != -1 && point.y != -1) {
    delay(1500);
    if (millis() - screenTime < 120000 && point.x < 100 && point.y > 700 && point.x != -1) {
      lockWrite = true;
      displayText("Rebooting...", RED, BLACK, 4, 100, 100);
      rgb.on(255, 0, 0);
      delay(5000);
      NVIC_SystemReset();  // Reboots the Arduino Giga
    }
    screenTime = millis();
  }

  if (ledState == HIGH && currentLED - previousLED >= intervalOn) {
    ledState = LOW;
    previousLED = currentLED;
    rgb.on(0, 0, 0);
  } else if (ledState == LOW && currentLED - previousLED >= intervalOff) {
    ledState = HIGH;
    previousLED = currentLED;
    rgb.on(0, 255, 0);
  }
  yield();
}

void updateTime() {
  timeStruc updateTime = RTCTimeStruc();
  delay((86400 - (updateTime.hour * 3600 + updateTime.minute * 60 + updateTime.second)) * 1000);
  unsigned long startTime = millis();
  while (millis() - startTime < 60000) {
    if (RTCSet(config.GOES)) {
      break;
    }
  }
}
