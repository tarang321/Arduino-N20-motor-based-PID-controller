#include <PIDController.h>
/* ENCODER_A and ENCODER_B pins are used to read the encoder
   data from the microcontroller, the data from the encoder
   comes very fast so these two pins have to be interrupt enabled
   pins
*/
#define ENCODER_A 14
#define ENCODER_B 12
/* the MOTOR_CW and MOTOR_CCW pins are used to drive the H-bridge
   the H-bridge then drives the motors, This two pins must have to
   be PWM enabled, otherwise the code will not work.
*/
#define MOTOR_CW 26
#define MOTOR_CCW 27
/*In this section we have defined the gain values for the
   proportional,integral, and derivative controller i have set
   the gain values with the help of trial and error methods.
*/
#define __Kp 260 // Proportional constant
#define __Ki 2.7 // Integral Constant
#define __Kd 2000 // Derivative Constant
volatile long int encoder_count = 0; // stores the current encoder count
unsigned int integerValue = 0; // stores the incoming serial value. Max value is 65535
char incomingByte; // parses and stores each individual character one by one
int motor_pwm_value = 255; // after PID computation data is stored in this variable.
PIDController pidcontroller;
void setup() {
  Serial.begin(115200); // Serial for Debugging
  pinMode(ENCODER_A, INPUT); // ENCODER_A as Input
  pinMode(ENCODER_B, INPUT); // ENCODER_B as Input
  pinMode(MOTOR_CW, OUTPUT); // MOTOR_CW as Output
  pinMode(MOTOR_CCW, OUTPUT); // MOTOR_CW as Output
  /* attach an interrupt to pin ENCODER_A of the Arduino, and when the
      pulse is in the RISING edge called the function encoder().
  */
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoder, RISING);
  pidcontroller.begin(); // initialize the PID instance
  pidcontroller.tune(260, 2.7, 2000); // Tune the PID, arguments: kP, kI, kD
  pidcontroller.limit(-255, 255); // Limit the PID output between -255 to 255, this is important to get rid of integral windup!
}
void loop() {
  while (Serial.available() > 0) {
    integerValue = Serial.parseInt(); // stores the integerValue
    incomingByte = Serial.read(); // stores the /n character
    if (incomingByte == '\n') // if we receive a newline character we will continue in the loop
      continue;
  }
  pidcontroller.setpoint(integerValue); // The "goal" the PID controller tries to "reach",
  Serial.println(integerValue); // print the incoming value for debugging
  motor_pwm_value = pidcontroller.compute(encoder_count);  //Let the PID compute the value, returns the calculated optimal output
  Serial.print(motor_pwm_value); // print the calculated value for debugging
  Serial.print("   ");
  if (motor_pwm_value > 0) // if the motor_pwm_value is greater than zero we rotate the  motor in clockwise direction
    motor_ccw(motor_pwm_value);
  else // else we move it in a counter clockwise direction
    motor_cw(abs(motor_pwm_value));
  Serial.println(encoder_count);// print the final encoder count.
}
void encoder() {
  if (digitalRead(ENCODER_B) == HIGH) // if ENCODER_B is high increase the count
    encoder_count++; // increment the count
  else // else decrease the count
    encoder_count--;  // decrement the count
}
void motor_cw(int power) {
  if (power > 100) {
    ledcWrite(MOTOR_CW, power); //rotate the motor if the value is grater than 100
    digitalWrite(MOTOR_CCW, LOW); // make the other pin LOW
  }
  else {
    // both of the pins are set to low    
    digitalWrite(MOTOR_CW, LOW);
    digitalWrite(MOTOR_CCW, LOW);
  }
}
void motor_ccw(int power) {
  if (power > 100) {
    ledcWrite(MOTOR_CCW, power);
    digitalWrite(MOTOR_CW, LOW);
  }
  else {
    digitalWrite(MOTOR_CW, LOW);
    digitalWrite(MOTOR_CCW, LOW);
  }
}
