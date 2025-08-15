#include <Arduino.h>
#include "SD.h"
#include "SPI.h"
#include "FS.h"

#ifndef MY_SD_MANAGER
#define MY_SD_MANAGER
    class MySdManager
    {
    private:
        fs::FS &fs;
    public:
        MySdManager(fs::FS &fs_);
        ~MySdManager();

        int listDir(const char * dirname, uint8_t levels, const char* nameFilter = nullptr, int depths = 0);    /*  Lists all directories and files recursively, up to "levels" deep.
                                                                                                    - nameFilter is an optional parameter.
                                                                                                    - Returns the number of files whose names include "nameFilter".
                                                                                                    - If no nameFilter is given, counts all files.
                                                                                                */
        void createDir(const char * path);                         // Creates a directory at "path"
        void removeDir(const char * path);                         // Removes the directory at "path"
        void serialPrintFile(const char * path);                   // Prints the contents of the file at "path" to Serial
        void writeFile(const char * path, const char * message);   // Creates a file at "path" and writes "message" to it
        void appendFile(const char * path, const char * message);  // Appends "message" to the file at "path"
        void renameFile(const char * path1, const char * path2);   // Moves or renames a file from "path1" to "path2"
        void deleteFile(const char * path);                        // Deletes the file at "path"
        void testFileIO(const char * path);                        // Benchmarks read/write speed and logs the result
        bool readFileToBuffer(const char* path, char* buffer, size_t maxLen);
        fs::FS& getFS() {
            return fs;
        }
    };
    
    MySdManager::MySdManager(fs::FS &fs_)
        : fs(fs_) //saves parameters in properties
    {
        Serial.println("Created SdManager");
    }
    MySdManager::~MySdManager()
    {
       Serial.println("Destroyed SdManager");
    }
    
#endif