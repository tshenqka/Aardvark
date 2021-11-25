/**********************************************************************
* Filename    : Multifunctional_IR_Remote_Car.ino
* Product     : Freenove 4WD Car for UNO
* Description : A Multifunctional IR-Remote Car.
* Auther      : www.freenove.com
* Modification: 2019/08/06
**********************************************************************/
#include "Freenove_WS2812B_RGBLED_Controller.h"
////////////////////////////////////////////
////Definitions related to IR-remote    --- included with pre-made code

#define PIN_SERVO     2
#define PIN_DIRECTION_LEFT  4
#define PIN_DIRECTION_RIGHT 3
#define PIN_MOTOR_PWM_LEFT  6
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_SONIC_TRIG    7
#define PIN_SONIC_ECHO    8
#define PIN_IRREMOTE_RECV 9
#define PIN_SPI_CE      9
#define PIN_SPI_CSN     10
#define PIN_SPI_MOSI    11
#define PIN_SPI_MISO    12
#define PIN_SPI_SCK     13

#define PIN_BATTERY     A0
#define PIN_BUZZER      A0
#define PIN_TRACKING_LEFT A1
#define PIN_TRACKING_CENTER A2
#define PIN_TRACKING_RIGHT  A3
#define MOTOR_PWM_DEAD    10

#define BAT_VOL_STANDARD  7.0

// Definitions related to ultrasonic sensor  --- copied from 02.2_Ultrasonic_Ranging example
#include "Servo.h"             //include servo library
 
#define PIN_SERVO      2       //define servo pin

#define PIN_SONIC_TRIG    7    //define Trig pin
#define PIN_SONIC_ECHO    8    //define Echo pin

#define MAX_DISTANCE    300   //cm
#define SONIC_TIMEOUT   (MAX_DISTANCE*60) // calculate timeout 
#define SOUND_VELOCITY  340  //soundVelocity: 340m/s

Servo servo;             //create servo object
byte servoOffset = 0;    //change the value to Calibrate servo

//Definitions related to Led-strip
#define STRIP_I2C_ADDRESS  0x20
#define STRIP_LEDS_COUNT   10

u8 colorPos = 0;
u8 colorStep = 50;
u8 stripDisplayMode = 1;
u8 currentLedIndex = 0;
u16 stripDisplayDelay = 100;
u32 lastStripUpdateTime = 0;
Freenove_WS2812B_Controller strip(STRIP_I2C_ADDRESS, STRIP_LEDS_COUNT, TYPE_GRB);

float batteryVoltage = 0;
bool isBuzzered = false;

//batteryVoltageCompensationToSpeed
int speedOffset;

// Servo parameters
int max_angle = 165;
bool is_clockwise = true;
int current_angle = 0;
int servo_delay = 15;

// Motor parameters
int speed = 100;

// Obstacle avoidance parameters
int min_distance = 10;
int turn_delay = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SONIC_TRIG, OUTPUT);// set trigPin to output mode
  pinMode(PIN_SONIC_ECHO, INPUT); // set echoPin to input mode
  servo.attach(PIN_SERVO);        //initialize servo 
  servo.write(90 + servoOffset);  // change servoOffset to Calibrate servo
  calculateVoltageCompensation();
  strip.begin();
}

// Loop function is called continuously
void loop() {
  //motorRun(speed + speedOffset, speed + speedOffset);
  servo_rotate_step();

}

////////// All code below is taken from the example code provided with the car kit
void pinsSetup() {
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);
  pinMode(PIN_SONIC_TRIG, OUTPUT);// set trigPin to output mode
  pinMode(PIN_SONIC_ECHO, INPUT); // set echoPin to input mode
  pinMode(PIN_TRACKING_LEFT, INPUT); // 
  pinMode(PIN_TRACKING_RIGHT, INPUT); // 
  pinMode(PIN_TRACKING_CENTER, INPUT); // 
  setBuzzer(false);
}

void servo_rotate_step(void) {
  if(is_clockwise) {
   current_angle++;
  } else {
   current_angle--;
  }
 
  servo.write(current_angle);
 
 if(current_angle == max_angle) {
   is_clockwise = false;
 } else if (current_angle == 180 - max_angle) {
   is_clockwise = true;
 }
 delay(servo_delay);
}

void obstacle_avoidance() {
  if(getSonar() < 10) {
    if(is_clockwise) {
      motorRun(-(speed + speedOffset), speed + speedOffset);
    }
    if(!is_clockwise) {
      motorRun((speed + speedOffset), -(speed + speedOffset));
    }
    delay(turn_delay);
  }
}

void motorRun(int speedl, int speedr) {
  int dirL = 0, dirR = 0;
  if (speedl > 0) {
    dirL = 0;
  }
  else {
    dirL = 1;
    speedl = -speedl;
  }
  if (speedr > 0) {
    dirR = 1;
  }
  else {
    dirR = 0;
    speedr = -speedr;
  }
  speedl = constrain(speedl, 0, 255);
  speedr = constrain(speedr, 0, 255);
  if (abs(speedl) < MOTOR_PWM_DEAD && abs(speedr) < MOTOR_PWM_DEAD) {
    speedl = 0;
    speedr = 0;
  }

  digitalWrite(PIN_DIRECTION_LEFT, dirL);
  digitalWrite(PIN_DIRECTION_RIGHT, dirR);
  analogWrite(PIN_MOTOR_PWM_LEFT, speedl);
  analogWrite(PIN_MOTOR_PWM_RIGHT, speedr);
}

void calculateVoltageCompensation() {
  float voltageOffset = 8.4 - getBatteryVoltage();
  speedOffset = voltageOffset * 20;
}

bool getBatteryVoltage() {
  if (!isBuzzered) {
    pinMode(PIN_BATTERY, INPUT);
    int batteryADC = analogRead(PIN_BATTERY);
    if (batteryADC < 614)   // 3V/12V ,Voltage read: <2.1V/8.4V
    {
      batteryVoltage = batteryADC / 1023.0 * 5.0 * 4;
      return true;
    }
  }
  return false;
}

void setBuzzer(bool flag) {
  isBuzzered = flag;
  pinMode(PIN_BUZZER, flag);
  digitalWrite(PIN_BUZZER, flag);
}

void alarm(u8 beat, u8 repeat) {
  beat = constrain(beat, 1, 9);
  repeat = constrain(repeat, 1, 255);
  for (int j = 0; j < repeat; j++) {
    for (int i = 0; i < beat; i++) {
      setBuzzer(true);
      delay(100);
      setBuzzer(false);
      delay(100);
    }
    delay(500);
  }
}

void resetCarAction() {
  motorRun(0, 0);
  setBuzzer(false);
}

float getSonar() {
  unsigned long pingTime;
  float distance;
  digitalWrite(PIN_SONIC_TRIG, HIGH); // make trigPin output high level lasting for 10μs to triger HC_SR04,
  delayMicroseconds(10);
  digitalWrite(PIN_SONIC_TRIG, LOW);
  pingTime = pulseIn(PIN_SONIC_ECHO, HIGH, SONIC_TIMEOUT); // Wait HC-SR04 returning to the high level and measure out this waitting time
  if (pingTime != 0)
    distance = (float)pingTime * SOUND_VELOCITY / 2 / 10000; // calculate the distance according to the time
  else
    distance = MAX_DISTANCE;
  return distance; // return the distance value
}
