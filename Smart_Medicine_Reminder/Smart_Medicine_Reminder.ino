#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <WiFi.h>
#include <HTTPClient.h>


// ---------------- OLED ----------------

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// ---------------- RTC ----------------

RTC_DS3231 rtc;


// ---------------- Wi-Fi ----------------

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "WIFI_PASSWORD";


// ---------------- Telegram ----------------

String botToken = "YOUR_BOT_TOKEN";
String chatID = "YOUR_CHAT_ID";


// ---------------- Pins ----------------

const int buzzerPin = 4;

const int ledPin1 = 2;
const int ledPin2 = 15;
const int ledPin3 = 5;

const int buttonPin1 = 18;
const int buttonPin2 = 19;
const int buttonPin3 = 23;


// ---------------- Alarm States ----------------

bool alarm1Active = false;
bool alarm2Active = false;
bool alarm3Active = false;

bool alarm1Done = false;
bool alarm2Done = false;
bool alarm3Done = false;


// ---------------- Missed Dosage ----------------

bool missedDosage1 = false;
bool missedDosage2 = false;
bool missedDosage3 = false;


// ---------------- Notification Flags ----------------

bool notificationSent1 = false;
bool notificationSent2 = false;
bool notificationSent3 = false;


// ---------------- Missed Message Timer ----------------

unsigned long missedMessageStart = 0;

const unsigned long missedMessageDuration = 5000;


// ---------------- Buzzer Timer ----------------

unsigned long buzzerPreviousMillis = 0;

const unsigned long buzzerOnTime = 2000;
const unsigned long buzzerOffTime = 2000;

bool buzzerState = false;


// ---------------- Telegram Function ----------------

void sendTelegramMessage(String message)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String url = "https://api.telegram.org/bot" +
                 botToken +
                 "/sendMessage?chat_id=" +
                 chatID +
                 "&text=" +
                 message;

    http.begin(url);

    int httpResponseCode = http.GET();

    Serial.print("Telegram Response: ");
    Serial.println(httpResponseCode);

    http.end();
  }
  else
  {
    Serial.println("WiFi not connected");
  }
}


// ---------------- SETUP ----------------

void setup()
{
  Serial.begin(115200);


  // OLED

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED not found");

    while (1);
  }

  display.clearDisplay();
  display.display();


  // Pins

  pinMode(buzzerPin, OUTPUT);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);


  digitalWrite(buzzerPin, LOW);

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);


  // RTC

  if (!rtc.begin())
  {
    Serial.println("RTC not found");

    while (1);
  }

  // Set RTC time only once if needed
  //rtc.adjust(DateTime(2026, 9, 6, 22, 10, 0));


  // Wi-Fi

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");

  Serial.println(WiFi.localIP());
}


// ---------------- LOOP ----------------

void loop()
{
  DateTime now = rtc.now();


  // =====================================================
  // DISPLAY
  // =====================================================

  display.clearDisplay();

  display.setTextColor(WHITE);


  // -------- ALARM 1 ACTIVE --------

  if (alarm1Active)
  {
    display.setTextSize(1);

    display.setCursor(10, 10);

    display.print("TAKE MEDICINE");

    display.setCursor(15, 30);

    display.print("PARACETAMOL");
  }


  // -------- ALARM 1 DONE --------

  else if (alarm1Done)
  {
    display.setTextSize(2);

    display.setCursor(10, 10);

    display.print("MEDICINE");

    display.setCursor(15, 35);

    display.print("TAKEN!");

    alarm1Done = false;
  }


  // -------- ALARM 2 ACTIVE --------

  else if (alarm2Active)
  {
    display.setTextSize(1);

    display.setCursor(10, 10);

    display.print("TAKE MEDICINE");

    display.setCursor(15, 30);

    display.print("DOLO 65");
  }


  // -------- ALARM 2 DONE --------

  else if (alarm2Done)
  {
    display.setTextSize(2);

    display.setCursor(10, 10);

    display.print("MEDICINE");

    display.setCursor(15, 35);

    display.print("TAKEN!");

    alarm2Done = false;
  }


  // -------- ALARM 3 ACTIVE --------

  else if (alarm3Active)
  {
    display.setTextSize(1);

    display.setCursor(10, 10);

    display.print("TAKE MEDICINE");

    display.setCursor(15, 30);

    display.print("B-HOLD");
  }


  // -------- ALARM 3 DONE --------

  else if (alarm3Done)
  {
    display.setTextSize(2);

    display.setCursor(10, 10);

    display.print("MEDICINE");

    display.setCursor(15, 35);

    display.print("TAKEN!");

    alarm3Done = false;
  }


  // -------- MISSED DOSAGE --------

  else if (missedDosage1 ||
           missedDosage2 ||
           missedDosage3)
  {
    display.setTextSize(1);

    display.setCursor(10, 20);

    display.print("MISSED DOSAGE");
  }


  // -------- NORMAL CLOCK --------

  else
  {
    display.setTextSize(2);

    display.setCursor(16, 0);

    display.print(now.hour());

    display.print(":");

    display.print(now.minute());

    display.print(":");

    display.print(now.second());
  }

  display.display();


  // =====================================================
  // ALARM 1
  // Example: 18:18
  // =====================================================

  if (now.hour() == 22 &&
      now.minute() == 14 &&
      now.second() == 0 &&
      !alarm1Done &&
      !alarm1Active &&
      !missedDosage1)
  {
    alarm1Active = true;

    buzzerPreviousMillis = millis();

    notificationSent1 = false;
  }


  // -------- ALARM 1 ACTIVE --------

  if (alarm1Active)
  {
    digitalWrite(ledPin1, HIGH);


    // Buzzer using millis()

    unsigned long currentMillis = millis();


    if (buzzerState == false &&
        currentMillis - buzzerPreviousMillis >= buzzerOffTime)
    {
      buzzerState = true;

      buzzerPreviousMillis = currentMillis;

      digitalWrite(buzzerPin, HIGH);
    }


    if (buzzerState == true &&
        currentMillis - buzzerPreviousMillis >= buzzerOnTime)
    {
      buzzerState = false;

      buzzerPreviousMillis = currentMillis;

      digitalWrite(buzzerPin, LOW);
    }


    // Button pressed

    if (digitalRead(buttonPin1) == LOW)
    {
      alarm1Active = false;

      alarm1Done = true;

      digitalWrite(ledPin1, LOW);

      digitalWrite(buzzerPin, LOW);

      buzzerState = false;
    }


    // 10-minute missed dosage

    if (now.minute() >= 15 &&
        !missedDosage1)
    {
      missedDosage1 = true;

      missedMessageStart = millis();

      alarm1Active = false;

      digitalWrite(ledPin1, LOW);

      digitalWrite(buzzerPin, LOW);


      // Telegram notification

      if (!notificationSent1)
      {
        sendTelegramMessage(
          "Medicine%20Missed%0AParacetamol"
        );

        notificationSent1 = true;
      }
    }
  }


  // =====================================================
  // ALARM 2
  // Example: 19:17
  // =====================================================

  if (now.hour() == 22 &&
      now.minute() == 17 &&
      now.second() == 0 &&
      !alarm2Done &&
      !alarm2Active &&
      !missedDosage2)
  {
    alarm2Active = true;

    buzzerPreviousMillis = millis();

    notificationSent2 = false;
  }


  // -------- ALARM 2 ACTIVE --------

  if (alarm2Active)
  {
    digitalWrite(ledPin2, HIGH);


    unsigned long currentMillis = millis();


    if (buzzerState == false &&
        currentMillis - buzzerPreviousMillis >= buzzerOffTime)
    {
      buzzerState = true;

      buzzerPreviousMillis = currentMillis;

      digitalWrite(buzzerPin, HIGH);
    }


    if (buzzerState == true &&
        currentMillis - buzzerPreviousMillis >= buzzerOnTime)
    {
      buzzerState = false;

      buzzerPreviousMillis = currentMillis;

      digitalWrite(buzzerPin, LOW);
    }


    if (digitalRead(buttonPin2) == LOW)
    {
      alarm2Active = false;

      alarm2Done = true;

      digitalWrite(ledPin2, LOW);

      digitalWrite(buzzerPin, LOW);

      buzzerState = false;
    }


    // 10-minute missed dosage

    if (now.hour() == 22 &&
        now.minute() >= 18 &&
        !missedDosage2)
    {
      missedDosage2 = true;

      missedMessageStart = millis();

      alarm2Active = false;

      digitalWrite(ledPin2, LOW);

      digitalWrite(buzzerPin, LOW);


      if (!notificationSent2)
      {
        sendTelegramMessage(
          "Medicine%20Missed%0ADolo%2065"
        );

        notificationSent2 = true;
      }
    }
  }


  // =====================================================
  // ALARM 3
  // Example: 19:20
  // =====================================================

  if (now.hour() == 20 &&
      now.minute() == 15 &&
      now.second() == 0 &&
      !alarm3Done &&
      !alarm3Active &&
      !missedDosage3)
  {
    alarm3Active = true;

    buzzerPreviousMillis = millis();

    notificationSent3 = false;
  }


  // -------- ALARM 3 ACTIVE --------

  if (alarm3Active)
  {
    digitalWrite(ledPin3, HIGH);


    unsigned long currentMillis = millis();


    if (buzzerState == false &&
        currentMillis - buzzerPreviousMillis >= buzzerOffTime)
    {
      buzzerState = true;

      buzzerPreviousMillis = currentMillis;

      digitalWrite(buzzerPin, HIGH);
    }


    if (buzzerState == true &&
        currentMillis - buzzerPreviousMillis >= buzzerOnTime)
    {
      buzzerState = false;

      buzzerPreviousMillis = currentMillis;

      digitalWrite(buzzerPin, LOW);
    }


    if (digitalRead(buttonPin3) == LOW)
    {
      alarm3Active = false;

      alarm3Done = true;

      digitalWrite(ledPin3, LOW);

      digitalWrite(buzzerPin, LOW);

      buzzerState = false;
    }


    // 10-minute missed dosage

    if (now.hour() == 20 &&
        now.minute() >= 25 &&
        !missedDosage3)
    {
      missedDosage3 = true;

      missedMessageStart = millis();

      alarm3Active = false;

      digitalWrite(ledPin3, LOW);

      digitalWrite(buzzerPin, LOW);


      if (!notificationSent3)
      {
        sendTelegramMessage(
          "Medicine%20Missed%0AB-HOLD"
        );

        notificationSent3 = true;
      }
    }
  }


  // =====================================================
  // CLEAR MISSED DOSAGE MESSAGE AFTER 5 SECONDS
  // =====================================================

  if (missedDosage1 ||
      missedDosage2 ||
      missedDosage3)
  {
    if (millis() - missedMessageStart >= missedMessageDuration)
    {
      missedDosage1 = false;

      missedDosage2 = false;

      missedDosage3 = false;
    }
  }


  // =====================================================
  // RESET DAILY FLAGS
  // =====================================================

  // This part can be improved later for a proper daily reset.
}