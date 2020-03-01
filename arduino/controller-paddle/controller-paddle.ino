// ABSOLUTE PADDLE
// sends value between MIN and MAXSCALE

// PIN ASSIGNMENTS
#define ENCODER0PINA 2 // Must support interupts
#define ENCODER0PINB 3 // Must support interupts
//#define HOMESWITCH0 4  // Comment-out if no HOMESWITCH
#define LOCSWITCH0 5 // Controller L/R detect pin; default to L

// CONSTANTS
#define MAX 65535
#define MIN 0
#define MAXSCALE 29000

// GLOBALS
volatile unsigned int encoder0Pos = 0;
volatile boolean encoderChanged = true;
volatile char controlLoc = 'X'; // overwritten during setup

// SETUP
void setup()
{
  pinMode(ENCODER0PINA, INPUT);
  pinMode(ENCODER0PINB, INPUT);
#ifdef HOMESWITCH0
  pinMode(HOMESWITCH0, INPUT);
#endif
#ifdef LOCSWITCH0
  pinMode(LOCSWITCH0, INPUT);
#endif

  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);

  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);

  Serial.begin(115200);
  Serial.println("!BP2020-ENCODER");
  if (digitalRead(LOCSWITCH0) == LOW)
  {
    controlLoc = 'R';
    Serial.println("!Starting Right Controller");
  }
  else
  {
    controlLoc = 'L';
    Serial.println("!Starting Left Controller");
  }
#ifndef HOMESWITCH0
  Serial.println("!WARNING ! HOMESWITCH0 IS DISABLED");
#endif
}

// MAIN RUN-LOOP
void loop()
{
// Check the home switch status
// if activated, zero position
#ifdef HOMESWITCH0
  if (digitalRead(HOMESWITCH0) == LOW)
  {
    encoder0Pos = MAXSCALE;
  }
#endif

  // Output current position and controller location
  // ex: "R3034\n" for right controller
  // ex: "L3034\n" for left controller
  if (encoderChanged) {
    Serial.print(controlLoc);
    Serial.println(encoder0Pos, DEC);
    encoderChanged = false;
    delay(16);
  }
  
}

// FUNCTIONS
int incrementPos()
{
  if (encoder0Pos < MAX)
    ++encoder0Pos;
  encoderChanged = true;
  return encoder0Pos;
}

int decrementPos()
{
  if (encoder0Pos > MIN)
    --encoder0Pos;
  encoderChanged = true;
  return encoder0Pos;
}

// Channel A Interupt Routine
void doEncoderA()
{
  // look for a low-to-high on channel A
  if (digitalRead(ENCODER0PINA) == HIGH)
  {
    // check channel B to see which way encoder is turning
    if (digitalRead(ENCODER0PINB) == LOW)
      decrementPos();
   else
      incrementPos();
  }
  else
  { // Must be a high-to-low transistion
    if (digitalRead(ENCODER0PINB) == HIGH)
      decrementPos();
    else
      incrementPos();
  }
}

// Channel B Interupt Routine
void doEncoderB()
{
  // look for a low-to-high on channel B
  if (digitalRead(ENCODER0PINB) == HIGH)
  {
    // check channel A to see which way encoder is turning
    if (digitalRead(ENCODER0PINA) == HIGH)
      decrementPos();
    else
      incrementPos();
  }
  else
  { // Must be a high-to-low transistion
    if (digitalRead(ENCODER0PINA) == LOW)
      decrementPos();
    else
      incrementPos();
  }
}