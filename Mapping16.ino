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
  (40 is the highest score)

To play again, just press the left button again. Scores are not tracked.
*/
unsigned long timeToReact = 4000;
unsigned long timer = 0;
unsigned long debugTimer = 0;

char currentTask = 'U';
int const delayBetweenTasks = 500;
int score = 0;

bool gameInProgress = false;

// For mapping light value depending on ambient light
int minLightVal = 0;
int maxLightVal = 30; // lowest starting value
int targetLightVal = 20;
// P.S. The accelerometers are already mapped to easy to understand values so I didn't need to
// map them.

void setup() {
  // put your setup code here, to run once: 
  CircuitPlayground.begin();

  CircuitPlayground.setAccelRange(LIS3DH_RANGE_4_G); // Sets range to +-4G (4x force of gravity)

  pinMode(A8, INPUT);

  Serial.begin(9600);
  delay(1000);
}

void loop() {
  
  // Starts the game when you press the left button.
  if (CircuitPlayground.leftButton() && gameInProgress == false) {
    // Sets up first task
    currentTask = determineNextTask();
    lightDirection(currentTask);

    //Debounce
    delay(200);

    // Resets variables
    gameInProgress = true;
    timer = millis();
    timeToReact = 4000;
    score = 0;
  }

  // The rest of the fucking game
  else if (gameInProgress == true) {

    bool detectedMotion = checkForMotion();

    
    int lightReading = analogRead(A8);
    lightReading = map(lightReading, minLightVal, maxLightVal, 0, (targetLightVal * 5));

    if (currentTask == 'C') {
      if (lightReading < targetLightVal) {
        success();
      }
    }
    else if (detectedMotion == true) {
      bool correctInput = checkMotionInput(currentTask);
      
      if (correctInput == true) {
        success();

      } /* else { 
        failure();
        gameInProgress = false;
      } */
      // I cut out this failure state for doing the wrong input since sometimes 
      // the accelerometer would read the wrong direction for seemingly no reason.
    }

    // If the player takes too long, they lose.
    if (millis() > timer + timeToReact) {
      failure();
    }

    detectedMotion = false;

  }

  // Updates maximum brightness every second
  if (millis() % 1000 == 0) {
    maxLightVal = updateMaxLightVal();
    delay(1);
  } 

}

void tallyScore() {
  // RED 0-5
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 0)) break;
    CircuitPlayground.setPixelColor(i+5, 200, 0, 0);
    delay(80);
  }
  // ORANGE 6-10
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 5)) break;
    CircuitPlayground.setPixelColor(i+5, 0, 0, 0);
    CircuitPlayground.setPixelColor(i+5, 200, 100, 0);
    delay(80);
  }
  // YELLOW 11-15
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 10)) break;
    CircuitPlayground.setPixelColor(i+5, 0, 0, 0);
    CircuitPlayground.setPixelColor(i+5, 200, 200, 0);
    delay(80);
  }
  // GREEN 16-20
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 15)) break;
    CircuitPlayground.setPixelColor(i+5, 0, 0, 0);
    CircuitPlayground.setPixelColor(i+5, 0, 200, 0);
    delay(80);
  }
  // BLUE 21-25
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 20)) break;
    CircuitPlayground.setPixelColor(i+5, 0, 0, 0);
    CircuitPlayground.setPixelColor(i+5, 0, 0, 200);
    delay(80);
  }
  // INDIGO 26-30
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 25)) break;
    CircuitPlayground.setPixelColor(i+5, 0, 0, 0);
    CircuitPlayground.setPixelColor(i+5, 0 , 200, 210);
    delay(80);
  }
  // VIOLET 31-35
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 30)) break;
    CircuitPlayground.setPixelColor(i+5, 0, 0, 0);
    CircuitPlayground.setPixelColor(i+5, 100, 0, 200);
    delay(80);
  }
  // WHITE 36-40
  for (int i = 0; i < 5; i++) {
    if (score <= (i + 35)) break;
    CircuitPlayground.setPixelColor(i+5, 0, 0, 0);
    CircuitPlayground.setPixelColor(i+5, 200, 200, 200);
    delay(80);
  }

  delay(3000);
  CircuitPlayground.clearPixels();
}

int updateMaxLightVal() {
  if (analogRead(A8) > maxLightVal) {
    return analogRead(A8);
  } else {
    return maxLightVal;
  }
}

// Plays a short tone and then delays the program so the user has time
// to reposition the device.
void success () {
  for (int i = 0; i < 10; i++) {CircuitPlayground.setPixelColor(i, 0, 255, 0);}
  CircuitPlayground.playTone(5000, 200, false);
  delay(delayBetweenTasks);

  CircuitPlayground.clearPixels();

  // Increases score and sets up next task.
  score++;
  currentTask = determineNextTask();
  lightDirection(currentTask);

  // Resets timer and shortens time available to react to the next task.
  timer = millis();
  timeToReact -= 90;
}

// Plays a short tone and then delays the program so the user has time
// to reposition the device.
void failure () {
  for (int i = 0; i < 10; i++) {CircuitPlayground.setPixelColor(i, 255, 0, 0);}

  CircuitPlayground.playTone(1000, 200, false);
  delay(delayBetweenTasks);
  CircuitPlayground.clearPixels();

  delay(delayBetweenTasks);

  tallyScore();

  gameInProgress = false;
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

  float horizontal = 0;
  float vertical = 0;

  // Slightly smooths out motion to prevent misinputs
  for (int i = 0; i < 3; i++) {
    horizontal += CircuitPlayground.motionX();
    vertical += CircuitPlayground.motionY();
    delay(1);
  }
  horizontal = horizontal / 3;
  vertical = vertical / 3;

  Serial.print("Horizontal is: ");
  Serial.println(horizontal);
  Serial.print("Vertical is: ");
  Serial.println(vertical);
  Serial.print("Direction is: ");
  Serial.println(direction);
  bool correctMotion = false;

  switch (direction) {
    case 'U': // UP
      (vertical >= 0) ? correctMotion = true : correctMotion = false;
      break;

    case 'L': // LEFT
      (horizontal <= 0) ? correctMotion = true : correctMotion = false;
      break;

    case 'D': // DOWN
      (vertical <= 0) ? correctMotion = true : correctMotion = false;
      break;

    case 'R': // RIGHT
      (horizontal >= 0) ? correctMotion = true : correctMotion = false;
      break;
  }

  return correctMotion;
}

// Determines the next challenge for the player.
// Returns a character corresponding to the 6 tasks
char determineNextTask() {
  long randomNum = random(0, 5);

  // Converts int to corresponding character
  // 0-3 are up, left, down, and right
  // 4 and 5 are yell and cover respectively.
  // Getting 4 or 5 at all is half as likely as the directions so that they feel special.
  switch (randomNum) {
    case 0: return 'U';
    case 1: return 'L';
    case 2: return 'D';
    case 3: return 'R';
    case 4: return 'C';
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

    case 'C': // COVER
      for (int i = 0; i <  10; i++) {
        CircuitPlayground.setPixelColor(i, 100, 100, 100);
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
