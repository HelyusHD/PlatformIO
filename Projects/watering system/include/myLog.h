#ifndef MY_LOG_H
#define MY_LOG_H

#include <Arduino.h>
#include <time.h>
#include <MySdManager.h>


// used to define debug level
enum LogLevel {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_NONE
};

#define LOG_DIR "/logs" // file path on SD card where the log gets stored
#define LOG_LEVEL_THRESHOLD LOG_DEBUG // configuration for the used debug level

class MyLogger {
public:
  // binds logger to a csPin. Thats a connection on the SD module used to transfere data
  MyLogger(MySdManager& SdM_) : SdM(SdM_) {}

  // requires WiFi connection already active
  // will load the time and date from a server
  void initFilePath() {
    // Ensure /logs directory exists
    SdM.getFS().mkdir(LOG_DIR);
    logFilePath = String(LOG_DIR) + "/log.txt";
  }

  void initDate(const char* ntpServer = "pool.ntp.org"){
    configTime(0, 0, ntpServer);
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();
    Serial.println("SUCCESS: Logging to SD");
    online = true;
    // Rotate existing log file
    if (SdM.getFS().exists(logFilePath)) {
      String archivePath = String(LOG_DIR) + "/log_" + getTimestampForFilename() + ".txt";
      SdM.renameFile(logFilePath.c_str(), archivePath.c_str());
    }else{
      File file = SdM.getFS().open(logFilePath, FILE_WRITE);
      file.close();
    }

    cleanupOldLogs(7); // Deletes logs older than 7 days
  }

  // logs to SD
  void log(LogLevel level, const String& message, const char* file_name = nullptr, const int line = -1, const char* func = nullptr) {
    if (level < LOG_LEVEL_THRESHOLD) return;

    String levelStr = levelToString(level);
    String timestamp;
    if (online){
      timestamp = getTimestamp();
    }else{
      timestamp = "offline";
    }
    String logLine = "[" + timestamp + "] [" + levelStr + "] " + message;
    if(file_name){logLine += String(" | in ") + file_name;}
    if(line != -1){logLine += String(" :") + line;}
    if(func){logLine += String(" in ") + func;}

    Serial.println(logLine);

    if (!SdM.getFS().exists(logFilePath)){
      File file = SdM.getFS().open(logFilePath, FILE_WRITE);
      file.println(logLine);
      file.close();
    }else{
      File file = SdM.getFS().open(logFilePath, FILE_APPEND);
      if (file) {
        file.println(logLine);
        file.close();
      }
    }
  }

private:
  bool online = false;
  MySdManager &SdM;
  String logFilePath;

  String levelToString(LogLevel level) {
    switch (level) {
      case LOG_DEBUG: return "DEBUG";
      case LOG_INFO:  return "INFO";
      case LOG_WARN:  return "WARN";
      case LOG_ERROR: return "ERROR";
      default:        return "UNKNOWN";
    }
  }

  String getTimestamp() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char buf[20];
      strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
      return String(buf);
    } else {
      // fallback to seconds since boot
      return String(millis() / 1000) + "s";
    }
  }

  String getTimestampForFilename() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char buf[32];
      strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &timeinfo);
      return String(buf);
    }
    return "unknown";
  }

  String getDateOnly() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char buf[11];
      strftime(buf, sizeof(buf), "%Y-%m-%d", &timeinfo);
      return String(buf);
    }
    return "unknown";
  }

  void cleanupOldLogs(int maxDays) {
    File root = SdM.getFS().open(LOG_DIR);
    if (!root || !root.isDirectory()) return;

    File file = root.openNextFile();
    while (file) {
      String name = file.name();
      int logPrefixIndex = name.indexOf("/log_");
      if (logPrefixIndex >= 0 && name.endsWith(".txt")) {
        String timestampPart = name.substring(logPrefixIndex + 5);  // after "log_"
        int dotIndex = timestampPart.lastIndexOf(".txt");
        if (dotIndex > 0) {
          timestampPart = timestampPart.substring(0, dotIndex); // remove ".txt"
          String dateOnly = timestampPart.substring(0, 10);     // YYYY-MM-DD
          if (isOlderThan(dateOnly, maxDays)) {
            SdM.deleteFile(name.c_str());
          }
        }
      }
      file = root.openNextFile();
    }
  }

  bool isOlderThan(const String& dateStr, int days) {
    struct tm fileDate = {};
    strptime(dateStr.c_str(), "%Y-%m-%d", &fileDate);
    time_t fileTime = mktime(&fileDate);

    time_t now;
    time(&now);

    double seconds = difftime(now, fileTime);
    return (seconds > days * 86400);
  }
};

extern MyLogger* logger;
#define LOG(level, msg) logger->log(level, String(msg), __FILE__, __LINE__, __func__)

#endif  // MY_LOG_H
