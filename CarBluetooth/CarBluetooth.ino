#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
//lcd vars
int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//Motor Driver vars
int in1 = A1, in2 = A2, in3 = A3, in4 = A4, enR = 3, enL = 11;
//Button Pin
int b = A0;
// Button Variables
int timeSpent;
//Speed Variables
int baseSpeed = 100;
int turnSpeed = 100;
int speedL, speedR;
// Bluetooh Variables
char flag = '0';
const byte rx = 2, tx = 12;

// Setting up objects
// lcd
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// bluetooth
SoftwareSerial MyBlue(rx, tx); //rx, tx

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

void setSpeedBluetooth(char flag){
  switch(flag){
      // Foward Flag
      case 'F':
        speedL = baseSpeed;
        speedR = baseSpeed;
        break;
      // Backward Flag
      case 'B':
        speedL = -baseSpeed;
        speedR = -baseSpeed;
        break;
      // Left Flag
      case 'L':
        speedL = -turnSpeed;
        speedR = turnSpeed;
        break;
      // Right Flag
      case 'R':
        speedL = turnSpeed;
        speedR = -turnSpeed;
        break;
      case '0':
        baseSpeed = 0;
        break;
      case '1':
        baseSpeed = 25;
        break;
      case '2':
        baseSpeed = 50;
        break;
      case '3':
        baseSpeed = 75;
        break;
      case '4':
        baseSpeed = 100;
        break;
      case '5':
        baseSpeed = 125;
        break;
      case '6':
        baseSpeed = 150;
        break;
      case '7':
        baseSpeed = 175;
        break;
      case '8':
        baseSpeed = 200;
        break;
      case '9':
        baseSpeed = 225;
        break;
      case 'q':
        baseSpeed = 250;
         break;
      // Anything Else
      default:
        speedL = 0;
        speedR = 0;
  }
}

void setup() {
  // put your setup code here, to run once:
  //Setting Up the lcd
  lcd.begin(16, 2);

  //Begin Serial
  Serial.begin(9600);
  //Begin MyBlue Serial Communication
  MyBlue.begin(9600);

  // Printing initial Screen
  lcd.setCursor(0, 0);
  lcd.print("Bluetooth Flag: ");

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
  // Bluetooth
  if(MyBlue.available() > 0){
    flag = MyBlue.read();
    Serial.println(flag);  
  }

  // Setting Speed of the wheels
  setSpeedBluetooth(flag);
  
  //Speed Tuning
  //speedR*=1.07;
  
  // Moving Motors
  moveCar(speedL, speedR);

  // Printing to lcd
  lcd.setCursor(0, 1);
  lcd.print(flag);

  // Printing to Serial
  Serial.println(flag);
}
