// CmdFunctions.h

#ifndef CMDFUNCTIONS_H
#define CMDFUNCTIONS_H

#include <Arduino.h>

// Function prototype for sendSL3Cmd
String sendSL3Get(String input);
String sendSL3Set(String input);
String sendSL3Tx(String command);

String mComGet(String input);
String mComSet(String input);
String sendmComTx(String command);

String cleanTXResponse(String txInput);

String subComma(String input);
bool printChar(String str);
// Template for myCommands
String myCommands(Config input) {
  String response = "";
  String mTemp = "";
  switch (input.GOES) {
    case 1:
      response += subComma(sendSL3Set("TX1 Satellite ID=" + input.ID)) + "\n";
      response += subComma(sendSL3Set("TX1 Channel=" + input.Channel)) + "\n";
      response += subComma(sendSL3Set("TX1 Scheduled Time=" + input.First)) + "\n";
      response += subComma(sendSL3Set("TX1 Scheduled Interval=" + input.Period)) + "\n";
      response += subComma(sendSL3Set("TX1 Window Length=" + input.Window)) + "\n";
      response += subComma(sendSL3Set("TX1 Satellite Type=GOES " + input.Baud)) + "\n";
      response += subComma(sendSL3Set("Cutoff Tx Low=11.5")) + "\n";
      response += subComma(sendSL3Set("Cutoff System low=11.3")) + "\n";
      response += subComma(sendSL3Set("GPS Antenna Detect=Off")) + "\n";
      response += subComma(sendSL3Set("TX1 Enable=On")) + "\n";
      response = response.substring(4);
      break;

    case 2:
      response += mComSet("DTX") + "\n";
      response += mComSet("PID=" + input.ID) + "\n";
      mTemp = input.Channel;
      mTemp.trim();
      mTemp = mTemp + "," + input.Baud;
      response += mComSet("TCH=" + mTemp) + "\n";
      response += mComSet("FTT=" + input.First) + "\n";
      response += mComSet("TTI=00:" + input.Period) + "\n";
      mTemp = input.Window;
      mTemp.trim();
      mTemp = mTemp.substring(mTemp.length() - 2);
      response += mComSet("TWL=" + mTemp) + "\n";
      response += mComSet("ETX") + "\n";
      response += mComSet("RCF") + "\n";
      break;


    // Future cases for other commands can be added here
    default:
      response = "1";
      break;
  }
  if (printChar(response)) {
    return response;
  }
  return "0";
}

// Template for myCommands
String TXCommands(String input, int cmdType) {
  String response = "";
  switch (cmdType) {
    case 1:
      response = sendSL3Tx("\n" + input + "\x03");
      break;

    case 2:
      response = sendmComTx(input);
      break;

    default:
      break;
  }
  return response;
}

// Definition for sendSL3Cmd function
String sendSL3Get(String input) {
  String output = "";
  input = "\r" + input + "\r";

  // Write command to Serial3
  Serial3.print(input);
  delay(50);

  // Read response from Serial3
  while (Serial3.available()) {
    output = Serial3.readString();

    // Clean up the response
    output.replace(">", "");
    output.replace("\n", "");
    output.replace("\r", "");
    output.replace("\x03", "");
  }
  return output;
}


// Definition for sendSL3Cmd function
String sendSL3Set(String input) {
  String output = "";
  input = "\r" + input + "\r";

  // Write command to Serial3
  Serial3.print(input);
  delay(50);

  // Read response from Serial3
  while (Serial3.available()) {
    output = Serial3.readString();
    // Clean up the response
    output.replace(">", "");
    output.replace("\r\n\r\n", "");
    output.replace("\r\n", " -> ");
    output.replace("\x03", "");
  }
  return output;
}

String mComGet(String input) {
  Serial3.setTimeout(250);
  Serial3.print("\r");
  Serial3.readString();
  Serial3.print(input + "\r");
  String output = Serial3.readString();
  output.replace("\r", "");
  output.replace("\n", "");
  output.replace(">", "");
  output.replace(" ", "");

  if (input == "RBV") {
    int myIndex = output.indexOf("+");
    if (myIndex != -1) {
      output = output.substring(myIndex + 1);  // Extract the value after the '+' character for battery
    }
    return output;
  }

  int myIndex = output.indexOf('=');
  if (myIndex != -1) {
    output = output.substring(myIndex + 1);  // Extract the value after the '=' character
  }

  // Remove any trailing data after a dot if present
  myIndex = output.indexOf('.');
  if (myIndex != -1) {
    return output.substring(0, myIndex);
  }
  return output;
}


String mComSet(String input) {
  String output = "";
  input += "\r";  // Append carriage return to the input command
  Serial3.setTimeout(125);
  Serial3.print("\r");            // Send a carriage return to clear any previous commands
  Serial3.readString();           // Read and discard the first response
  Serial3.print(input);           // Send the input command
  output = Serial3.readString();  // Read the response

  // Clean and format the response
  output.replace("\r", "");
  output.replace("\n", "");
  output.replace(">", "");
  output.replace("OK", ", Response: OK");
  output.replace("ERR", ", Response: Err");
  return output;
}




// Function to send a command and receive a response via Serial3
String sendSL3Tx(String command) {
  String response = "";
  // Write initial command to Serial3
  Serial3.print("\r!TX1 EXTBUF INSERT\r");
  while (Serial3.available()) {
    response = Serial3.readString();
  }
  // Send the actual command
  Serial3.println(command);
  delay(75);  // Adjust delay as needed for response time
  while (Serial3.available()) {
    response = Serial3.readString();

    // Clean up the response
    response.replace(">", "");
    response.replace("\n", "");
    response.replace("\r", "");
    response.replace("\x03", "");
  }
  return response;
}

String sendmComTx(String command) {
  String response = "";
  command.replace("/", "//");
  command.replace("\r", "/r");
  command.replace("\n", "/n");
  command.replace("\x03", "");
  Serial3.setTimeout(125);
  Serial3.print("\r");
  Serial3.readString();
  Serial3.print("CTB\r");
  Serial3.readString();
  Serial3.print("TDT=/n" + command + "\r");
  Serial3.readString();
  Serial3.print("TBS?\r");
  String sizeStr = Serial3.readString();
  sizeStr = cleanTXResponse(sizeStr);
  int sizeInt = sizeStr.toInt();
  if (sizeInt) {
    return "OK " + sizeStr + " bytes added";
  }
  return "ERR";
}

String cleanTXResponse(String txInput) {
  // Remove unwanted characters
  txInput.replace("\r", "");
  txInput.replace("\n", "");
  txInput.replace(">", "");
  // Find the position of the '=' character
  int myIndex = txInput.indexOf('=');
  if (myIndex != -1) {
    return txInput.substring(myIndex + 1);  // Extract the value after the '=' character
  }
  return "";
}


String subComma(String input) {
  int commaIndex = input.indexOf(',');      // Find the index of the comma
  if (commaIndex != -1) {                   // If comma is found
    return input.substring(0, commaIndex);  // Return substring before the comma
  }
  return input;  // If no comma, return the original string
}

bool printChar(String str) {
  for (int i = 0; i < str.length(); i++) {
    if (isPrintable(str[i])) {
      return true;
    }
  }
  return false;
}


#endif
