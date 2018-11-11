/******************************************************************************
 * Bomb                                                                       *
 *                                                                            *
 * Bruno R. Arico                                                             *
 * Pedro L. Pereira                                                           *
 * Raphael R. Gusmao                                                          *
 ******************************************************************************/

#include <Wire.h>
#include <math.h>

typedef struct {
  int x;
  int y;
  int z;
  int temperature;
  int gx;
  int gy;
  int gz;
} accelerometer_values;

const static int MPU = 0x68;
#define pot1 A1
#define pot2 A2
#define red_led A3

long rnd_interval1;
long rnd_interval2;

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

  // //Envia valor Y do acelerometro para a serial e o LCD
  // Serial.print(" | AcY = "); Serial.print(accel.y);

  // //Envia valor Z do acelerometro para a serial e o LCD
  // Serial.print(" | AcZ = "); Serial.print(accel.z);

  // //Envia valor da temperatura para a serial e o LCD
  // //Calcula a temperatura em graus Celsius
  // Serial.print(" | Tmp = "); Serial.print(accel.temperature/340.00+36.53);

  // //Envia valor X do giroscopio para a serial e o LCD
  // Serial.print(" | GyX = "); Serial.print(accel.gx);

  // //Envia valor Y do giroscopio para a serial e o LCD
  // Serial.print(" | GyY = "); Serial.print(accel.gy);

  // //Envia valor Z do giroscopio para a serial e o LCD
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
  return true;
}

bool yellow_light() {
  // Button challenge
  return true;
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // Initializes MPU
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); Wire.write(0);
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
}

void defuse() {
}

void loop() {
  bool blue   = blue_light(),
       red    = red_light(),
       green  = green_light(),
       yellow = yellow_light();

  digitalWrite(3, blue ? HIGH : LOW);

  if (blue_light()
      && red_light()
      && green_light()
      && yellow_light()) {
        defuse();
  }

  delay(500);
}
