#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "eduroam";  // your WPA2 enterprise network SSID (name)
char user[] = "c54chung@uwaterloo.ca";  // your WPA2 enterprise username
char pass[] = "";  // your WPA2 enterprise password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

const char server[] = "encyz7exee95.x.pipedream.net";

WiFiClient client;

#define VERBOSE_WIFI true
void wifiprint(String msg) {
  if (VERBOSE_WIFI) {
    Serial.print(msg);
  }
}

void wifiprintln(String msg) {
  if (VERBOSE_WIFI) {
    Serial.print(msg);
    Serial.println();
  }
}

void setup() {
  // Initialize serial and wait for port to open
  Serial.begin(9600);
  while (!Serial); 

  wifiprintln("Begin startup");

  // Abort if there is no wifi module
  if (WiFi.status() == WL_NO_MODULE) {
    wifiprintln("Communication with WiFi module failed! Aborting...");
    while (true);
  }

  // Check firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
    wifiprintln("Firmware upgrade found.");

  wifiprintln("Setup complete");
}

void connect() {
  // Attempt to connect to eduroam
  while (status != WL_CONNECTED) {
    wifiprint("Attempting to connect to network with SSID: ");
    wifiprintln(ssid);
    // status = WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
    status = WiFi.beginEnterprise(ssid, user, pass);

    // Check the connection every 5 seconds
    delay(5000);
    if (status != WL_CONNECTED) {
      wifiprintln("Connection failed. Retrying in 3 seconds.");
      delay(3000);
    }
  }

  wifiprintln("Connection successful.");
  byte mac[6];
  WiFi.macAddress(mac);
  wifiprint("MAC address: ");
  printMacAddress(mac);
  wifiprint("Local IP Address: ");
  wifiprintln(String(WiFi.localIP()));
  wifiprint("Signal strength (RSSI): ");
  wifiprintln(String(WiFi.RSSI()));
}

void internetCheck() {
  // Check the network connection once every 5 seconds. Returns when connection to the server is lost
  while (true) {
    delay(5000);
    bool connected = false;
    for (int i = 0; i < 3; i++) {
      if (client.connect(server, 80)) {
        wifiprint("Connection established to ");
        wifiprintln(server);
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
        wifiprint("HTTP request sent to ");
        wifiprintln(server);
        break;
      }
    }
    if (!connected) return;
  }
}

void loop() {
  wifiprintln("Establishing first connection");
  while (true) {
    connect();
    delay(3000);
    while (status == WL_CONNECTED) {
      delay(5000);
      internetCheck();
      wifiprintln("Lost connection to the server.");
    }
    wifiprintln("Lost connection to the network. Reattempting connection in 5 seconds...");
    delay(5000);
  }
}

void printMacAddress(byte mac[]) {
//  for (int i = 5; i >= 0; i--) {
//    if (mac[i] < 16) {
//      wifiprint("0");
//    }
//    wifiprint(mac[i], HEX);
//    if (i > 0) {
//      wifiprint(":");
//    }
//  }
//  wifiprintln();
}
