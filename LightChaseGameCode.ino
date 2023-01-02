// Light chaser game by yui_maker
//  In this game Random LEDs will light up and you need to press the buttons associated with each LED.
//  the game ends if you take too long to press the button or you press a wrong button.
//  this code has a lot of Serial print commands for debugging. This way it can be played even without a display and just with serial monitor.
//  But if you have display you should remove the Serial print commands.

#include <Arduino.h> //this is only required on older Arduino IDEs or on platform IO.
#include <EEPROM.h>  //The highest score will be saved on EEPROM.

// defining LED pins. Change it according to the pins you have connected your LEDs to.
#define GRN_LED 5
#define YEL_LED 4
#define RED_LED 3
#define BLU_LED 2

// defining pushbutton pins. Change it according to the pins you have connected your push buttons to.
#define GRN_BTN 9
#define YEL_BTN 8
#define RED_BTN 7
#define BLU_BTN 6

// importing the libraries required for OLED display
// display related codes are copied from Adafruit's SSD1306 drivers example code.
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See data sheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// variables related with keeping score
byte randomButton; // this variable is used for storing the random number generated. As it will be always in between zero and four we are using byte.
int score = 0;
byte gamemode = 0;
int highest_score;
unsigned long current_time1;
const byte address = 0;                         // EEPROM address where you will be storing the highest score. As it will remain the same throughout the code we are using a constant byte
float gameDifficulty;


void setup()
{
  Serial.begin(9600);
  // setting LED pins as OUTPUT pins
  pinMode(GRN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YEL_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);

  // Setting Push button pins as INPUT pins with internal pullup resistors
  pinMode(GRN_BTN, INPUT_PULLUP);
  pinMode(RED_BTN, INPUT_PULLUP);
  pinMode(YEL_BTN, INPUT_PULLUP);
  pinMode(BLU_BTN, INPUT_PULLUP);

  // initializing the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setTextColor(SSD1306_WHITE);

  // initializing the pseudo-random number generator
  randomSeed(analogRead(millis()));
  randomButton = random(0, 4); // creating a random number between zero and four. As we only have four buttons.
  current_time1 = millis();    // putting a time stamp

//  EEPROM.write(address, 0);             // Uncomment this line only if you ever want to reset the heighest score saved in EEPROM to zero,
  highest_score = EEPROM.read(address); // Reading the heighest score from EEPROM

}   //closing the set up function


// this function will be running when we are waiting for a player to start the game by pressing any button.
void startMode(void)
{
  bool light_on = true;
  while (
    digitalRead(GRN_BTN) == HIGH && digitalRead(RED_BTN) == HIGH && digitalRead(YEL_BTN) == HIGH && digitalRead(BLU_BTN) == HIGH)
  {
    // looping between lighting two LED pairs at a time with different text on display
    if (millis() - current_time1 >= 1000)
    {
      if (light_on)
      {
        digitalWrite(GRN_LED, HIGH);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(YEL_LED, LOW);
        digitalWrite(BLU_LED, LOW);

        // showing instructions on display
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(10, 5);
        display.println(F("Press any button "));
        display.setCursor(10, 20);
        display.println(F("to start playing."));
        display.display();
        light_on = false;
      }
      else
      {
        digitalWrite(GRN_LED, LOW);
        digitalWrite(RED_LED, LOW);
        digitalWrite(YEL_LED, HIGH);
        digitalWrite(BLU_LED, HIGH);

        // asking to start paying
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0, 8);
        display.println(F("Let's play"));
        display.display();
        light_on = true;
      }
      current_time1 = millis();
    }
  }
  // if the player presses any button we turn off all the LEDs and start a count down.
  digitalWrite(GRN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YEL_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  delay(250);
  for (int i = 3; i >= 1; i--)
  {
    // showing the start count down
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(25, 4);
    display.println(F("Let's start!"));
    display.setTextSize(2);
    display.setCursor(55, 17);
    display.println(i);
    display.display();
    delay(500);
  }
  // After the count down we clear the display, change the gamemode flag, set the score to zero and put a time stamp on current_time1
  display.clearDisplay();
  gamemode = 1;
  score = 0;
  current_time1 = millis();
} // closing the startMode function


// this function continuously updates the score while user is playing the game
void updateScore(void)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(25, 4);
  display.println(F("Your score: "));
  display.setTextSize(2);
  display.setCursor(55, 17);
  display.println(score);
  display.display();
} // closing the updateScore function


// this function runs if you press the wrong button or don't press the button in time
void gameOver(void)
{
  Serial.println("you were over time limit or pressed the wrong button.");
  if ((score >= highest_score) && (score != 0))
  {
    highest_score = score;
    EEPROM.update(address, highest_score); // updating the highest score in EEPROM memory address.
    Serial.println("Congratulations, you made the highest score of " + String(score));
    // showing the highest score on display
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(5, 5);
    display.println(F("You made the"));
    display.setCursor(5, 18);
    display.println("higest score: " + String(highest_score) + "!");
    display.display();
    delay(3 * 1000); // the highest score will be shown for 3 seconds.
  }

  else if (score < highest_score)
  {
    Serial.println("Your final score: " + String());
    Serial.println("Highest score: " + String(highest_score));

    // showing the player's score on display with the highest score.
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(5, 5);
    display.println(F("Your score: "));
    display.setCursor(100, 5);
    display.println(score);
    display.setCursor(5, 20);
    display.println(F("highest score: "));
    display.setCursor(100, 20);
    display.println(highest_score);
    display.display();
    delay(3 * 1000); // the player's score will be shown for 3 seconds.
  }
  gamemode = 0; // this flag will take us back to the start mode
} // closing the gameOver function


// Based on the random number generated we give the buttonChase function a LED and Button combination.
// this function runs the game for each combination.
void buttonChase(int led, int but)
{
  digitalWrite(led, HIGH);
  Serial.println("Press " + String(but));
  updateScore();
  gameDifficulty = 1500 / (score / 10 + 1); // As the score increase the time allowed to press the button will go down.
  if (millis() - current_time1 <= gameDifficulty)
  {
    if ((digitalRead(but) == LOW))
    {
      digitalWrite(led, LOW);
      Serial.println("You pressed" + String(but));
      byte oldnum = randomButton;    // as the variable oldnum won't be used in any other function we are creating it only here
      while (randomButton == oldnum) // making sure we do not light up the same LED as before
      {
        randomSeed(analogRead(millis()));
        randomButton = random(0, 4);
      }
      score = score + 1;
      updateScore();
      current_time1 = millis();         // updating the time stamp
      while ((digitalRead(but) == LOW)) // you need to let go of the button for the game to proceed
      {
      }
    }
    // following lines of code checks if user pressed the right button or not.
    if ((led == GRN_LED) && ((digitalRead(RED_BTN) == LOW) || (digitalRead(YEL_BTN) == LOW) || (digitalRead(BLU_BTN) == LOW)))
    {
      gameOver();
    }
    else if ((led == RED_LED) && ((digitalRead(GRN_BTN) == LOW) || (digitalRead(YEL_BTN) == LOW) || (digitalRead(BLU_BTN) == LOW)))
    {
      gameOver();
    }
    else if ((led == YEL_LED) && ((digitalRead(RED_BTN) == LOW) || (digitalRead(GRN_BTN) == LOW) || (digitalRead(BLU_BTN) == LOW)))
    {
      gameOver();
    }
    else if ((led == BLU_LED) && ((digitalRead(RED_BTN) == LOW) || (digitalRead(YEL_BTN) == LOW) || (digitalRead(GRN_BTN) == LOW)))
    {
      gameOver();
    }
  }
  // if you didn't press the button in time the game will end
  else
  {
    gameOver();
  }
} // closing buttonChase function


void loop()
{
    // based on the random number generated we run the game function buttonchase for different LED and button combinations

  if (gamemode == 0)
  {
    startMode();
  }

  else if (randomButton == 0)
  {
    buttonChase(GRN_LED, GRN_BTN);
  }

  else if (randomButton == 1)
  {
    buttonChase(RED_LED, RED_BTN);
  }

  else if (randomButton == 2)
  {
    buttonChase(YEL_LED, YEL_BTN);
  }

  else if (randomButton == 3)
  {
    buttonChase(BLU_LED, BLU_BTN);
  }
} // closing the void loop function
