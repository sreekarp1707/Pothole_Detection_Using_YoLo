#include <WiFi.h>

// ================= WIFI =================
const char* ssid = "xxxxxxxxx";
const char* password = "xxxxxx";

WiFiServer server(80);

// ================= L298N PINS =================
#define ENA 25
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12
#define ENB 33

// Current motor speed (0-255)
int motorSpeed = 130;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // PWM setup
  ledcAttach(ENA, 5000, 8);
  ledcAttach(ENB, 5000, 8);

  // Start safely stopped
  stopCar();

  // Connect to WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("HTTP server started");
}

// ================= MAIN LOOP =================
void loop() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  delay(10);

  String request = "";

  if (client.available()) {
    request = client.readStringUntil('\n');
  }

  Serial.println("Request: " + request);

  // Send HTTP response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println("OK");

  client.stop();

  // ================= COMMAND HANDLING =================

  // Speed command: /cmd?cmd=speed:130
  if (request.indexOf("/cmd?cmd=speed:") != -1) {
    int position = request.indexOf("speed:");
    String speedValue = request.substring(position + 6);
    speedValue.trim();

    int newSpeed = speedValue.toInt();

    if (newSpeed >= 0 && newSpeed <= 255) {
      motorSpeed = newSpeed;

      Serial.print("Speed changed to: ");
      Serial.println(motorSpeed);
    }
  }

  // Forward
  else if (request.indexOf("/cmd?cmd=forward") != -1) {
    Serial.println("-> FORWARD");
    forward();
  }

  // Left: in-place rotation
  else if (request.indexOf("/cmd?cmd=left") != -1) {
    Serial.println("-> LEFT");
    left();
  }

  // Right: in-place rotation
  else if (request.indexOf("/cmd?cmd=right") != -1) {
    Serial.println("-> RIGHT");
    right();
  }

  // Stop
  else if (request.indexOf("/cmd?cmd=stop") != -1) {
    Serial.println("-> STOP");
    stopCar();
  }

  else {
    Serial.println("-> Unknown command");
  }
}

// ================= MOTOR FUNCTIONS =================

void forward() {
  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left() {
  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed);

  // Left motor backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Right motor forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed);

  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motor backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopCar() {
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
