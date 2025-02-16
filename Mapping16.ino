#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>

/*
Scaffolding: Mapping - Group 16

Program by: Luke Whipple and Fabiana Rodriguez Velez

GOAL:
Bop-It like game where the player must flick the Circuit Playground in different directions,
cover the device with their hand, or shout into it depending on different Neopixel patterns.
Upon taking too long or making a mistake, the game will end and your score will be displayed.

TO PLAY:
Hold the device with one hand parralel to the ground and press the left button to start the game.

2-3 Neopixels will light in the direction you need to move the sensor,
so hold it firm when you move your arm in that direction.
If all the neopixels start flashing, you need to shout into the device.
If the neopixels start to alternate a dim glow, you need to cover the device with your other hand.

If you succeeded, a short animation will play along with a possitive beep.
If you did the wrong action or took too long, 
the neopixels will flash red with a long buzzer and the game ends. Your score is then displayed.

Your score is based on the number of LEDs that light up, ranging as so:
  0-5: RED
  6-10: ORANGE
  11-15: YELLOW
  16-20: GREEN
  21-25: BLUE
  26-30: INDIGO
  31-35: VIOLET
  36-40: WHITE
  41-45: RAINBOW
  (45 is the highest score)

To play again, just press the left button again. Scores are not tracked.
*/
int timeToReact = 4000;
char currentTask = 'U';
int const delayBetweenTasks = 500;
bool gameInProgress = false;
unsigned long timer = 0;
unsigned long debugTimer = 0;

void setup() {
  // put your setup code here, to run once: 
  CircuitPlayground.begin();

  CircuitPlayground.setAccelRange(LIS3DH_RANGE_4_G); // Sets range to +-4G (4x force of gravity)
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  // Starts the game when you press the left button.
  if (CircuitPlayground.leftButton() && gameInProgress == false) {
    currentTask = determineNextTask();
    lightDirection(currentTask);
    delay(200);
    gameInProgress = true;
    timer = millis();
  }

  // The rest of the fucking game
  else if (gameInProgress == true) {
    
    bool detectedMotion = checkForMotion();

    if (detectedMotion == true) {
      bool correctInput = checkMotionInput(currentTask);
      
      if (correctInput == true) {
        success();
        currentTask = determineNextTask();
        lightDirection(currentTask);
        timer = millis();
      } else {
        failure();
        gameInProgress = false;
      }
    }

    if (millis() > timer + timeToReact) {
      failure();
      gameInProgress = false;
    }

    detectedMotion = false;

  }

}

// Plays a short tone and then delays the program so the user has time
// to reposition the device.
void success () {
  for (int i = 0; i < 10; i++) {CircuitPlayground.setPixelColor(i, 0, 255, 0);}

  CircuitPlayground.playTone(5000, 200, false);
  delay(delayBetweenTasks);
  CircuitPlayground.clearPixels();
}

// Plays a short tone and then delays the program so the user has time
// to reposition the device.
void failure () {
  for (int i = 0; i < 10; i++) {CircuitPlayground.setPixelColor(i, 255, 0, 0);}

  CircuitPlayground.playTone(1000, 200, false);
  delay(delayBetweenTasks);
  CircuitPlayground.clearPixels();
}

void printDebug() {
  if (millis() > debugTimer + 500) {
    Serial.println("gameInProgress: ");
    Serial.print(gameInProgress);
    Serial.println();
    debugTimer = millis();
  }
}

bool checkForMotion() {
  float horizontal = CircuitPlayground.motionX();
  float vertical = CircuitPlayground.motionY();

  

  if (abs(horizontal) >= 15.0 || abs(vertical) >= 15.0) {
    return true;
  }
  else {
    return false;
  }
}

// Checks to see if the right motion input was made.
// Should only be called if a motion input was made over the amount to check in
// either direction.
// returns true if the right motion input has been made, false if any other
// are detected.
bool checkMotionInput(char direction) {
  float horizontal = CircuitPlayground.motionX();
  float vertical = CircuitPlayground.motionY();

  Serial.print("Horizontal is: ");
  Serial.println(horizontal);
  Serial.print("Vertical is: ");
  Serial.println(vertical);
  Serial.print("");

  bool correctMotion = false;

  switch (direction) {
    case 'U': // UP
      (vertical >= 15.0) ? correctMotion = true : correctMotion = false;
      break;

    case 'L': // LEFT
      (horizontal <= -15.0) ? correctMotion = true : correctMotion = false;
      break;

    case 'D': // DOWN
      (vertical <= -15.0) ? correctMotion = true : correctMotion = false;
      break;

    case 'R': // RIGHT
      (horizontal >= 15.0) ? correctMotion = true : correctMotion = false;
      break;
  }

  return correctMotion;
}

// Determines the next challenge for the player.
// Returns a character corresponding to the 6 tasks
char determineNextTask() {
  long randomNum = random(4);

  // Converts int to corresponding character
  // 0-3 are up, left, down, and right
  // 4 and 5 are yell and cover respectively.
  // Getting 4 or 5 at all is half as likely as the directions so that they feel special.
  switch (randomNum) {
    case 0: return 'U';
    case 1: return 'L';
    case 2: return 'D';
    case 3: return 'R';
    case 4: return 'Y'; //yell
    case 5: return 'C'; //cover
  }
}

// Creates lights that point to one of four directions.
// Accepts a char to determine the direction.
void lightDirection(char direction) { 

  switch (direction) {
    case 'U': // UP
      CircuitPlayground.setPixelColor(0, 0, 0, 255);
      CircuitPlayground.setPixelColor(9, 0, 0, 255);
      break;

    case 'L': // LEFT
      for (int i = 1; i < 4; i++) {
        CircuitPlayground.setPixelColor(i, 0, 0, 255);
      }
      break;

    case 'D': // DOWN
      for (int i = 4; i < 6; i++) {
        CircuitPlayground.setPixelColor(i, 0, 0, 255);
      }
      break;

    case 'R': // RIGHT
      for (int i = 6; i < 9; i++) {
        CircuitPlayground.setPixelColor(i, 0, 0, 255);
      }
      break;

    default: // Flashes an error animation.
      for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 10; i++) {
          CircuitPlayground.setPixelColor(i, 255, 0, 0);
        }
        delay(100);
        CircuitPlayground.clearPixels();
        delay(100);
      }
      
  }
} 
