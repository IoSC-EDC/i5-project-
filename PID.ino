const unsigned long LOOP_US = 4000; // 4 ms fixed loop
int error = pos - 2000; // + = line to the right
float Kp = 0.007; // Proportional gain
float Ki = 0.0;   // Integral gain
float Kd = 0.8;   // Derivative gain
int speed = baseSpeed - abs(error) / 25;
speed = constrain(speed, 100, maxSpeed);
drive(constrain(speed + c, -255, maxSpeed),
      constrain(speed - c, -255, maxSpeed));
