#include "NTPClient.h"

NTPClient::NTPClient(float utcOffset) {
  this->utcOffset = utcOffset;
}

void NTPClient::update() {
  WiFiClient client;

  if (!client.connect(ntpServerName, httpPort)) {
    Serial.println("Failed to connecto to NTP server");
    return;
  }

  client.print(String("GET / HTTP/1.1\r\n") +
      String("Host: www.google.com\r\n") +
      String("Connection: close\r\n\r\n"));

  int repeatCounter = 0;
  while(!client.available() && repeatCounter < 10) {
    delay(1000);
    Serial.println(".");
    repeatCounter++;
  }

  String line;

  int size = 0;
  client.setNoDelay(false);
  while(client.connected()) {
    while((size = client.available()) > 0) {
			line = client.readStringUntil('\n');
      line.toUpperCase();
      // date: Thu, 19 Nov 2015 20:25:40 GMT
      if (line.startsWith("DATE: ")) {
        int parsedHours = line.substring(23, 25).toInt();
        int parsedMinutes = line.substring(26, 28).toInt();
        int parsedSeconds = line.substring(29, 31).toInt();
        Serial.println(String(parsedHours) + ":" + String(parsedMinutes) + ":" + String(parsedSeconds));

        localEpoc = (parsedHours * 60 * 60 + parsedMinutes * 60 + parsedSeconds);
        Serial.println(localEpoc);
        localMillisAtUpdate = millis();
        client.stop();
      }
    }
  }
}

String NTPClient::getHours() {
    if (localEpoc == 0) {
      return "--";
    }
    int hours = ((getTime()  % 86400L) / 3600) % 24;
    if (hours < 10) {
      return "0" + String(hours);
    }
    return String(hours); // print the hour (86400 equals secs per day)

}
String NTPClient::getMinutes() {
    if (localEpoc == 0) {
      return "--";
    }
    int minutes = ((getTime() % 3600) / 60);
    if (minutes < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      return "0" + String(minutes);
    }
    return String(minutes);
}
String NTPClient::getSeconds() {
    if (localEpoc == 0) {
      return "--";
    }
    int seconds = getTime() % 60;
    if ( seconds < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      return "0" + String(seconds);
    }
    return String(seconds);
}

String NTPClient::toString() {
  return getHours() + ":" + getMinutes() + ":" + getSeconds();
}

long NTPClient::getUtcTime() {
  return localEpoc + ((millis() - localMillisAtUpdate) / 1000);
}

long NTPClient::getTime() {
  return round(getUtcTime() + 3600 * utcOffset + 86400L) % 86400L;
}
