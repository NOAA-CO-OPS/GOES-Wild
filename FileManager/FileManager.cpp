#include "FileManager.h"

FileManager::FileManager(int chipSelect, const String& fileTime)
  : _chipSelect(chipSelect), _directoryPath("/RcvdMsgs/" + fileTime), _currentFilePos(0), _charsWritten(0), _charsRead(0) {}

bool FileManager::initialize() {
  return SD.begin(_chipSelect);
}

bool FileManager::createNewFile() {
  SD.mkdir(_directoryPath);  // Create directory if it doesn't exist

  for (int i = 0; i < 100; i++) {
    String fileName = _directoryPath + "/Msgs" + String(i);
    if (!SD.exists(fileName)) {
      _currentFileName = fileName;
      _dataFile = SD.open(_currentFileName, FILE_WRITE);
      if (_dataFile) {
        _dataFile.close();
        return true;
      }
      return false;  // Error opening file
    }
  }
  return false;  // No available file slots
}

unsigned long FileManager::writeLine(const String& line) {
  _dataFile = SD.open(_currentFileName, FILE_WRITE);
  if (_dataFile) {
    _dataFile.print(line);
    unsigned long length = line.length();
    _charsWritten += length;
    _dataFile.close();
    return length;  // Return the number of characters written
  }
  return 0;  // Return 0 if the file fails to open
}

String FileManager::readUpTo(int length) {
  String result = "";
  _dataFile = SD.open(_currentFileName, FILE_READ);
  if (_dataFile) {
    _dataFile.seek(_currentFilePos);
    int charCount = 0;
    bool foundNewLine = false;

    while (_dataFile.available() && !foundNewLine) {
      char c = _dataFile.read();
      if (c == '\n' && charCount >= length) {
        foundNewLine = true;
      }
      result += c;
      charCount++;
    }

    _currentFilePos = _dataFile.position();
    _charsRead += result.length();
    _dataFile.close();
  }
  return result;
}

long FileManager::getRemaining() {
  return _charsWritten - _charsRead;
}

Config FileManager::readConfig() {
  Config config;
  config.configRead = false;  // Default to false if config is not read

  File myFile = SD.open("config.txt");
  if (myFile) {
    String fileData = "";
    while (myFile.available()) {
      fileData += (char)myFile.read();
    }
    myFile.close();

    if (fileData.length() > 0) {
      fileData.replace(" ", "");
      config.GOES = fileData.substring(fileData.indexOf("GOES=") + 5, fileData.indexOf('\n', fileData.indexOf("GOES="))).toInt();
      config.ID = fileData.substring(fileData.indexOf("ID=") + 3, fileData.indexOf('\n', fileData.indexOf("ID=")));
      config.Channel = fileData.substring(fileData.indexOf("Channel=") + 8, fileData.indexOf('\n', fileData.indexOf("Channel=")));
      config.First = fileData.substring(fileData.indexOf("First=") + 6, fileData.indexOf('\n', fileData.indexOf("First=")));
      config.Period = fileData.substring(fileData.indexOf("Period=") + 7, fileData.indexOf('\n', fileData.indexOf("Period=")));
      config.Window = fileData.substring(fileData.indexOf("Window=") + 7, fileData.indexOf('\n', fileData.indexOf("Window=")));
      config.Baud = fileData.substring(fileData.indexOf("Baud=") + 5, fileData.indexOf('\n', fileData.indexOf("Baud=")));
      config.configRead = true;  // Set to true if config was successfully read
    } else {
      Serial.println("config.txt file is empty");
    }
  } else {
    Serial.println("Error opening config.txt file");
  }

  return config;
}

bool FileManager::writeToFile(const String& message, const String& fileTimeTwo, bool sentStatus) {
  String directoryPath = "/txArch/" + fileTimeTwo;

  if (!SD.mkdir(directoryPath)) {
    Serial.println("Failed to create directory or directory already exists.");
  }

  String fileName = directoryPath + (sentStatus ? "/sent.txt" : "/unst.txt");
  File dataFile = SD.open(fileName, FILE_WRITE);

  if (dataFile) {
    dataFile.println(message);  // Write the message
    dataFile.close();           // Close the file
    return true;
  } else {
    Serial.print("Failed to open file: ");
    Serial.println(fileName);
    return false;  // Failed to open the file
  }
}

unsigned long FileManager::getFileSize() {
  _dataFile = SD.open(_currentFileName, FILE_READ);
  if (_dataFile) {
    unsigned long size = _dataFile.size();
    _dataFile.close();
    return size;
  }
  return 0;  // Return 0 if the file fails to open
}
