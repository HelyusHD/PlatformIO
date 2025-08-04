#ifndef MY_LOG_H
#define MY_LOG_H

#include <Arduino.h>
#include <SD.h>
#include <time.h>
#include <WiFi.h>

enum LogLevel {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_NONE
};

#define LOG_FILE_PATH "/log.txt"
#define LOG_LEVEL_THRESHOLD LOG_DEBUG

class MyLogger {
public:
  void begin(uint8_t csPin = SS, const char* ntpServer = "pool.ntp.org") {
    if (!SD.begin(csPin)) {
      Serial.println("SD card init failed. Logging to file disabled.");
      sdAvailable = false;
    } else {
      sdAvailable = true;
    }

    // NTP time setup (requires WiFi connection already active)
    configTime(0, 0, ntpServer);  // UTC; adjust timezone if needed
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);  // Europe (with DST)
    tzset();
  }

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
