#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// ---------------- PIN DEFINITIONS ----------------
const int L_RPWM = 1;
const int L_LPWM = 2;
const int R_RPWM = 4;
const int R_LPWM = 5;

const int CH1_STEER    = 10;
const int CH2_THROTTLE = 11;
const int CH5_MODE     = 12;

const int SDA_PIN = 8;
const int SCL_PIN = 9;

// ---------------- MPU SETUP ----------------
Adafruit_MPU6050 mpu;

float smoothAccelY = 0.0f;
float smoothAccelZ = 0.0f;
const float ACCEL_ALPHA = 0.12f;

// ---------------- RC INTERRUPT VARIABLES ----------------
volatile unsigned long ch1Rise = 0, ch2Rise = 0, ch5Rise = 0;
volatile int ch1Pulse = 1500, ch2Pulse = 1500, ch5Pulse = 1500;

// ---------------- ISR FUNCTIONS ----------------
void IRAM_ATTR isr_ch1() {
  if (digitalRead(CH1_STEER)) ch1Rise = micros();
  else ch1Pulse = micros() - ch1Rise;
}

void IRAM_ATTR isr_ch2() {
  if (digitalRead(CH2_THROTTLE)) ch2Rise = micros();
  else ch2Pulse = micros() - ch2Rise;
}

void IRAM_ATTR isr_ch5() {
  if (digitalRead(CH5_MODE)) ch5Rise = micros();
  else ch5Pulse = micros() - ch5Rise;
}

// ---------------- MOTOR SMOOTHING ----------------
float smoothLeft = 0.0f;
float smoothRight = 0.0f;
const float RAMP_RATE = 5.0f;

// ---------------- CONTROL PARAMETERS ----------------
const int RC_DEADZONE = 20;
const float EXPO = 0.4f;

// ---------------- BALANCE CONTROL ----------------
float balanceOffset = 0.0f;
const float kP = 1.8f;

// --------------------------------------------------

void setup() {
  Serial.begin(115200);

  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);

  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, 0);

  pinMode(CH1_STEER, INPUT);
  pinMode(CH2_THROTTLE, INPUT);
  pinMode(CH5_MODE, INPUT);

  attachInterrupt(digitalPinToInterrupt(CH1_STEER), isr_ch1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH2_THROTTLE), isr_ch2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH5_MODE), isr_ch5, CHANGE);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);

  // Initialize filter
  for (int i = 0; i < 50; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    smoothAccelY += a.acceleration.y;
    smoothAccelZ += a.acceleration.z;
    delay(5);
  }

  smoothAccelY /= 50.0f;
  smoothAccelZ /= 50.0f;

  Serial.println("MPU Ready");
}

// ---------------- RC MAPPING ----------------
int rcToMotor(int pulse) {
  pulse = constrain(pulse, 1000, 2000);
  int centered = pulse - 1500;

  if (abs(centered) < RC_DEADZONE) return 0;

  int sign = (centered > 0) ? 1 : -1;
  int value = abs(centered) - RC_DEADZONE;

  float norm = (float)value / (500.0f - RC_DEADZONE);
  float expo_out = EXPO * norm * norm * norm + (1.0f - EXPO) * norm;

  return sign * (int)(expo_out * 255.0f);
}

// ---------------- MOTOR CONTROL ----------------
void setMotors(float targetLeft, float targetRight) {
  smoothLeft += constrain(targetLeft - smoothLeft, -RAMP_RATE, RAMP_RATE);
  smoothRight += constrain(targetRight - smoothRight, -RAMP_RATE, RAMP_RATE);

  int L = (int)smoothLeft;
  int R = (int)smoothRight;

  analogWrite(L_RPWM, L > 0 ? L : 0);
  analogWrite(L_LPWM, L < 0 ? -L : 0);

  analogWrite(R_RPWM, R > 0 ? R : 0);
  analogWrite(R_LPWM, R < 0 ? -R : 0);
}

// ---------------- MAIN LOOP ----------------
void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  smoothAccelY = ACCEL_ALPHA * a.acceleration.y + (1.0f - ACCEL_ALPHA) * smoothAccelY;
  smoothAccelZ = ACCEL_ALPHA * a.acceleration.z + (1.0f - ACCEL_ALPHA) * smoothAccelZ;

  float tiltAngle = atan2f(smoothAccelY, smoothAccelZ) * (180.0f / PI);

  int steer = rcToMotor(ch1Pulse);
  int throttle = rcToMotor(ch2Pulse);
  bool autoBalance = (ch5Pulse > 1700);

  float leftTarget, rightTarget;

  if (autoBalance) {
    float balanceOutput = (tiltAngle - balanceOffset) * kP;
    balanceOutput = constrain(balanceOutput, -255, 255);

    leftTarget = balanceOutput + steer;
    rightTarget = balanceOutput - steer;
  } else {
    leftTarget = throttle + steer;
    rightTarget = throttle - steer;
  }

  leftTarget = constrain(leftTarget, -255, 255);
  rightTarget = constrain(rightTarget, -255, 255);

  setMotors(leftTarget, rightTarget);

  Serial.print("Tilt: ");
  Serial.print(tiltAngle);
  Serial.print(" | L: ");
  Serial.print((int)smoothLeft);
  Serial.print(" | R: ");
  Serial.println((int)smoothRight);
}
