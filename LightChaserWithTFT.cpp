#include <Arduino.h>
#include <EEPROM.h>

// LED Definitions
#define GRN_LED 2
#define RED_LED 3
#define YEL_LED 4
#define BLU_LED 5

// Pushbutton Definitions
#define GRN_BTN 6
#define RED_BTN 7
#define YEL_BTN 8
#define BLU_BTN 9

// Display libraries
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"


// Display connections
#define TFT_CS 10
#define TFT_DC 1
#define TFT_RST -1 // RST can be set to -1 if you tie it to Arduino's reset

// Using SPI hardware (on Uno/nano, #13, #12, #11) 
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);


// the variabes used in the code
unsigned int randbut;  //integer for storing random numbers.
unsigned int score = 0; //integer for storing score.
unsigned int gamemode = 0; //integer for going from waiting for players to game mode.
unsigned int highest_score;  //integer for storing highest score. This one gets stored in EEPROM
unsigned long current_time1; //integer for using millis as delay() should not be used. 
unsigned int address = 0; //integer for pointing to the memory location where we want to store the highest score. 
unsigned int difficulty = 1500/(score/10 +1); //the user will have 1.5 seconds for the first button and then as the score goes up time to press each button will go down respectively. Change 10 to a bigger number if you want to make it easier and vice versa.  


void setup()
{

  // setting LEDs as outputs
  pinMode(GRN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YEL_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);

  // setting Pushbuttons as Inputs with internal pullup resistors
  pinMode(GRN_BTN, INPUT_PULLUP);
  pinMode(RED_BTN, INPUT_PULLUP);
  pinMode(YEL_BTN, INPUT_PULLUP);
  pinMode(BLU_BTN, INPUT_PULLUP);

  //Initializing the display
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);
  tft.setTextSize(4);
  
   //seeding the random numbers for the game. This one only seeds the very first time when nano is powered. 
  randomSeed(analogRead(millis()));
  randbut = random(0, 4); //picking up the very first button to light up. 
  current_time1 = millis();  //starting the timer

   //Reading the highest score from the EEPROM
  highest_score = EEPROM.read(address);
//  EEPROM.write(address, 0);     //This is here just in case someone neads to remove the highest score and put it back to zero. 
//  Serial.begin(9600);           //Serial moniter can't be used as the pin number 1 is being used for the display. Disconnect Tx pin from display if Serial moniter needs to be used for debuging. 
  
}

//The wait mode function. This function lights up two LEDs at a time. 
void startmode(void)
{
  bool light_on = true;
  while ( 
      digitalRead(GRN_BTN) == HIGH && digitalRead(RED_BTN) == HIGH && digitalRead(YEL_BTN) == HIGH && digitalRead(BLU_BTN) == HIGH)
  {
    if (millis() - current_time1 >= 1000)
    {
      if ( light_on ) {
        digitalWrite(GRN_LED, HIGH);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(YEL_LED, LOW);
        digitalWrite(BLU_LED, LOW);
        tft.setTextSize(3);
        tft.setTextColor(HX8357_CYAN); 
        tft.setCursor(50, 40);  
        tft.println("Hi there!");
        tft.setCursor(50, 80);  
        tft.println("Welcome to ");
        tft.setCursor(50, 120);  
        tft.println("Aalto Design Factory. ");
        tft.setCursor(50, 160);  
        tft.println("Press any button ");
        tft.setCursor(50, 200);  
        tft.println("to start playing.");
        tft.fillRect(90,240,320,260,HX8357_BLACK);
        light_on = false;
      } else {
        digitalWrite(GRN_LED, LOW);
        digitalWrite(RED_LED, LOW);
        digitalWrite(YEL_LED, HIGH);
        digitalWrite(BLU_LED, HIGH);
        tft.setTextSize(5);
        tft.setCursor(90, 240);  
        tft.setTextColor(HX8357_YELLOW);
        tft.println("Let's play!");
        light_on = true;
      }
      current_time1 = millis();
    }    
  }
  digitalWrite(GRN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YEL_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  tft.fillScreen(HX8357_BLACK);
  tft.setCursor(0, 130);  
  tft.setTextColor(HX8357_GREEN);  
  tft.setTextSize(3);
  tft.println("Push the lit-up button as fast as you can.");
  delay(1500);
  tft.fillScreen(HX8357_BLACK);

  //to show the 3, 2, 1 on screen. 
  for(int i=3; i>=1; i--) {
    tft.setCursor(200, 100);  
    tft.setTextSize(18);
    tft.setTextColor(HX8357_RED); 
    tft.println(i);
    delay(1000);
    tft.setCursor(200, 100);  
    tft.setTextSize(18);
    tft.setTextColor(HX8357_BLACK);
    tft.println(i);  
    delay(1);
  }
  tft.fillScreen(HX8357_BLACK);
  gamemode = 1;
  score = 0;
  current_time1 = millis();     
}


//this function executes if user pressed the wrong button or went over the time limit. 
void gameover(void)
{
//  Serial.println("you were over time limit or pressed the wrong button.");
    if (score >= highest_score){
      highest_score = score; 
      EEPROM.update(address, highest_score);
//      Serial.print("Congratulations, you made the highest score of ");
//      Serial.println(score); 
      tft.setTextSize(4); 
      tft.fillScreen(HX8357_BLACK);
      tft.setCursor(0, 120);  
      tft.setTextColor(HX8357_RED);  
      tft.println("Congratulations,");
      tft.setCursor(0, 160); 
      tft.println("you just made the ");
      tft.setCursor(0, 200); 
      tft.print("highest score of " + String(highest_score) + "!");     
      delay(5000);
      tft.fillScreen(HX8357_BLACK);
    }
    if (score < highest_score){
//      Serial.print("Youe final score: ");
//      Serial.println(score);      
//      Serial.print("Highest score: ");
//      Serial.println(highest_score);
      tft.setTextSize(4);
      tft.fillScreen(HX8357_BLACK);
      tft.setCursor(10, 120);  
      tft.setTextColor(HX8357_YELLOW);  
      tft.print("Your score: ");    
      tft.println(score);
      tft.setCursor(10, 180);  
      tft.setTextColor(HX8357_RED);  
      tft.print("Highest score: " + String(highest_score));    
      delay(5000);
      tft.fillScreen(HX8357_BLACK);

    }
        gamemode = 0;
}


   //When the game is being played this function continuously updates the score on display 
void updateScore()
{
      tft.setTextSize(4);
      tft.setCursor(40, 140); 
      tft.setTextSize(5);  
      tft.setTextColor(HX8357_YELLOW);
      tft.setTextColor(HX8357_YELLOW,HX8357_BLACK);
      tft.println("Your score: " + String(score));
}

   //The game function
void buttchase(int led, int but)
{
  digitalWrite(led, HIGH);
  //    Serial.print("Press ");
  //    Serial.println(but);
  updateScore();
  if (millis() - current_time1 <= difficulty)
  {
    if ((digitalRead(but) == LOW))
    {
      digitalWrite(led, LOW);
      //      Serial.print("You pressed");
      //      Serial.println(but);
      unsigned int oldnum = randbut;    //this is so that two consecutive buttons are not same color. 
      while (randbut == oldnum){
        randomSeed(analogRead(millis()));
        randbut = random(0, 4);
      }
      score = score + 1;
      updateScore();
      current_time1 = millis();
      while((digitalRead(but) == LOW)){  //if the user keeps any button pressed then the game won't move forward
        
      }
    }
    //the following commands check if the wrong button was pressed.
    if ((led == GRN_LED) && ((digitalRead(RED_BTN) == LOW) || (digitalRead(YEL_BTN) == LOW) || (digitalRead(BLU_BTN) == LOW))){
      gameover();
    }
    if ((led == RED_LED) && ((digitalRead(GRN_BTN) == LOW) || (digitalRead(YEL_BTN) == LOW) || (digitalRead(BLU_BTN) == LOW))){
      gameover();
    }
    if ((led == YEL_LED) && ((digitalRead(RED_BTN) == LOW) || (digitalRead(GRN_BTN) == LOW) || (digitalRead(BLU_BTN) == LOW))){
      gameover();
    }
    if ((led == BLU_LED) && ((digitalRead(RED_BTN) == LOW) || (digitalRead(YEL_BTN) == LOW) || (digitalRead(GRN_BTN) == LOW))){
      gameover();
    }

  }

  else{
    gameover();
  }
}

void loop()
{
  //If the game mode is zero the game is in waiting mode. If you press any button it will go to player mode.
  if (gamemode == 0){
    startmode();
  }  
  
    //Associating each random number with a different button. 
  if (randbut == 0)
  {
    buttchase(GRN_LED, GRN_BTN);
  }

  if (randbut == 1)
  {
    buttchase(RED_LED, RED_BTN);
  }

  if (randbut == 2)
  {
    buttchase(YEL_LED, YEL_BTN);
  }

  if (randbut == 3)
  {
    buttchase(BLU_LED, BLU_BTN);
  }
}