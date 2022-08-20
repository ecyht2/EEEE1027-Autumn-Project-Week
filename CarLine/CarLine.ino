#include <LiquidCrystal.h>
//lcd vars
int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//Motor Driver vars
int in1 = A1, in2 = A2, in3 = A3, in4 = A4, enR = 3, enL = 11;
//Button Pin
int b = A0;
// Button Variables
int timeSpent;
//Speed Variables
int baseSpeed = 80;
int turnSpeed = 83;
int speedL, speedR;
// Line Following Variables
bool Lstate, Rstate;
int irL = 2, irR = 12;

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

void setSpeedLine(int Lstate, int Rstate){
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

void setup() {
  // put your setup code here, to run once:
  //Setting Up the lcd
  lcd.begin(16, 2);
  // Asking baseSpeed
  baseSpeed = buttonRead(baseSpeed, "Setup baseSpeed: ");
  // Asking turnSpeed
  turnSpeed = buttonRead(turnSpeed, "Setup turnSpeed: ");

  // Printing initial time elasped
  lcd.setCursor(0, 0);
  lcd.print("Time Elapsed: ");

  //Begin Serial
  Serial.begin(9600);

  //Motor Pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //Button Pins
  pinMode(b, INPUT);
  //IR pins
  pinMode(irL, INPUT);
  pinMode(irR, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Setting up Variables
  Rstate = !digitalRead(irR);
  Lstate = digitalRead(irL);

  // Setting up timer
  int cTime = millis()/1000 - timeSpent;

  // Setting Speed of the wheels
  setSpeedLine(Lstate, Rstate);
  
  // Speed Tuning to make the car move straight
  speedR*=1.07;

  // Moving Motors
  moveCar(speedL, speedR);

  // Printing to lcd
  lcd.setCursor(0, 1);
  if(Rstate != 0 && Lstate != 0){
    lcd.print(cTime);
  }

  // Printing to Serial
  Serial.println("Left: " + String(Lstate));
  Serial.println("Right: " + String(Rstate));
}