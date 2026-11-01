void loop() {
  // 1. Fixed timing loop (runs every 4 ms)
  if (micros() - lastLoop < LOOP_US) return;
  lastLoop = micros();

  int pos;
  // 2. Read sensors and check if line is found
  if (!readLine(pos)) {
    if (lostSince == 0) lostSince = millis();

    // Grace period: keep previous motor command briefly (40 ms)
    if (millis() - lostSince < 40) {
      return;                       
    }

    // Line lost recovery: spin in place toward last known error direction
    bool goLeft = (lastError < 0);
    if (STEER_INVERT) goLeft = !goLeft;
    if (goLeft) drive(-turnSpeed, turnSpeed);
    else        drive(turnSpeed, -turnSpeed);
    return;
  }
  lostSince = 0;                    // Line found again, reset timer

  // 3. PID Error Calculation
  int error = pos - 2000;            // Center is 2000 (+ = line to the right)
  integral = constrain(integral + error, -5000, 5000);
  float derivative = error - lastError;
  lastError = error;

  float correction = Kp * error + Ki * integral + Kd * derivative;

  // 4. Dynamic Speed Control (Slow down on sharp corners)
  int speed = baseSpeed - abs(error) / 25;
  speed = constrain(speed, 100, maxSpeed);

  float c = STEER_INVERT ? -correction : correction;

  // 5. Apply Speeds to Motors
  drive(constrain(speed + c, -255, maxSpeed),
        constrain(speed - c, -255, maxSpeed));
}
