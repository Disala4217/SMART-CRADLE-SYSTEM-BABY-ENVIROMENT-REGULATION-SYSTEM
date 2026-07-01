#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ================= DHT11 =================
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ================= OLED SPI =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &SPI,
  OLED_DC,
  OLED_RESET,
  OLED_CS
);

// ================= RELAY PINS =================
#define MIST_RELAY     7
#define FAN_RELAY      8
#define HEATER_RELAY   12

// ================= SETTINGS =================
#define TEMP_HIGH   25.0
#define TEMP_LOW    20.0

#define HUMID_LOW   40.0
#define HUMID_HIGH  60.0

// =============================================

void setup() {

  Serial.begin(9600);

  dht.begin();

  // Relay pin modes
  pinMode(MIST_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);
  pinMode(HEATER_RELAY, OUTPUT);

  // All devices OFF
  digitalWrite(MIST_RELAY, LOW);
  digitalWrite(FAN_RELAY, LOW);
  digitalWrite(HEATER_RELAY, LOW);

  // OLED Start
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {

    Serial.println("OLED FAILED");

    while (1);
  }

  // Startup Screen
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(2);

  display.setCursor(15, 20);
  display.println("START");

  display.display();

  delay(2000);

  Serial.println("System Started");
}

// =============================================

void loop() {

  // Read DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // ===== SENSOR ERROR =====
  if (isnan(humidity) || isnan(temperature)) {

    Serial.println("DHT11 ERROR");

    display.clearDisplay();

    display.setTextSize(2);

    display.setCursor(10, 20);
    display.println("ERROR");

    display.display();

    delay(2000);

    return;
  }

  // =========================================
  // DEVICE STATES
  // =========================================

  bool fanState = false;
  bool heaterState = false;
  bool mistState = false;

  // ===== TEMPERATURE CONTROL =====

  if (temperature > TEMP_HIGH) {

    fanState = true;
  }

  if (temperature < TEMP_LOW) {

    heaterState = true;
  }

  // ===== HUMIDITY CONTROL =====

  if (humidity < HUMID_LOW) {

    mistState = true;
  }

  if (humidity > HUMID_HIGH) {

    fanState = true;
  }

  // =========================================
  // APPLY OUTPUTS
  // =========================================

  digitalWrite(FAN_RELAY, fanState);
  digitalWrite(HEATER_RELAY, heaterState);
  digitalWrite(MIST_RELAY, mistState);

  // =========================================
  // SERIAL MONITOR
  // =========================================

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C   ");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // =========================================
  // OLED DISPLAY
  // =========================================

  display.clearDisplay();

  // Title
  display.setTextSize(1);

  display.setCursor(15, 0);
  display.println("TEMP & HUMIDITY");

  // Temperature
  display.setTextSize(2);

  display.setCursor(0, 18);

  display.print("T:");
  display.print(temperature, 1);
  display.print("C");

  // Humidity
  display.setCursor(0, 42);

  display.print("H:");
  display.print(humidity, 1);
  display.print("%");

  display.display();

  // =========================================
  // DELAY
  // =========================================

  delay(2000);
