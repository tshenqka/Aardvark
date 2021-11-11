#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "eduroam";  // your WPA2 enterprise network SSID (name)
char user[] = "c54chung@uwaterloo.ca";  // your WPA2 enterprise username
char pass[] = "";  // your WPA2 enterprise password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

const char server[] = "encyz7exee95.x.pipedream.net";

WiFiClient client;

#define VERBOSE_WIFI false
void wifiprint(char *msg) {
  if (VERBOSE_WIFI) {
    Serial.print(msg);
  }
}

void wifiprint(char *msg) {
  if (VERBOSE_WIFI) {
    Serial.print(msg);
    Serial.println();
  }
}

void setup() {
  // Initialize serial and wait for port to open
  Serial.begin(9600);
  while (!Serial); 

  Serial.println("Begin startup");

  // Abort if there is no wifi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed! Aborting...");
    while (true);
  }

  // Check firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
    Serial.println("Firmware upgrade found.");

  Serial.println("Setup complete");
}

void connect() {
  // Attempt to connect to eduroam
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network with SSID: ");
    Serial.println(ssid);
    // status = WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
    status = WiFi.beginEnterprise(ssid, user, pass);

    // Check the connection every 5 seconds
    delay(5000);
    if (status != WL_CONNECTED) {
      Serial.println("Connection failed. Retrying in 3 seconds.");
      delay(3000);
    }
  }

  Serial.println("Connection successful.");
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal strength (RSSI): ");
  Serial.println(WiFi.RSSI());
}

void internetCheck() {
  // Check the network connection once every 5 seconds. Returns when connection to the server is lost
  while (true) {
    delay(5000);
    bool connected = false;
    for (int i = 0; i < 3; i++) {
      if (client.connect(server, 80)) {
        Serial.print("Connection established to ");
        Serial.println(server);
        // Make a HTTP request:
        client.println("GET / HTTP/1.0");
        client.print("Host: ");
        client.println(server);
        client.println("Accept: */*");
        client.println("Message: Hello, from the arduino!");
        client.print("Current RSSI: ");
        client.println(WiFi.RSSI());
        client.println("Connection: close");
        client.println();
        connected = true;
        Serial.print("HTTP request sent to ");
        Serial.println(server);
        break;
      }
    }
    if (!connected) return;
  }
}

void loop() {
  Serial.println("Establishing first connection");
  while (true) {
    connect();
    delay(3000);
    while (status == WL_CONNECTED) {
      delay(5000);
      internetCheck();
      Serial.println("Lost connection to the server.");
    }
    Serial.println("Lost connection to the network. Reattempting connection in 5 seconds...");
    delay(5000);
  }
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
