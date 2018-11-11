/******************************************************************************
 * Bomb                                                                       *
 *                                                                            *
 * Bruno R. Arico                                                             *
 * Pedro L. Pereira                                                           *
 * Raphael R. Gusmao                                                          *
 ******************************************************************************/

#include <Wire.h>
#include <math.h>

#define RED_POTENTIOMETER1 A1
#define RED_POTENTIOMETER2 A2
#define RED_LED A3
// A4 and A5 are for Accelerometer
#define YELLOW_CHALL1 A6
#define YELLOW_CHALL2 A7
#define YELLOW_CHALL3 0
#define YELLOW_CHALL4 1

#define BLUE_LED 3
#define YELLOW_LED A0
#define GREEN_LED 7

#define GREEN_CHALL1 8
#define GREEN_CHALL2 9
#define GREEN_CHALL3 10

enum button_pressed {
  NO_BUTTON,
  BUTTON1,
  BUTTON2,
  BUTTON3,
  BUTTON4
};

typedef struct {
  int x;
  int y;
  int z;
  int temperature;
  int gx;
  int gy;
  int gz;
} accelerometer_values;

typedef struct {
  unsigned long int begin_time;
  unsigned long int end_time;
  long int now_time;
} timer_control;

const static int MPU = 0x68;

#define pot1 A1
#define pot2 A2
#define red_led A3

long rnd_interval1;
long rnd_interval2;

timer_control time_fuse;
int buttons_are_correct = false;


accelerometer_values get_accelerometer_values() {
  accelerometer_values values;
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // get data from sensor
  values.x = Wire.read() << 8 | Wire.read();
  values.y = Wire.read() << 8 | Wire.read();
  values.z = Wire.read() << 8 | Wire.read();
  values.temperature = Wire.read() << 8 | Wire.read();
  values.gx = Wire.read() << 8 | Wire.read();
  values.gy = Wire.read() << 8 | Wire.read();
  values.gz = Wire.read() << 8 | Wire.read();
  return values;
}

bool is_close_to(int value, int desired, int tolerance) {
  return abs(value - desired) < tolerance;
}

bool blue_light() {
  // Accelerometer challenge
  accelerometer_values accel = get_accelerometer_values();

  // Serial.print("AcX = "); Serial.print(accel.x);

  // Serial.print(" | AcY = "); Serial.print(accel.y);
   
  // Serial.print(" | AcZ = "); Serial.print(accel.z);
   
  // Serial.print(" | Tmp = "); Serial.print(accel.temperature/340.00+36.53);
   
  // Serial.print(" | GyX = "); Serial.print(accel.gx);

  // Serial.print(" | GyY = "); Serial.print(accel.gy);
   
  // Serial.print(" | GyZ = "); Serial.println(accel.gz);

  return is_close_to(accel.x, 11500, 4200)
         && is_close_to(accel.y, -2000, 4200)
         && is_close_to(accel.z, -12000, 4200);
}

bool red_light() {
  // Potentiometer challenge

  float pot1_value = map(analogRead(pot1), 0, 915, 0, 900);
  float pot2_value = map(analogRead(pot2), 0, 915, 0, 900);

  Serial.print("Pot1: ");Serial.print(pot1_value);Serial.print("\n");
  Serial.print("Pot2: ");Serial.print(pot2_value);Serial.print("\n\n");

  if (rnd_interval1*100 < pot1_value && pot1_value <= (rnd_interval1+1)*100
   && rnd_interval2*100 < pot2_value && pot2_value <= (rnd_interval2+1)*100) {
    digitalWrite(red_led, HIGH);
  } else {
    digitalWrite(red_led, LOW);
  }

  return true;
}

bool green_light() {
  // Unplugging a wire challenge
  int  chall1 = !digitalRead(GREEN_CHALL1),
       chall2 = !digitalRead(GREEN_CHALL2),
       chall3 = !digitalRead(GREEN_CHALL3);

  return chall1 && chall2 && !chall3;
}

bool check_guess(int guess) {
  static const int answer = 0x02030104;
  int checker;
  int answer_iterator = answer;
  for (checker = guess & 0xff; guess; guess >>= 2) {
    if (checker != answer_iterator & 0xff) {
      return false;
    }
    answer_iterator >>= 2;
    delay(1000);
  }
  return true;
}

enum button_pressed check_button_press() {
  if (!digitalRead(YELLOW_CHALL1)) {
    return BUTTON1;
  }
  if (!digitalRead(YELLOW_CHALL2)) {
    return BUTTON2;
  }
  if (!digitalRead(YELLOW_CHALL3)) {
    return BUTTON3;
  }
  if (!digitalRead(YELLOW_CHALL4)) {
    return BUTTON4;
  }
  return NO_BUTTON;
}

bool yellow_light() {
  // Button challenge
  static const int answer = 0x02030104;
  static int current = 0x0;

  if (buttons_are_correct) {
    return true;
  }

  if (current > 0x01000000) {
    // check if right
    if (check_guess(current)) {
      buttons_are_correct = true;
      return true;
    }

    current = 0;
    return false;
  }
  else {
    // check if button was pressed
    enum button_pressed bp = check_button_press();
    if (bp != NO_BUTTON) {
      current = (current << 2) | (int) bp;
    }

    return false;
  }
}

void timer_begin(unsigned long int end_value) {
  time_fuse.end_time = end_value;
  time_fuse.begin_time = millis();
}

int timer_end_check() {
  time_fuse.now_time = time_fuse.end_time - (millis()-time_fuse.begin_time);
  return time_fuse.now_time < 0;
}


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // Initializes MPU
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); Wire.write(0);
  timer_begin(60000);
  byte error = Wire.endTransmission(false);
  if (error != 0) {
    Serial.write("WARNING: Accelerometer not found.");
  }

  // Potentiometer
  pinMode(3, OUTPUT);
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(red_led, OUTPUT);
  rnd_interval1 = random(0, 9);
  rnd_interval2 = random(0, 9);

  pinMode(GREEN_CHALL1, INPUT_PULLUP);
  pinMode(GREEN_CHALL2, INPUT_PULLUP);
  pinMode(GREEN_CHALL3, INPUT_PULLUP);

  pinMode(YELLOW_CHALL1, INPUT_PULLUP);
  pinMode(YELLOW_CHALL2, INPUT_PULLUP);
  pinMode(YELLOW_CHALL3, INPUT_PULLUP);
  pinMode(YELLOW_CHALL4, INPUT_PULLUP);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
}

void defuse() {
}

void loop() {
  timer_end_check();
  bool blue   = blue_light(),
       red    = red_light(),
       green  = green_light(),
       yellow = yellow_light();

  digitalWrite(BLUE_LED, blue ? HIGH : LOW);
  digitalWrite(GREEN_LED, green ? HIGH : LOW);
  digitalWrite(YELLOW_LED, yellow ? HIGH : LOW);

  if (blue_light()
      && red_light()
      && green_light()
      && yellow_light()) {
        defuse();
  }

  delay(500);
}

