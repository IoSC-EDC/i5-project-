// ===== LFR: Arduino Uno + TB6612FNG + 5 analog IR array =====
// Fixed threshold version (line reads LOW ~25, white reads HIGH ~1020)

// --- Motor driver pins ---
#define PWMA 3
#define AIN1 7
#define AIN2 8
#define STBY 4
#define BIN1 2
#define BIN2 13
#define PWMB 11

// --- Sensors (S1 = leftmost, S5 = rightmost) ---
const uint8_t sensorPin[5] = {A0, A1, A2, A3, A4};
unsigned long lostSince = 0;   // add near the other globals
const int weight[5] = {0, 1000, 2000, 3000, 4000};
const bool STEER_INVERT = false;   // flips left/right steering

// --- Threshold (raw analogRead, 0-1023) ---
// Line ~25, white ~1020  ->  midpoint ~520.
// Raw value BELOW this = sensor is on the line.
const int THRESHOLD = 500;

// --- Settings to adjust after testing ---
const bool LEFT_REVERSE  = false;    // true if the left wheel spins backward
const bool RIGHT_REVERSE = false;    // true if the right wheel spins backward
const bool DEBUG = false;            // true = print values to Serial Monitor

// --- PID tuning ---
float Kp = 0.007;
float Ki = 0.0;
float Kd = 0.8;
int   baseSpeed = 220;               // start low, raise gradually
int   maxSpeed  = 255;
int   turnSpeed = 140;               // used when the line is lost

int lastError = 0;
float integral = 0;
unsigned long lastLoop = 0;
const unsigned long LOOP_US = 4000;  // fixed 4 ms loop

// ---------- Motor control ----------
void setMotor(uint8_t in1, uint8_t in2, uint8_t pwm, int speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0)      { digitalWrite(in1, HIGH); digitalWrite(in2, LOW);  }
  else if (speed < 0) { digitalWrite(in1, LOW);  digitalWrite(in2, HIGH); }
  else                { digitalWrite(in1, LOW);  digitalWrite(in2, LOW);  }
  analogWrite(pwm, abs(speed));
}
void drive(int left, int right) {
  if (LEFT_REVERSE)  left  = left;
  if (RIGHT_REVERSE) right = right;
  setMotor(AIN1, AIN2, PWMA, left);    // A = left motor
  setMotor(BIN1, BIN2, PWMB, right);   // B = right motor
}

// ---------- Sensors ----------
// Returns true if the line is seen; position = 0..4000 (2000 = centered)
bool readLine(int &position) {
  long sum = 0, weighted = 0;
  bool seen = false;

  for (int i = 0; i < 5; i++) {
    int raw = analogRead(sensorPin[i]);
    int v = constrain(map(raw, 25, 1020, 1000, 0), 0, 1000); // 1000 = on line, 0 = floor

    if (raw < THRESHOLD) seen = true;   // threshold decides "line seen"
    if (v < 200) v = 0;                 // ignore weak noise in the average

    sum += v;
    weighted += (long)v * weight[i];

    if (DEBUG) { Serial.print(raw); Serial.print('\t'); }
  }
  if (DEBUG) Serial.println();

  if (!seen || sum == 0) return false;
  position = weighted / sum;
  return true;
}

// ---------- Setup ----------
void setup() {
  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);            // enable driver
  if (DEBUG) Serial.begin(9600);
  delay(2000);                         // time to place the robot on the line
}

// ---------- Main loop ----------
void loop() {
  if (micros() - lastLoop < LOOP_US) return;
  lastLoop = micros();

  int pos;
  if (!readLine(pos)) {
    if (lostSince == 0) lostSince = millis();

    if (millis() - lostSince < 40) {
      return;                       // keep the last motor command briefly
    }

    bool goLeft = (lastError < 0);
    if (STEER_INVERT) goLeft = !goLeft;
    if (goLeft) drive(-turnSpeed, turnSpeed);
    else        drive(turnSpeed, -turnSpeed);
    return;
  }
  lostSince = 0;                    // line found again

  int error = pos - 2000;              // + = line to the right
  integral = constrain(integral + error, -5000, 5000);
  float derivative = error - lastError;
  lastError = error;

  float correction = Kp * error + Ki * integral + Kd * derivative;

  // Slow down on sharp corners
  int speed = baseSpeed - abs(error) / 25;
  speed = constrain(speed, 100, maxSpeed);

    float c = STEER_INVERT ? -correction : correction;

  drive(constrain(speed + c, -255, maxSpeed),
        constrain(speed - c, -255, maxSpeed));
}