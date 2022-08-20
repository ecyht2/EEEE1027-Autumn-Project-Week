#include <LiquidCrystal.h>
//lcd vars
int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//Motor Driver vars
int in1 = A1, in2 = A2, in3 = A3, in4 = A4, enR = 3, enL = 11;
//Button Pin
int b = A0;
// Button Variables
int timeSpent;
int cTime;
//Speed Variables
int baseSpeed = 80;
int turnSpeed = 83;
int speedL, speedR;
// Encoder Variables
const byte encoderRight = 12;
const byte encoderLeft = 2;
volatile long counterLeft = 0;
volatile long counterRight = 0;
int journeyDistance = 19;
int journeyTime = 5;
int nCount = 0;
double distanceTraveled;
bool encoderMode;
bool tempL, tempR;

// Setting up objects
// lcd
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void moveCar(int speed2, int speed1){
  //Setting Speed of the wheels
  analogWrite(enR, abs(speed1));
  analogWrite(enL, abs(speed2));

  //Move Right Wheel Forward if speed > 0 else move backwards
  if(speed1 > 0){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if(speed1 < 0){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);    
  } else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);  
  }

  //Move Left Wheel Forward if speed > 0 else move backwards
  if(speed2 > 0){
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else if(speed2 < 0){
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);    
  } else{
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);  
  }
}

int buttonRead(int initial, String question){
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

void setSpeedEncoder(int counterLeft, int counterRight){
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

int distanceToCounter(int journey, byte type){
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

double counterToDistance(int counter, byte type){
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

void blinkLeft() {
  counterLeft++;
}

void blinkRight() {
  counterRight++;
}

void setup() {
  // put your setup code here, to run once:
  //Setting Up the lcd
  lcd.begin(16, 2);
  
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
  
  //Begin Serial
  Serial.begin(9600);

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

  //Motor Pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //Button Pins
  pinMode(b, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Setting up timer
  cTime = millis()/1000 - timeSpent;

  // Encoder
  if(digitalRead(encoderRight) != tempR){
    blinkRight();  
    tempR = digitalRead(encoderRight);
  }
  if(digitalRead(encoderLeft) != tempL){
    blinkLeft();  
    tempL = digitalRead(encoderLeft);
  }

  // Setting speed of the wheels
  setSpeedEncoder(counterLeft, counterRight);
  
  // Moving Motors
  moveCar(speedL, speedR);

  // Printing to lcd
  distanceTraveled = counterToDistance(counterLeft, 0); 
  if(encoderMode == 1){
    // Time
    if(speedR != 0 && speedL != 0){
      lcd.setCursor(13, 0);
      lcd.print(cTime);
    }
  } else {
    // Distance Traveled
    lcd.setCursor(9, 0);
    lcd.print(distanceTraveled);
  }
  // Speed
  if(speedR != 0 && speedL != 0){
    lcd.setCursor(6, 1);
    lcd.print(distanceTraveled/cTime);
  }
  
  // Printing to Serial
  lcd.setCursor(0, 1);
  if(encoderMode == 1){
    if(speedR != 0 && speedL != 0){
      Serial.println(cTime);
    }
  } else {
    Serial.println(distanceTraveled);
  }
}
