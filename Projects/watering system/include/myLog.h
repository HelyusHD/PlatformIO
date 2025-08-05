#ifndef MY_LOG_H
#define MY_LOG_H

#include <Arduino.h>
#include <SD.h>
#include <time.h>
#include <WiFi.h>


// used to define debug level
enum LogLevel {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_NONE
};

#define LOG_FILE_PATH "/log.txt" // file path on SD card where the log gets stored
#define LOG_LEVEL_THRESHOLD LOG_DEBUG // configuration for the used debug level

class MyLogger {
public:
  // binds logger to a csPin. Thats a connection on the SD module used to transfere data
  // requires WiFi connection already active
  void begin(uint8_t csPin = SS, const char* ntpServer = "pool.ntp.org") {
    if (!SD.begin(csPin)) {
      Serial.println("SD card init failed. Logging to file disabled.");
      sdAvailable = false;
    } else {
      Serial.println("SUCCESS: Logging to SD");
      sdAvailable = true;
    }

    // NTP time setup (requires WiFi connection already active)
    configTime(0, 0, ntpServer);  // UTC; adjust timezone if needed
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);  // Europe (with DST)
    tzset();
  }

  // logs to SD
  void log(LogLevel level, const String& message) {
    if (level < LOG_LEVEL_THRESHOLD) return;

    String levelStr = levelToString(level);
    String timestamp = getTimestamp();
    String logLine = "[" + timestamp + "] [" + levelStr + "] " + message;

    Serial.println(logLine);

    if (sdAvailable) {
      File file = SD.open(LOG_FILE_PATH, FILE_APPEND);
      if (file) {
        file.println(logLine);
        file.close();
      }
    }
  }

private:
  bool sdAvailable = false;

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
};

extern MyLogger logger;
#define LOG(level, msg) logger.log(level, String(msg))

#endif  // MY_LOG_H
