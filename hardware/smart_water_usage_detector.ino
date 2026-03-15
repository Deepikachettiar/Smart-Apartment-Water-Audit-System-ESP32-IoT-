/******************** BLYNK CONFIG ********************/
#define BLYNK_TEMPLATE_ID "TMPL3_CGQ8mvP"
#define BLYNK_TEMPLATE_NAME "smart water usage detection"
#define BLYNK_AUTH_TOKEN "wab4YvVc8SU1D3CyqwgQr5Cj8JD3LpGX"

/******************** LIBRARIES ********************/
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

/******************** WIFI ********************/
char ssid[] = "Redmi K50i";
char pass[] = "Indumouli1975";

/******************** TIMER ********************/
BlynkTimer timer;

/******************** FLOW PINS ********************/
#define FLOW1 2
#define FLOW2 4
#define FLOW3 25
#define FLOW4 33

/******************** PULSE COUNTERS ********************/
volatile uint32_t pulse1 = 0;
volatile uint32_t pulse2 = 0;
volatile uint32_t pulse3 = 0;
volatile uint32_t pulse4 = 0;

/******************** TOTAL STORAGE ********************/
float total1 = 0, total2 = 0, total3 = 0, total4 = 0;

/******************** LEAK VARIABLES ********************/
unsigned long leakStartTime = 0;
bool leakActive = false;

/******************** INTERRUPTS ********************/
void IRAM_ATTR count1() {
  pulse1++;
}
void IRAM_ATTR count2() {
  pulse2++;
}
void IRAM_ATTR count3() {
  pulse3++;
}
void IRAM_ATTR count4() {
  pulse4++;
}

/******************** BLYNK STATUS ********************/
BLYNK_CONNECTED() {
  Serial.println("Blynk Connected!");
}

BLYNK_DISCONNECTED() {
  Serial.println("Blynk Disconnected!");
}

/******************** MAIN FUNCTION ********************/
void sendSensorData() {
  if (!Blynk.connected()) {
    Serial.println("Trying to reconnect to Blynk...");
    Blynk.connect();
  }

  noInterrupts();
  uint32_t p1 = pulse1;
  uint32_t p2 = pulse2;
  uint32_t p3 = pulse3;
  uint32_t p4 = pulse4;
  pulse1 = pulse2 = pulse3 = pulse4 = 0;
  interrupts();

  float lps1 = p1 / 450.0;
  float lps2 = p2 / 450.0;
  float lps3 = p3 / 450.0;
  float lps4 = p4 / 450.0;

  total1 += lps1;
  total2 += lps2;
  total3 += lps3;
  total4 += lps4;

  float buildingTotal = total1 + total2 + total3 + total4;

  // Send only if connected
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, lps1);
    Blynk.virtualWrite(V1, lps2);
    Blynk.virtualWrite(V2, lps3);
    Blynk.virtualWrite(V3, lps4);
    Blynk.virtualWrite(V6, total1);
    Blynk.virtualWrite(V7, total2);
    Blynk.virtualWrite(V8, total3);
    Blynk.virtualWrite(V9, total4);
    Blynk.virtualWrite(V4, buildingTotal);
  }

  /********** LEAK DETECTION **********/
  float totalFlow = lps1 + lps2 + lps3 + lps4;

  if (totalFlow > 0.05) {
    if (!leakActive) {
      leakStartTime = millis();
      leakActive = true;
    }

    if (millis() - leakStartTime > 60000) {
      if (Blynk.connected()) Blynk.virtualWrite(V10, 1);
      Serial.println("⚠ LEAK DETECTED!");
    } else {
      if (Blynk.connected()) Blynk.virtualWrite(V10, 0);
    }
  } else {
    leakActive = false;
    if (Blynk.connected()) Blynk.virtualWrite(V10, 0);
  }

  /********** SERIAL DEBUG **********/
  Serial.print("L/s: ");
  Serial.print(lps1);
  Serial.print(" | ");
  Serial.print(lps2);
  Serial.print(" | ");
  Serial.print(lps3);
  Serial.print(" | ");
  Serial.print(lps4);
  Serial.print(" || Total: ");
  Serial.print(buildingTotal);

  Serial.print(" || Blynk: ");
  Serial.println(Blynk.connected() ? "ONLINE" : "OFFLINE");
}

/******************** SETUP ********************/
void setup() {
  Serial.begin(115200);

  pinMode(FLOW1, INPUT_PULLUP);
  pinMode(FLOW2, INPUT_PULLUP);
  pinMode(FLOW3, INPUT_PULLUP);
  pinMode(FLOW4, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(FLOW1), count1, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW2), count2, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW3), count3, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW4), count4, FALLING);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  timer.setInterval(1000L, sendSensorData);
}

/******************** LOOP ********************/
void loop() {
  Blynk.run();
  timer.run();
}
