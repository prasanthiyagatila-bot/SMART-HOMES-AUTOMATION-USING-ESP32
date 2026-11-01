#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

const int lightPin = 26;
const int fanPin = 27;

// Many relay modules are active LOW.
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

bool lightState = false;
bool fanState = false;

String webpage() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport"
          content="width=device-width, initial-scale=1">
    <title>Smart Home</title>
    <style>
      body {
        font-family: Arial;
        text-align: center;
        background: #eef2f3;
        padding: 20px;
      }
      button {
        padding: 14px 28px;
        margin: 8px;
        font-size: 18px;
        border: none;
        border-radius: 8px;
        background: #1976d2;
        color: white;
      }
    </style>
  </head>
  <body>
    <h1>ESP32 Smart Home</h1>
    <h2>Light: %LIGHT%</h2>
    <a href="/light/on"><button>Light ON</button></a>
    <a href="/light/off"><button>Light OFF</button></a>

    <h2>Fan: %FAN%</h2>
    <a href="/fan/on"><button>Fan ON</button></a>
    <a href="/fan/off"><button>Fan OFF</button></a>
  </body>
  </html>
  )rawliteral";

  html.replace("%LIGHT%", lightState ? "ON" : "OFF");
  html.replace("%FAN%", fanState ? "ON" : "OFF");
  return html;
}

void sendPage() {
  server.send(200, "text/html", webpage());
}

void setup() {
  Serial.begin(115200);

  pinMode(lightPin, OUTPUT);
  pinMode(fanPin, OUTPUT);

  digitalWrite(lightPin, RELAY_OFF);
  digitalWrite(fanPin, RELAY_OFF);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected");
  Serial.print("Open this IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendPage);

  server.on("/light/on", []() {
    lightState = true;
    digitalWrite(lightPin, RELAY_ON);
    sendPage();
  });

  server.on("/light/off", []() {
    lightState = false;
    digitalWrite(lightPin, RELAY_OFF);
    sendPage();
  });

  server.on("/fan/on", []() {
    fanState = true;
    digitalWrite(fanPin, RELAY_ON);
    sendPage();
  });

  server.on("/fan/off", []() {
    fanState = false;
    digitalWrite(fanPin, RELAY_OFF);
    sendPage();
  });

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
