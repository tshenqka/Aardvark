#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "eduroam";  // your WPA2 enterprise network SSID (name)
char user[] = "jkarapos@uwaterloo.ca";  // your WPA2 enterprise username
char pass[] = "Pearly*1997";  // your WPA2 enterprise password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

const char server[] = "deployments.jaysee.ca";

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

void serialSend(int n) {
  wifiprint("Signalling ");
  wifiprintln(String(n));
  if (n == 0) {
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  } else if (n == 1) {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
  } else if (n == 2) {
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  } else {
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
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

  pinMode(12, OUTPUT); // data1
  pinMode(13, OUTPUT); // data2

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

  wifiprintln("Connection to eduroam successful");
  wifiprint("Local IP Address: ");
  wifiprintln(String(WiFi.localIP()));
  wifiprint("Signal strength (RSSI): ");
  wifiprintln(String(WiFi.RSSI()));
}

int lastRSSI = 0;

void internetCheck() {
  // Check the network connection once every 5 seconds. Returns when connection to the server is lost
  while (true) {
    lastRSSI = WiFi.RSSI();
    delay(2000);
    bool connected = false;
    for (int i = 0; i < 1; i++) {
      if (client.connect(server, 8080)) {
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
        int currentRSSI = WiFi.RSSI();
        if (currentRSSI > -48) serialSend(3);
        else if (currentRSSI >= lastRSSI) serialSend(2);
        else serialSend(1);
        break;
      }
    }
    if (!connected) return;
  }
}

void loop() {
  while (true) {
    wifiprintln("Establishing first connection");
    while (true) {
      connect();
      serialSend(3);
      delay(3000);
      while (status == WL_CONNECTED) {
        delay(5000);
        internetCheck();
        serialSend(0);
        wifiprintln("Lost connection to the server.");
      }
      wifiprintln("Lost connection to the network. Reattempting connection in 5 seconds...");
      delay(5000);
    }
  }
}
