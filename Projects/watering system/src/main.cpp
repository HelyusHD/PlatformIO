#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <CSV_Parser.h>
#include "myLog.h"
#include <networkConfig.h>
#include <mySdManager.h>

int mySdManager::listDir(const char * dirname, uint8_t levels, const char* nameFilter){
  LOG(LOG_INFO, String("Listing directory: ") + dirname);
  int matchCount = 0;

  File root = fs.open(dirname);
  if(!root){
    LOG(LOG_ERROR, "Failed to open directory");
    return matchCount;
  }
  if(!root.isDirectory()){
    LOG(LOG_ERROR, "Not a directory");
    return matchCount;
  }
  
  
  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      LOG(LOG_INFO, String("  DIR : ") + file.name());
      if(levels){
        matchCount += listDir(file.name(), levels -1,nameFilter);
      }
    } else {
      LOG(LOG_INFO, String("  FILE: ") + file.name() + "  SIZE: " + file.size());
      // increase found file count by one
        String fileName = file.name();
        if(!nameFilter || fileName.indexOf(nameFilter) >= 0){matchCount++;};
    }
    file = root.openNextFile();
  }
  return matchCount;
}

void mySdManager::createDir(const char * path){
  LOG(LOG_DEBUG, String("Creating Dir: ") + path);
  if(fs.mkdir(path)){
    LOG(LOG_DEBUG, "Dir created");
  } else {
    LOG(LOG_ERROR, String("Failed creating Dir: ") + path);
  }
}

void mySdManager::removeDir(const char * path){
  LOG(LOG_DEBUG, String("Removing Dir: ") + path);
  if(fs.rmdir(path)){
    LOG(LOG_DEBUG, "Dir removed");
  } else {
    LOG(LOG_ERROR, String("Failed removing Dir: ") + path);
  }
}

void mySdManager::serialPrintFile(const char * path){
  LOG(LOG_DEBUG, String("Reading file: ") + path);

  File file = fs.open(path);
  if(!file){
    LOG(LOG_ERROR, String("Failed to open file for reading at path: ") + path);
    return;
  }

  String content = "Read from file: ";
  while(file.available()){
    content += (char)file.read();
  }
  LOG(LOG_DEBUG, content);
  file.close();
}

void mySdManager::writeFile(const char * path, const char * message){
  LOG(LOG_DEBUG, String("Writing file: ") + path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    LOG(LOG_ERROR, String("Failed to open file for writing at path: ") + path);
    return;
  }
  if(file.print(message)){
    LOG(LOG_DEBUG, "File written");
  } else {
    LOG(LOG_ERROR, String("Failed to write to file at path: ") + path);
  }
  file.close();
}

void mySdManager::appendFile(const char * path, const char * message){
  LOG(LOG_DEBUG, String("Appending to file: ") + path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    LOG(LOG_ERROR, String("Failed to open file for appending at path: ") + path);
    return;
  }
  if(file.print(message)){
      LOG(LOG_DEBUG, "Message appended");
  } else {
    LOG(LOG_ERROR, String("Failed to append to file at path: ") + path);
  }
  file.close();
}

void mySdManager::renameFile(const char * path1, const char * path2){
  LOG(LOG_DEBUG, String("Renaming file ") + path1 + " to " + path2);
  if (fs.rename(path1, path2)) {
    LOG(LOG_DEBUG, "File renamed");
  } else {
    LOG(LOG_ERROR, String("Failed renaming file ") + path1 + " to " + path2);
  }
}

void mySdManager::deleteFile(const char * path){
  LOG(LOG_DEBUG, String("Deleting file: ") + path);
  if(fs.remove(path)){
    LOG(LOG_DEBUG, "File deleted");
  } else {
    LOG(LOG_ERROR, String("Failed deleting file at path: ") + path);
  }
}

void mySdManager::testFileIO(const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len > 512 ? 512 : len;
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    file.close();
    LOG(LOG_INFO, String(flen) + " bytes read in " + end + " ms");
  } else {
    LOG(LOG_ERROR, String("Failed to open file for reading at path: ") + path);
  }

  file = fs.open(path, FILE_WRITE);
  if(!file){
    LOG(LOG_ERROR, String("Failed to write to file at path: ") + path);
    return;
  }

  size_t i;
  start = millis();
  for(i = 0; i < 2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  file.close();
  LOG(LOG_INFO, String(2048 * 512) + " bytes written in " + end + " ms");
}

bool readFileToBuffer(const char* path, char* buffer, size_t maxLen) {
  File file = SD.open(path);
  if (!file) {
    LOG(LOG_ERROR, String("Failed to open file at path: ") + path);
    return false;
  }

  size_t len = file.readBytes(buffer, maxLen - 1);
  buffer[len] = '\0';
  file.close();
  return true;
}

MyLogger logger(SD);
mySdManager SdM(SD);

bool sdSetup(){
  if(!SD.begin(5)){
    LOG(LOG_ERROR, "Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    LOG(LOG_ERROR, "No SD card attached");
    return false;
  }

  //static MyLogger logger(SD);
  logger.init();

  String cardTypeStr = "SD Card Type: ";
  switch (cardType) {
    case CARD_MMC:  cardTypeStr += "MMC"; break;
    case CARD_SD:   cardTypeStr += "SDSC"; break;
    case CARD_SDHC: cardTypeStr += "SDHC"; break;
    default:        cardTypeStr += "UNKNOWN"; break;
  }
  LOG(LOG_INFO, cardTypeStr);

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  LOG(LOG_INFO, String("SD Card Size: ") + cardSize + "MB");

  //static mySdManager SdM(SD);

  SdM.listDir("/", 5);
  SdM.createDir("/csv");
  const char * csv_str = 
    "my_strings,my_numbers\n"
    "hello,5\n"
    "world,10\n";
  SdM.writeFile("/csv/first.csv", csv_str);
  char content[256];
  if (readFileToBuffer("/csv/first.csv", content, sizeof(content))){
    CSV_Parser cp(content, "sL");
    LOG(LOG_INFO, "Accessing values by column name:");
    char **strings = (char**)cp["my_strings"];
    int32_t *numbers = (int32_t*)cp["my_numbers"];
    
    for(int row = 0; row < cp.getRowsCount(); row++) {
      LOG(LOG_INFO, String(row) + ". String = " + strings[row]);
      LOG(LOG_INFO, String(row) + ". Number = " + numbers[row]);
    }
  }

  LOG(LOG_INFO, String("Total space: ") + (SD.totalBytes() / (1024 * 1024)) + "MB");
  LOG(LOG_INFO, String("Used space: ") + (SD.usedBytes() / (1024 * 1024)) + "MB");
  return true;
}

void setup(){
  Serial.begin(115200);

  WiFi.begin(HOME_NETWORK_SSID, HOME_NETWORK_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  sdSetup();
}

void loop(){}
