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
  // Initializes MPU
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); Wire.write(0);
  byte error = Wire.endTransmission(false);
  if (error != 0) {
    Serial.write("WARNING: Accelerometer not found.");
  }
  pinMode(3, OUTPUT);
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
