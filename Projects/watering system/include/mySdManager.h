#include <Arduino.h>
#include "SD.h"
#include <myLog.h>

#ifndef MY_SD_MANAGER
#define MY_SD_MANAGER
    class mySdManager
    {
    private:
        fs::FS &fs;
    public:
        mySdManager(fs::FS &fs_);
        ~mySdManager();
        void listDir(const char * dirname, uint8_t levels);
        void createDir(const char * path);
        void removeDir(const char * path);
        void readFile(const char * path);
        void writeFile(const char * path, const char * message);
        void appendFile(const char * path, const char * message);
        void renameFile(const char * path1, const char * path2);
        void deleteFile(const char * path);
        void testFileIO(const char * path);
    };
    
    mySdManager::mySdManager(fs::FS &fs_)
        : fs(fs_) //saves parameters in properties
    {
        LOG(LOG_DEBUG,"Created SdManager");
    }
    mySdManager::~mySdManager()
    {
        LOG(LOG_DEBUG,"Destroyed SdManager");
    }
    
#endif