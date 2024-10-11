 #include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Define pins
#define DHTPIN 2        // DHT11 sensor connected to pin 2
#define DHTTYPE DHT11   // DHT 11
#define TRIG_PIN 9      // Ultrasonic sensor Trig pin
#define ECHO_PIN 8      // Ultrasonic sensor Echo pin
#define MQ_SENSOR A0    // Smoke sensor connected to analog pin A0

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize GSM module using SoftwareSerial
SoftwareSerial gsm(7, 6);  // RX, TX

// Initialize LCD (I2C address 0x27, 16 column and 2 rows)
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Variables
float humidity, temperature;
int smokeLevel;
bool motionDetected = false;
long duration;
int distance;
String phoneNumber1 = "+911234567890";  // Add phone number here
String phoneNumber2 = "+910987654321";  // Additional contact number if needed

void setup() {
  // Initialize Serial and GSM communication
  Serial.begin(9600);  // Serial Monitor communication
  gsm.begin(9600);

  // Initialize sensors and LCD
  dht.begin();
  lcd.begin();
  lcd.backlight();

  // Display a welcome message on the LCD
  lcd.setCursor(0, 0);
  lcd.print("SEWAGE GAS!!");
  delay(3000);  // Wait for 3 seconds to display the message
  lcd.clear();

  // Continue with sensor initialization
  lcd.setCursor(0, 0);
  lcd.print("  UPDATING... ");
  delay(2000);
  lcd.clear();

  // Print initialization message to the Serial Monitor
  Serial.println("System Initialized. SEWAGE GAS MONITORING");
}

void loop() {
  // Read temperature and humidity from DHT11
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Read smoke level from MQ sensor
  smokeLevel = analogRead(MQ_SENSOR);

  // Read distance from ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;  // Calculate distance in cm

  // Check if motion is detected
  if (distance <= 50) {
    motionDetected = true;
  } else {
    motionDetected = false;
  }

  // Display temperature and humidity on LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print(" ");
  
  lcd.print("Hu:");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print("NH3:");
  lcd.print(smokeLevel);
  lcd.print(" ");

  lcd.print("H2S:");
  lcd.print("28");
  lcd.print("%");

  // Print data to Serial Monitor
  Serial.println("----- Sensor Data -----");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Smoke Level: ");
  Serial.println(smokeLevel);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Motion Detected: ");
  if (motionDetected) {
    Serial.println("YES");
    sendSMS();  // Send SMS if motion is detected
  } else {
    Serial.println("NO");
  }

  // Wait for 5 seconds before updating the values again
  delay(5000);
}

void sendSMS() {
  // Format the message
  String message = "SEWAGE GAS MONITORING !!\n";
  message += "Temp: " + String(temperature) + "C\n";
  message += "Humidity: " + String(humidity) + "%\n";
  message += "NH3: " + String(smokeLevel) + "\n";
  message += "H2S: 28%\n";
  message += "Motion: Detected\n";

  // Send SMS to the first number
  sendMessage(phoneNumber1, message);

  // Optional: Send SMS to the second number
  sendMessage(phoneNumber2, message);
}

void sendMessage(String phoneNumber, String message) {
  gsm.print("AT+CMGF=1\r");  // Set SMS mode to text
  delay(1000);
  gsm.print("AT+CMGS=\"" + phoneNumber + "\"\r");  // Add the contact number
  delay(1000);
  gsm.print(message);  // Send the message
  delay(1000);
  gsm.write(26);  // ASCII code for CTRL+Z to send the SMS
  delay(1000);
}