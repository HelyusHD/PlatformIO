#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <CSV_Parser.h>
#include "myLog.h"
#include <networkConfig.h>
#include <mySdManager.h>

void mySdManager::listDir(const char * dirname, uint8_t levels){
  LOG(LOG_INFO, String("Listing directory: ") + dirname);

  File root = fs.open(dirname);
  if(!root){
    LOG(LOG_ERROR, "Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    LOG(LOG_ERROR, "Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      LOG(LOG_INFO, String("  DIR : ") + file.name());
      if(levels){
        listDir(file.name(), levels -1);
      }
    } else {
      LOG(LOG_INFO, String("  FILE: ") + file.name() + "  SIZE: " + file.size());
    }
    file = root.openNextFile();
  }
}

void mySdManager::createDir(const char * path){
  LOG(LOG_INFO, String("Creating Dir: ") + path);
  if(fs.mkdir(path)){
    LOG(LOG_INFO, "Dir created");
  } else {
    LOG(LOG_ERROR, "mkdir failed");
  }
}

void mySdManager::removeDir(const char * path){
  LOG(LOG_INFO, String("Removing Dir: ") + path);
  if(fs.rmdir(path)){
    LOG(LOG_INFO, "Dir removed");
  } else {
    LOG(LOG_ERROR, "rmdir failed");
  }
}

void mySdManager::readFile(const char * path){
  LOG(LOG_INFO, String("Reading file: ") + path);

  File file = fs.open(path);
  if(!file){
    LOG(LOG_ERROR, "Failed to open file for reading");
    return;
  }

  String content = "Read from file: ";
  while(file.available()){
    content += (char)file.read();
  }
  LOG(LOG_INFO, content);
  file.close();
}

void mySdManager::writeFile(const char * path, const char * message){
  LOG(LOG_INFO, String("Writing file: ") + path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    LOG(LOG_ERROR, "Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    LOG(LOG_INFO, "File written");
  } else {
    LOG(LOG_ERROR, "Write failed");
  }
  file.close();
}

void mySdManager::appendFile(const char * path, const char * message){
  LOG(LOG_INFO, String("Appending to file: ") + path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    LOG(LOG_ERROR, "Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      LOG(LOG_INFO, "Message appended");
  } else {
    LOG(LOG_ERROR, "Append failed");
  }
  file.close();
}

void mySdManager::renameFile(const char * path1, const char * path2){
  LOG(LOG_INFO, String("Renaming file ") + path1 + " to " + path2);
  if (fs.rename(path1, path2)) {
    LOG(LOG_INFO, "File renamed");
  } else {
    LOG(LOG_ERROR, "Rename failed");
  }
}

void mySdManager::deleteFile(const char * path){
  LOG(LOG_INFO, String("Deleting file: ") + path);
  if(fs.remove(path)){
    LOG(LOG_INFO, "File deleted");
  } else {
    LOG(LOG_ERROR, "Delete failed");
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
    LOG(LOG_ERROR, "Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if(!file){
    LOG(LOG_ERROR, "Failed to open file for writing");
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
    LOG(LOG_ERROR, "Failed to open file");
    return false;
  }

  size_t len = file.readBytes(buffer, maxLen - 1);
  buffer[len] = '\0';
  file.close();
  return true;
}

bool sdSetup(){
  logger.begin(5);

  if(!SD.begin(5)){
    LOG(LOG_ERROR, "Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    LOG(LOG_ERROR, "No SD card attached");
    return false;
  }

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

  static mySdManager SdM(SD);

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
