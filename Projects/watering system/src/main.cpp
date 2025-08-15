
#include <CSV_Parser.h>
#include <myLog.h>
#include <mySdManager.h>
#include <myServer.h>

int MySdManager::listDir(const char * dirname, uint8_t levels, const char* nameFilter, int depths){
  if (depths == 0) LOG(LOG_INFO, String("Listing directory: ") + dirname);

  int matchCount = 0;

  String tabs = "";
  for (int i=0; i < depths; i++){
    tabs += "    ";
  }

  File root = fs.open(dirname);
  if(!root){
    LOG(LOG_ERROR, tabs + String("Failed to open directory ") + dirname);
    return matchCount;
  }
  if(!root.isDirectory()){
    LOG(LOG_ERROR, tabs + String("Not a directory ") + dirname);
    return matchCount;
  }
  
  
  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      LOG(LOG_INFO, tabs + String("  DIR : ") + file.name());
      if(levels){
        String subdir = String(dirname);
        if (!subdir.endsWith("/")) subdir += "/";
        subdir += file.name();
        matchCount += listDir(subdir.c_str(), levels - 1, nameFilter, depths+1);
      }
    } else {
      LOG(LOG_INFO, tabs + String("  FILE: ") + file.name() + "  SIZE: " + file.size());
      // increase found file count by one
        String fileName = file.name();
        if(!nameFilter || fileName.indexOf(nameFilter) >= 0){matchCount++;};
    }
    file = root.openNextFile();
  }
  return matchCount;
}

void MySdManager::createDir(const char * path){
  LOG(LOG_DEBUG, String("Creating Dir: ") + path);
  if(fs.mkdir(path)){
    LOG(LOG_DEBUG, "Dir created");
  } else {
    LOG(LOG_ERROR, String("Failed creating Dir: ") + path);
  }
}

void MySdManager::removeDir(const char * path){
  LOG(LOG_DEBUG, String("Removing Dir: ") + path);
  if(fs.rmdir(path)){
    LOG(LOG_DEBUG, "Dir removed");
  } else {
    LOG(LOG_ERROR, String("Failed removing Dir: ") + path);
  }
}

void MySdManager::serialPrintFile(const char * path){
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

void MySdManager::writeFile(const char * path, const char * message){
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

void MySdManager::appendFile(const char * path, const char * message){
  LOG(LOG_DEBUG, String("Appending to file: ") + path + "\n\tmessage//:" + String(message) + "//message end");

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

void MySdManager::renameFile(const char * path1, const char * path2){
  LOG(LOG_DEBUG, String("Renaming file ") + path1 + " to " + path2);
  if (fs.rename(path1, path2)) {
    LOG(LOG_DEBUG, "File renamed");
  } else {
    LOG(LOG_ERROR, String("Failed renaming file ") + path1 + " to " + path2);
  }
}

void MySdManager::deleteFile(const char * path){
  LOG(LOG_DEBUG, String("Deleting file: ") + path);
  if(fs.remove(path)){
    LOG(LOG_DEBUG, "File deleted");
  } else {
    LOG(LOG_ERROR, String("Failed deleting file at path: ") + path);
  }
}

void MySdManager::testFileIO(const char * path){
  File file = fs.open(path, FILE_WRITE);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;

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

  file = fs.open(path, FILE_READ);
  if(!file){
    LOG(LOG_ERROR, String("Failed to open file for reading at path: ") + path);
  } else {
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
  }
}

bool MySdManager::readFileToBuffer(const char* path, char* buffer, size_t maxLen) { // reads a file at "path" and stores the string at char buffer[maxLen], returns true if succesfull
  File file = fs.open(path);
  if (!file) {
    LOG(LOG_ERROR, String("Failed to open file at path: ") + path);
    return false;
  }

  size_t len = file.readBytes(buffer, maxLen - 1);
  buffer[len] = '\0';
  file.close();
  return true;
}

MySdManager sdM(SD); // a handy tool to manage the SD file system
MyServer server(sdM); // creates a server
MyLogger* logger = nullptr; /* already reserves memory for the logger, but before we initiate it, we have to connect to the SD
                            Later we use "logger = new MyLogger(sdM);" to write a configurated logger to that same memory which then correctly reffers to the sdM*/

// example use of the SD
void sdTest(){
  pinMode(14, OUTPUT);  // Set GPIO 14 as output for LED
  digitalWrite(14, LOW);  // LED OFF
  sdM.listDir("/", 5);
  sdM.createDir("/csv");
  const char * csv_str = 
    "my_strings,my_numbers\n"
    "hello,5\n"
    "world,10\n";
  sdM.writeFile("/csv/first.csv", csv_str);
  char content[256];
  if (sdM.readFileToBuffer("/csv/first.csv", content, sizeof(content))){
    CSV_Parser cp(content, "sL");
    LOG(LOG_INFO, "Accessing values by column name:");
    char **strings = (char**)cp["my_strings"];
    int32_t *numbers = (int32_t*)cp["my_numbers"];
    
    for(int row = 0; row < cp.getRowsCount(); row++) {
      LOG(LOG_INFO, String(row) + ". String = " + strings[row]);
      LOG(LOG_INFO, String(row) + ". Number = " + numbers[row]);
    }
    digitalWrite(14, HIGH); // LED ON
  }

  //sdM.testFileIO("/ioTest.txt");
  
  //LOG(LOG_INFO, String("Total space: ") + (SD.totalBytes() / (1024 * 1024)) + "MB");
  //LOG(LOG_INFO, String("Used space: ") + (SD.usedBytes() / (1024 * 1024)) + "MB");
}

// connects the SD card
bool sdSetup(){
  /*MOSI	GPIO 23
    MISO	GPIO 19
    SCK	  GPIO 18
    CS	  defined below*/
  if(!SD.begin(5)){ // defines the CS pin
    Serial.println("Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return false;
  }

  String cardTypeStr = "SD Card Type: ";
  switch (cardType) {
    case CARD_MMC:  cardTypeStr += "MMC"; break;
    case CARD_SD:   cardTypeStr += "SDSC"; break;
    case CARD_SDHC: cardTypeStr += "SDHC"; break;
    default:        cardTypeStr += "UNKNOWN"; break;
  }
  Serial.println(cardTypeStr);

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.println(String("SD Card Size: ") + cardSize + "MB");

  return true;
}

void setup(){
  Serial.begin(115200);

  // connecting to SD
  if (!sdSetup()){
    delay(1000);
    ESP.restart(); // full reset
  }else{
    // starting to log on the SD
    // until there is a internet connection, it will log in a "offline_log" file
    logger = new MyLogger(sdM);
    logger->initFilePath(); // tells the logger where the SD is
    LOG(LOG_WARN,"#########################################");
    LOG(LOG_WARN,"#########       RESTARTED       #########");
    LOG(LOG_WARN,"#########################################");

    // connecting to home network using a DNS name
    server.connectToNetwork("water");
    server.webserver.begin();

    // the logger can now use the real time and update the last log
    logger->initDate();

    // debug function to test if SD works
    sdTest();
  }
}

void loop(){
  server.webserver.tick();
}