#include "NTPClient.h"

NTPClient::NTPClient() {
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

long NTPClient::getTime() {
  return getTime(millis());
}

long NTPClient::getTime(long millis) {
  return localEpoc + ((millis - localMillisAtUpdate) / 1000);
}
