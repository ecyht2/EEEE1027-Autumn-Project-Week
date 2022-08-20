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
    lcd.setCursor(0,1);
    lcd.print("                 ");
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

// Game
int difficulty = 1;

class ScrollingGame{
  public:
    void start();
    void printLine(byte line, unsigned short row);
    void randomSpawn();
    void changeLane();
    void check_lose();
    void setTimes();
    void move();
    void reset();
    void setProb(int);
    void printInitial();
    ScrollingGame(int);
      
  private:
    unsigned short rows[2];
    //Speed Adjusted due to simulation speeds default 1000
    int speed = 1000, prob = 5, score = -13;
    int ploc = 0;
    bool lost = 0;
    unsigned long t_now, t_move;
    int b;
    int counter = 0;
    int b_state;
    // Custom Char
    byte wallBoth[8] = {
      B11111,
      B10001,
      B10001,
      B10001,
      B10001,
      B10001,
      B10001,
      B11111
    };
    byte wallLeft[8] = {
      B11111,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B11111
    };
    byte wallRight[8] = {
      B11111,
      B00001,
      B00001,
      B00001,
      B00001,
      B00001,
      B00001,
      B11111
    }; 
    byte wallNone[8] = {
      B11111,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000,
      B11111
    };
};

void ScrollingGame::start(){
  setTimes();
  while(lost == 0){
      b_state = analogRead(b);
      //Switch Lane if button is pressed
      changeLane();
      //Prints First Row
      printLine(0, rows[0]);
      //Prints Second Row
      printLine(1, rows[1]);
    
      //Checking Game Status
      check_lose();
   
      //Updating t_now
      t_now = millis();
      //Moving The Obsticles
      move();
    
      //Speeds up the game
      if(score >= 0 && speed > 100){
        //Speed adjusted due to simulation speeds, default 100
        speed = 1000 - score*10;
        Serial.println(speed);
      }
    }
}

ScrollingGame::ScrollingGame(int button){
    //Setting Up Variables
    //Intitial Rows
    rows[1] = 1;
    rows[0] = 4;
    //Setting Up Button pins
    b = button;
    // Custom Chars
    lcd.createChar(0, wallNone);
    lcd.createChar(1, wallRight);
    lcd.createChar(2, wallLeft);
    lcd.createChar(3, wallBoth);
}

void ScrollingGame::printLine(byte line, unsigned short row){
  for(int i = 15; i >= 0 ; i--){
    lcd.setCursor(15 - i, line);
    
    //Prints Obsticles
    if(bitRead(row, i)){
      // Printing different walls
      // Both Sides has walls
      if(bitRead(row, i - 1) && bitRead(row, i + 1)){
        lcd.write((byte)0);  
      } else if(bitRead(row, i - 1)){ // Only Left has walls
        lcd.write((byte)2);  
      } else if(bitRead(row, i + 1)){ // Only Right has walls
        lcd.write((byte)1);  
      } else{ // No sides has walls
        lcd.write((byte)3);  
      }
    } else{
      lcd.print(" ");
    }
      //Prints Player
      lcd.setCursor(0, ploc);
      lcd.print("X");
  }
    
}
void ScrollingGame::randomSpawn(){
  //Random Spawning Row 0 /* Making Sure there is no impossible levels*/
  if(random(prob) == 0 && !bitRead(rows[1], 1)){
      rows[0]+=1;
  }
  //Random Spawning Row 1  /* Making Sure there is no impossible levels*/
  if(random(prob) == 0  && !(bitRead(rows[0], 0) || bitRead(rows[0], 1))){
    rows[1]+=1;
  }
}

void ScrollingGame::changeLane(){
  if(b_state >= 400 && b_state < 600){
    counter++;  
  } else{
    counter = 0;
  }
  
  if(counter == 1){
    ploc = (ploc + 1) % 2;
  }
}

void ScrollingGame::check_lose(){
    //Checking if player hits a wall
  if(bitRead(rows[ploc], 15)){
      //Set the game into lost state
      lost = 1;
      //Printing Game Over Screen
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(score));
      lcd.setCursor(0, 0);
      lcd.print("Game Over");
    }
}

void ScrollingGame::setTimes(){
  //Updating new move time
    t_now = millis();
    t_move = t_now + speed;
}

void ScrollingGame::move(){
  if(t_now >= t_move){
      //Shifts Everything to the left
      rows[0] = rows[0] << 1;
      rows[1] = rows[1] << 1;
      //Adding delay
      score++;
      //Adding Random Spawns
      randomSpawn();
      //Setting Times
      setTimes();
    }
}

void ScrollingGame::reset(){
  //Resets Game
  b_state = analogRead(b);
  if(b_state >= 0 && b_state < 60){
    //Restting Lost
    lost = 0;
    //Resetting Rows
    rows[1] = 1;
    rows[0] = 4;
    //Resting Speed
    speed = 1000;
    // Resting Score
    score = -13;
    lcd.clear();
    //Making sure Button only activates once
    while(b_state >= 0 && b_state < 60){
      b_state = analogRead(b);
      delay(100);
    }
    //Restarting the Game
    start();
  }
}

void ScrollingGame::setProb(int difficulty){
  //Dfficulty/prob
  prob = 5 - difficulty + 1;  
}

void ScrollingGame::printInitial(){
  lcd.setCursor(0, 0);
  lcd.print("Right to Play");  
  lcd.setCursor(0, 1);
  lcd.print("Left to Move");  
}

ScrollingGame game(b);

void setup() {
  // put your setup code here, to run once:
  //Setting Up the lcd
  lcd.begin(16, 2);

  //Begin Serial
  Serial.begin(9600);

  // Ask Difficulty
  difficulty = buttonRead(difficulty, "Set Difficulty: ");
  game.setProb(difficulty);

  // Printing Instructions
  game.printInitial();

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
  game.reset();
}
