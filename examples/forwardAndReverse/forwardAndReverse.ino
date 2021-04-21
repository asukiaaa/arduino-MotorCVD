#include <MotorCVD_asukiaaa.hpp>

#define SERIAL_RS485 Serial1
#define PIN_DE 5
#define PIN_RE 5

MotorCVD_asukiaaa::Driver motor(&SERIAL_RS485, PIN_DE, PIN_RE);

void setup() {
  while(!Serial) { delay(10); }
  Serial.begin(115200);
  Serial.println("start");
  motor.begin();
}

void loop() {
  uint32_t info;
  int32_t position;
  const int32_t stepToMove = 8000;
  int commState = motor.readInformation(&info);
  if (commState != 0) {
    Serial.println("Cannot communicate with driver. Error: " + String(commState));
    delay(2000);
    return;
  }
  Serial.println("Information: " + MotorCVD_asukiaaa::getLabelsOfInformation(info));

  motor.readPosition(&position);
  Serial.println("Froward from " + String(position));
  motor.writeSpeed(MotorCVD_asukiaaa::SPEED_MAX);
  delay(1000);
  motor.writeSpeed(0);
  delay(2000);

  motor.readPosition(&position);
  Serial.println("Reverse from " + String(position));
  motor.writeSpeed(-MotorCVD_asukiaaa::SPEED_MAX);
  delay(1000);
  motor.writeSpeed(0);
  delay(2000);

  motor.readPosition(&position);

  Serial.println("Move from " + String(position) + " to " + String(position + stepToMove) + " with half speed");
  motor.writeStep(MotorCVD_asukiaaa::SPEED_MAX / 2, stepToMove);
  delay(4000);
  motor.writeSpeed(0);
  delay(2000);


  motor.readPosition(&position);
  Serial.println("Move from " + String(position) + " to " + String(position - stepToMove) + " with 1/4 speed");
  motor.writeStep(MotorCVD_asukiaaa::SPEED_MAX / 4, -stepToMove);
  delay(6000);
  motor.writeSpeed(0);
  delay(2000);
}
