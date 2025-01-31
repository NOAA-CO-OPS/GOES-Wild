#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <SPI.h>
#include <SD.h>

// Config structure to hold configuration parameters
struct Config {
    int GOES;
    String ID;
    String Channel;
    String First;
    String Period;
    String Window;
    String Baud;
    bool configRead;
};

class FileManager {
public:
    FileManager(int chipSelect, const String& fileTime);
    bool initialize();
    bool createNewFile();
    unsigned long writeLine(const String& line);
    String readUpTo(int length);
    long getRemaining();
    Config readConfig();
    bool writeToFile(const String& message, const String& fileTimeTwo, bool sentStatus);
    unsigned long getFileSize();  // New method to get the current file size

private:
    int _chipSelect;
    String _directoryPath;
    String _currentFileName;
    File _dataFile;
    long _currentFilePos;
    long _charsWritten;
    long _charsRead;
};

#endif
