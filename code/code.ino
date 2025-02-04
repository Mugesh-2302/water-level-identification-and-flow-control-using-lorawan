#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "mugesh";
const char* password = "098765432";
unsigned long channelID = 2737629;
const char* writeAPIKey = "7ZL65552JCQAX1QH";

WiFiClient client;

int waterLevelPin = 34;
int waterLevelValue = 0;
int maxTankCapacity = 35;
int buzzerPin = 15;
int motorPin = 14;  // Define motor pin (set this to an available GPIO pin on your board)

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(motorPin, OUTPUT);  // Set motor pin as output

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  ThingSpeak.begin(client);
  
  digitalWrite(buzzerPin, LOW);
  digitalWrite(motorPin, LOW);  // Ensure motor is off initially
}

void loop() {
  waterLevelValue = analogRead(waterLevelPin);
  int scaledWaterLevel = map(waterLevelValue, 0, 4095, 0, 100);

  String waterLevelStatus;
  if (scaledWaterLevel >= maxTankCapacity) {
    waterLevelStatus = "Full";
    digitalWrite(buzzerPin, HIGH);
  }
  
    else {
    waterLevelStatus = "Not Full";
    digitalWrite(buzzerPin, LOW);
  }
 

  // Motor control based on water level
  if (scaledWaterLevel <= 10) {
    digitalWrite(motorPin, HIGH);  // Turn on motor if water level is 10 or below
  } else {
    digitalWrite(motorPin, LOW);   // Turn off motor if water level is above 10
  }

  Serial.print("Scaled Water Level Value: ");
  Serial.println(scaledWaterLevel);
  Serial.print("Water Level Status: ");
  Serial.println(waterLevelStatus);

  ThingSpeak.setField(1, scaledWaterLevel);
  ThingSpeak.setField(2, waterLevelStatus);

  int responseCode = ThingSpeak.writeFields(channelID, writeAPIKey);
  if (responseCode == 200) {
    Serial.println("Update successful.");
  } else {
    Serial.print("Problem updating data. HTTP error code: ");
    Serial.println(responseCode);
  }

  delay(15000);
}
