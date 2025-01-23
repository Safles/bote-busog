  #include <Servo.h>
  #include <Arduino.h>
  #include <LiquidCrystal_I2C.h>

  // Pins
  #define inductive_pin 8
  #define infrared_pin 7
  #define servo_pin 9
  #define bread_servo_pin 10
  #define tissue_servo_pin 11
  #define bread_pin 6
  #define tissue_pin 5

  // Flags and Variables
  const bool servo_enabled = true;
  const int servo_default = 90;
  const int servo_metal = 0;
  const int servo_plastic = 170;
  const int reward_servo_timer = 3;
  int metal_count = 0;
  int plastic_count = 0;

  // Declarations
  Servo main_servo;
  Servo bread_servo;
  Servo tissue_servo;
  LiquidCrystal_I2C lcd(0x27, 20,4);

  // Functions
  void activateServo(int position);
  void activateRewardServo(char mode);

  void writeLCD(char mode);

  void startFunction();
  void rewardSystem(char mode);

  void displayReward(char mode);
  void displayPoints();
  void displayMain();


  void setup() {
    lcd.init();
    lcd.backlight();
    Serial.begin(9600);
    digitalWrite(inductive_pin, INPUT);
    digitalWrite(infrared_pin, INPUT);
    Serial.println("Inductive and E18 Activated");
    delay(1000);
    digitalWrite(tissue_pin,INPUT);
    digitalWrite(bread_pin,INPUT);
    Serial.println("Buttons Activated");
    main_servo.attach(servo_pin);
    bread_servo.attach(bread_servo_pin);
    tissue_servo.attach(tissue_servo_pin);
    Serial.println("Servo Attached!");
    delay(1000);
    main_servo.write(servo_default);
    Serial.println("Servo set to Default");
    delay(2000);
    lcd.setCursor(0,0);
    lcd.print("LCD Initiated");
    delay(2000);
    lcd.clear();
    displayMain();
    displayPoints();
  }

  void loop() {
    int metal_btn = digitalRead(bread_pin);
    int plastic_btn = digitalRead(tissue_pin);
    Serial.print(plastic_count);
    Serial.print("\t");
    Serial.println(metal_count);
    if(metal_btn == 1){
      Serial.println("Metal Btn");
      rewardSystem('A');

    } else if (plastic_btn == 1){
      Serial.println("Plastic Btn");
      rewardSystem('B');
    }
    int infrared_value = digitalRead(infrared_pin);
    if (infrared_value == 0){
      startFunction();
    }
  }

  void startFunction(){
    lcd.setCursor(0,0);
    lcd.print("Trash Detected: ");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    delay(2000);
    int inductive_value = digitalRead(inductive_pin);
    if(inductive_value == 0){
      writeLCD('A');
    } else {
      writeLCD('B');
    }
    displayMain();
    delay(1000);
  }

  void activateServo(int position){
    if(servo_enabled){
      main_servo.write(position);
      delay(2000);
      main_servo.write(servo_default);
    } else {
      lcd.setCursor(8,1);
      lcd.print("No Servo");
      delay(2000);
    }
  }

  void writeLCD(char mode){
    switch(mode){
      case 'A':
        lcd.print("Metal               ");
        activateServo(servo_metal);
        metal_count++;
        break;
      case 'B':
        lcd.print("Plastic             ");
        activateServo(servo_plastic);
        plastic_count++;
        break;
    }
    displayPoints();
  }

  void displayPoints(){
    lcd.print("Points: ");
    lcd.setCursor(0,3);
    lcd.print("Metal: ");
    lcd.setCursor(6,3);
    lcd.print(metal_count);
    lcd.setCursor(9,3);
    lcd.print("Plastic: ");
    lcd.setCursor(17,3);
    lcd.print(plastic_count);
  }

  void displayMain(){
    lcd.setCursor(0,0);
    lcd.print("WASTE TO BUSOG!     ");
    lcd.setCursor(0,1);
    lcd.print("Please put trash");
    lcd.setCursor(0,2);
  }

  void rewardSystem(char mode){
    switch(mode){
      case 'A':
        if(plastic_count < 2){
          Serial.println(plastic_count);
          displayReward('B');
          break;
        } else {
          displayReward('A');
          plastic_count-=2;
          activateRewardServo('A');
          break;
        }
      case 'B':
        if(metal_count < 2){
          Serial.println(metal_count);
          displayReward('B');
          break;
        } else {
          displayReward('A');
          metal_count-=2;
          activateRewardServo('B');
          break;
        }
    }
  }

  void displayReward(char mode){
    switch(mode){
      case 'A':
        lcd.setCursor(0,0);
        lcd.print("Congratulations!    ");
        lcd.setCursor(0,1);
        lcd.print("Take your reward    ");
        break;
      case 'B':
        lcd.setCursor(0,0);
        lcd.print("Not enough points!  ");
        lcd.setCursor(0,1);
        lcd.print("                    ");
        break;
    }
    delay(1000);
    displayMain();
    displayPoints();
  }

  void activateRewardServo(char mode){
    switch(mode){
      case 'A':
        bread_servo.write(180);
        delay(reward_servo_timer*1000);
        bread_servo.write(90);
        break;
      case 'B':
        tissue_servo.write(180);
        delay(reward_servo_timer*1000);
        tissue_servo.write(90);
        break;
    }
  }