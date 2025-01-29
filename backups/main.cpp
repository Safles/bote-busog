#include <Servo.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Pushbutton.h>

// Pins
#define relay_pin 6
#define capacitive_pin 7
#define inductive_pin 8
#define servo_pin 9
// #define reset_pin 10
#define claim_pin 5

const bool servo_enabled = true;
const int servo_default = 90;
const int wifi_start_timer = 30;
const int wifi_add_amount = 180;

bool relay_enabled = false;
int number_bottles = 0;
unsigned long wifi_time_amount = 0;
unsigned long delayStart = 0;

// Declarations
Servo main_servo;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Pushbutton claim(claim_pin);

// Functions
void displayMain();
void displayPoints();
void displayDetected();
void displayWarning();
void displayAward();
void systemReward();
void openServo();
void startRelay();

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  pinMode(relay_pin, OUTPUT);
  pinMode(capacitive_pin, INPUT);
  pinMode(inductive_pin, INPUT);
  pinMode(claim_pin, INPUT_PULLUP);

  main_servo.attach(servo_pin);
  main_servo.write(servo_default);

  lcd.setCursor(0, 0);
  lcd.print("LCD Initiated");
  delay(2000);
  lcd.clear();

  Serial.println("Starting");

  displayMain();
}

void loop()
{
  if (relay_enabled && ((millis() - delayStart) >= (wifi_time_amount * 1000)))
  {
    relay_enabled = false;
    digitalWrite(relay_pin, LOW);
    lcd.setCursor(0, 0);
    lcd.print("Wi-Fi Time Ended");
    lcd.setCursor(0, 1);
    lcd.print("Thank you!      ");
    wifi_time_amount = 0;
    delay(1000);
  }
  Serial.println(millis() - delayStart);
  Serial.println(wifi_time_amount * 1000);

  if (claim.isPressed())
  {
    if (number_bottles >= 2)
    {
      if (relay_enabled)
      {
        displayAward();
        wifi_time_amount += wifi_add_amount;
      }
      else
      {
        displayAward();
        startRelay();
      }
      number_bottles -= 2;
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Not Enough Pts. ");
      lcd.setCursor(0, 1);
      lcd.print("Please Try Again");
      delay(1000);
    }
  }

  int capacitive = digitalRead(capacitive_pin);
  Serial.println(capacitive);
  if (capacitive == HIGH)
  {
    int inductive = digitalRead(inductive_pin);
    Serial.print("Inductive: \t");
    Serial.println(inductive);
    if (inductive == HIGH)
    {
      displayDetected();
      number_bottles++;
      openServo();
    }
    else
    {
      displayWarning();
    }
  }

  delay(1000);
  displayMain();
}

void displayWarning()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Trash Invalid!  ");
  lcd.setCursor(0, 1);
  lcd.print("Please Try Again");
  delay(1000);
}

void openServo()
{
  main_servo.write(0);
  delay(2000);
  main_servo.write(servo_default);
  delay(500);
}

void displayAward()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Congratulations!");
  lcd.setCursor(0, 1);
  lcd.print("Reward Redeemed!");
  delay(1000);
}

void displayDetected()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bottle Detected!");
  delay(1000);
}

void displayMain()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BOTE WIFI!      ");
  displayPoints();
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Please insert   ");
  lcd.setCursor(0, 1);
  lcd.print("a bottle!       ");
}

void displayPoints()
{
  lcd.setCursor(0, 1);
  lcd.print("Points: ");
  lcd.setCursor(7, 1);
  lcd.print(number_bottles);
}

void startRelay()
{
  bool start_sequence = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting Wi-Fi  ");
  lcd.setCursor(0, 1);
  digitalWrite(relay_pin, HIGH);
  delayStart = millis();
  while (start_sequence)
  {
    if (((millis() - delayStart) <= (wifi_start_timer * 1000)))
    {
      lcd.setCursor(0, 1);
      lcd.print("Please Wait...");
      delay(500);
    }
    else
    {
      start_sequence = false;
      lcd.setCursor(0, 0);
      lcd.print("Wi-Fi Started!  ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delayStart = millis();
      wifi_time_amount += wifi_add_amount;
      relay_enabled = true;
      delay(1000);
    }
  }
}