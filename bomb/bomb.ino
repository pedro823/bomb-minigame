#include <Wire.h>
#include <math.h>

#define RED_POTENTIOMETER1 A1
#define RED_POTENTIOMETER2 A2
#define RED_LED A3
// A4 and A5 are for Accelerometer
#define YELLOW_CHALL1 A6
#define YELLOW_CHALL2 A7
#define BLUE_LED 3
#define GREEN_LED 7
#define GREEN_CHALL1 8
#define GREEN_CHALL2 9
#define GREEN_CHALL3 10

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
timer_control time_fuse;

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
  return true;
}

bool green_light() {
  // Unplugging a wire challenge
  int  chall1 = !digitalRead(GREEN_CHALL1),
       chall2 = !digitalRead(GREEN_CHALL2),
       chall3 = !digitalRead(GREEN_CHALL3);

  return chall1 && chall2 && !chall3;
}

bool yellow_light() {
  // Button challenge
  return true;
}

void timer_begin(unsigned long int end_value) {
  time_fuse.end_time = end_value;
  time_fuse.begin_time = millis();
}

int timer_end_check() {
  time_fuse.now_time = time_fuse.end_time - (millis()-time_fuse.begin_time);
  if(time_fuse.now_time < 0) return true;
  else return false;
}


void setup() {
  Serial.begin(9600);
  // Initializes MPU
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); Wire.write(0);
  timer_begin(60000);
  byte error = Wire.endTransmission(false);
  if (error != 0) {
    Serial.write("WARNING: Accelerometer not found.");
  }

  pinMode(GREEN_CHALL1, INPUT_PULLUP);
  pinMode(GREEN_CHALL2, INPUT_PULLUP);
  pinMode(GREEN_CHALL3, INPUT_PULLUP);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
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

  if (blue_light()
      && red_light()
      && green_light()
      && yellow_light()) {
        defuse();
  }

  delay(500);
}

