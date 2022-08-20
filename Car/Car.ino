#include <Ultrasonic.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
//lcd vars
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//Motor Driver vars
const int in1 = A1, in2 = A2, in3 = A3, in4 = A4, enR = 3, enL = 11;
//Button Pin
const int b = A0;
// Line Following Variables
const byte irL = 2, irR = 12;
// Bluetooh Variables
const byte rx = 2, tx = 12;
// Ultrasonic Variables
const byte trig = 2, echo = 12;
// Encoder Variables
const byte encoderRight = 12;
const byte encoderLeft = 2;

// Setting up Objects
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial MyBlue(rx, tx); //rx, tx
Ultrasonic ultrasonic(trig, echo);

class Car{
  public:
    void setupDriver(int, int, int, int, int, int);
    void setupButton(int);
    void setupIR(byte, byte);
    void setupBluetooth(byte, byte);
    void setupUltrasonic(byte, byte);
    void setupEncoder(byte, byte);
    void setupGame(byte);
    void init();
    void setType();
    void moveCar();
    int buttonRead(int, String);
    void getValues();
    void setSpeedIR();
    void setSpeedEncoder(int, int);
    void setSpeedUltrasonic(unsigned int);
    void setSpeedBluetooth(char);
    void blinkLeft();
    void blinkRight();
    int distanceToCounter(int, byte);
    double counterToDistance(int, byte);
    void setSpeed();
    void lcdPrint();
    void serialPrint();
    
  private:
    // Button Variables
    int timeSpent;
    int cTime;
    //Speed Variables
    int baseSpeed = 80;
    int turnSpeed = 83;
    int speedL, speedR;
    // Line Following Variables
    bool Lstate, Rstate;
    // Bluetooh Variables
    char flag = '0';
    // Ultrasonic Variables
    int stopDistance = 20;
    int distance;
    // Encoder Variables
    volatile long counterLeft = 0;
    volatile long counterRight = 0;
    int journeyDistance = 200;
    int journeyTime = 5;
    int nCount = 0;
    long distanceTraveled;
    bool encoderMode;
    bool tempL, tempR;
    // Motor Driver Pins
    int in1, in2, in3, in4, enR, enL;
    // Button Pins
    byte b;
    // Type Variable
    byte type;
};

void Car::init(){
  /* type 0 = IR, type 1 = Encoder, type 2 = Ultrasonic, type 3 = Bluetooth, type 4 = Game */
  // Asking baseSpeed
  baseSpeed = buttonRead(baseSpeed, "Setup baseSpeed: ");
  // Asking turnSpeed
  turnSpeed = buttonRead(turnSpeed, "Setup turnSpeed: ");

  //Motor Pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //Button Pins
  pinMode(b, INPUT);

  //Begin Serial
  Serial.begin(9600);
  
  switch(type){
    case 0:
      // Printing initial time elasped
      lcd.setCursor(0, 0);
      lcd.print("Time Elapsed: ");

      //IR pins
      pinMode(irL, INPUT);
      pinMode(irR, INPUT);
      break;

    case 1:
      // Asking Encoder Mode
      encoderMode = buttonRead(encoderMode, "Setup Mode: ");
      if(encoderMode == 1){
        // Asking journey, distance that you want to travel
        journeyDistance = buttonRead(journeyDistance, "How far to go: ");
        // Converting journey in cm/revolutions to number of cunter
        nCount = distanceToCounter(journeyDistance, 0);
      } else{
        // Asking journey, distance that you want to travel
        journeyTime = buttonRead(journeyTime, "How long to go: ");
      }

      // Setting up initial temp
      tempR = digitalRead(encoderRight);
      tempL = digitalRead(encoderLeft);

      // Printing to lcd
      lcd.setCursor(0, 0);
      if(encoderMode == 1){
        // Printing initial time elasped
        lcd.print("Time Elapsed: ");
      } else{
        lcd.print("Distance: ");  
      }
      lcd.setCursor(0, 1);
      lcd.print("Speed: ");

      // Encoder pin Interupts
      pinMode(encoderRight, INPUT_PULLUP);
      // attachInterrupt(digitalPinToInterrupt(encoderRight), blinkRight, CHANGE);
      pinMode(encoderLeft, INPUT_PULLUP);
      // attachInterrupt(digitalPinToInterrupt(encoderLeft), blinkLeft, CHANGE);
      break;
      
    case 3:
      break;
  }
}

void Car::setType(){
  type = buttonRead(type, "Setup Mode: ");  
}

void Car::setupDriver(int in_1, int in_2, int in_3, int in_4, int en_R, int en_L){
  in1 = in_1;
  in2 = in_2;
  in3 = in_3;
  in4 = in_4;
  enR = en_R;
  enL = en_L;
}

void Car::setupButton(int button){
  b = button;  
}

void Car::getValues(){
  cTime = millis()/1000 - timeSpent;
  
  switch(type){
    case 0:
      Rstate = !digitalRead(irR);
      Lstate = digitalRead(irL);
      break;
    case 1:
      // Encoder
      if(digitalRead(encoderRight) != tempR){
        blinkRight();  
        tempR = digitalRead(encoderRight);
      }
      if(digitalRead(encoderLeft) != tempL){
        blinkLeft();  
        tempL = digitalRead(encoderLeft);
      }
  }  
}
// ultrasonic
// Ultrasonic ultrasonic(trig, echo);

void Car::moveCar(){
  //Setting Speed of the wheels
  analogWrite(enR, abs(speedR));
  analogWrite(enL, abs(speedL));

  //Move Right Wheel Forward if speed > 0 else move backwards
  if(speedR > 0){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if(speedR < 0){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);    
  } else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);  
  }

  //Move Left Wheel Forward if speed > 0 else move backwards
  if(speedL > 0){
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else if(speedL < 0){
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);    
  } else{
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);  
  }
}


int Car::buttonRead(int initial, String question){
  lcd.setCursor(0, 0);
  lcd.print(question);
  int start = 0;
  int bState = analogRead(b);

  // Button Reading
  while(start == 0){
    bState = analogRead(b);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(initial);
    Serial.println(bState);
    
    if(bState < 60){
      initial++;
      
      while(bState < 60){
        bState = analogRead(b);
      }
    } else if(bState < 200){
      initial+=10;
      
      while(bState < 200){
        bState = analogRead(b);
      }
    } else if(bState < 400){
      initial-=10;
 
      while(bState < 400){
        bState = analogRead(b);
      }
    } else if(bState < 600){
      initial--;
      
      while(bState < 600){
        bState = analogRead(b);
      }
    } else if(bState >= 600 && bState <= 800){
      timeSpent = millis()/1000;
      lcd.setCursor(0,1);
      lcd.print("                 ");
      
      while(bState < 800){
        bState = analogRead(b);       
      }
      start = 1;
    }

    delay(100);
  }

  lcd.clear();
  return initial;
}

void Car::setSpeedIR(){
  // Motor Controller
  // Going Straight
  if(Rstate == 1 && Lstate == 1){
    speedR = baseSpeed;
    speedL = baseSpeed;
  } else if(Rstate == 1 && Lstate == 0){ // Turn Right
    speedR = turnSpeed;
    speedL = -turnSpeed;
  } else if(Rstate == 0 && Lstate == 1){ // Turn Left
    speedL = turnSpeed;
    speedR = -turnSpeed;
  } else{ // Stop
    speedL = 0;
    speedR = 0;
  }
}

void Car::setSpeedEncoder(int counterLeft, int counterRight){
   // Travel a certain distance
  if(encoderMode == 1){
    // Left encoder
    if(counterLeft < nCount){
      speedL = baseSpeed;
    } else{
      speedL = 0;
    }
    // Right encoder
    if(counterRight < nCount){
      speedR = baseSpeed;  
    } else{
      speedR = 0;
    }
  } else { // Travel for a certain amount of time
    if(cTime >= journeyTime){
      speedR = 0;
      speedL = 0;  
    } else{
      speedR = baseSpeed;
      speedL = baseSpeed;
    }
  }

  // Speed Tuning
  if(counterRight > counterLeft){
    speedL *= 1.5;  
  } else if(counterLeft > counterRight){
    speedR *= 1.5;  
  }
}

void Car::setSpeedUltrasonic(unsigned int distance){
  if(distance < stopDistance){
    speedL = 0;
    speedR = 0;
  } else {
    speedL = baseSpeed;
    speedR = baseSpeed;
  }
}

void Car::setSpeedBluetooth(char flag){
  switch(flag){
      case 'F':
        speedL = baseSpeed;
        speedR = baseSpeed;
        break;
      case 'B':
        speedL = -baseSpeed;
        speedR = -baseSpeed;
        break;
      case 'L':
        speedL = -turnSpeed;
        speedR = turnSpeed;
        break;
      case 'R':
        speedL = turnSpeed;
        speedR = -turnSpeed;
        break;
      default:
        speedL = 0;
        speedR = 0;
  }
}

void Car::blinkLeft() {
  counterLeft++;
}

void Car::blinkRight() {
  counterRight++;
}

int Car::distanceToCounter(int journey, byte type){
  // counter = 80, 1 rotation, d = 6.5
  double ratio = 40/(PI*6.2);
  int counter = 0;
  
  switch(type){
    // journey in cm
    case 0:  
      counter = journey * ratio;
      break;
      
    // journey in amount of wheel revolutions
    case 1:
      counter = journey * 40;
      break;

    default: 
      counter = 0;
      break;
  }  

  return counter;
}

double Car::counterToDistance(int counter, byte type){
  // counter = 80, 1 rotation, d = 6.5
  double ratio = (PI*6.2)/40;
  double journey = 0;
  
  switch(type){
    // journey in cm
    case 0:  
      journey = counter * ratio;
      break;
      
    // journey in amount of wheel revolutions
    case 1:
      journey = counter / 40;
      break;

    default: 
      counter = 0;
      break;
  }  

  return journey;
}


void Car::setSpeed(){
  switch(type){
    // Case 0 for line following speeds
    case 0:
      setSpeedIR();
      break;
    case 1:
      setSpeedEncoder(counterLeft, counterRight);
      break;
    case 2:
      setSpeedUltrasonic(distance);
      break;
    case 3:
      setSpeedBluetooth(flag);
      break;
    default:
      return;
  }  

  // Speed Tuning
  speedR*=1.07;
}

void Car::lcdPrint(){
    switch(type){
    // Case 0 for line following speeds
    case 0:
      // Printing to lcd
      lcd.setCursor(0, 1);
      if(Rstate != 0 && Lstate != 0){
        lcd.print(cTime);
      }
      break;
      
    case 1:
      setSpeedEncoder(counterLeft, counterRight);
      break;
    case 2:
      setSpeedUltrasonic(distance);
    default:
      return;
  }  
}

void Car::serialPrint(){
  switch(type){
    // Case 0 for line following speeds
    case 0:
      // Printing to Serial
      Serial.println("Left: " + String(Lstate));
      Serial.println("Right: " + String(speedR));
      break;
      
    case 1:
      setSpeedEncoder(counterLeft, counterRight);
      break;
    case 2:
      setSpeedUltrasonic(distance);
    default:
      return;
  }  
}

Car car;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  car.setType();
  car.init();
  car.setupDriver(in1, in2, in3, in4, enR, enL);
  
  Serial.println("hi");
}

void loop() {
  // Setting up Variables
  car.getValues();

  car.setSpeed();

  car.moveCar();

  car.lcdPrint();

  car.serialPrint();
}
