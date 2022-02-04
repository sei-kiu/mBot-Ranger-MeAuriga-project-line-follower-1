#include <Arduino.h>
#include <MeAuriga.h>

#define FORWARD 1
#define REVERSE 2
#define LEFT 3
#define RIGHT 4
#define STOP 5

MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);

MeLineFollower linefollower_9(9);

void isr_process_encoder1(void);
void isr_process_encoder2(void);

void moveDuration(float seconds);
void move(int direction, int speed);

void setup()
{
  Serial.begin(115200);

  // Set PWM 8 KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);

  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
}

void loop()
{
  int sensorState = linefollower_9.readSensors();

  switch (sensorState)
  {
  case S1_IN_S2_IN:
    Serial.println("Sensor 1 and 2 are inside of black line");
    // FORWARD at 25% for 0.5 second
    move(FORWARD, 25 / 100.0 * 255);
    moveDuration(0.5);
    break;
  case S1_IN_S2_OUT:
    Serial.println("Sensor 2 (on right side) is outside of black line");
    // LEFT at 25% for 0.5 second
    move(LEFT, 25 / 100.0 * 255);
    moveDuration(0.5);
    break;
  case S1_OUT_S2_IN:
    Serial.println("Sensor 1 (on left side) is outside of black line");
    // RIGHT at 25% for 0.5 second
    move(RIGHT, 25 / 100.0 * 255);
    moveDuration(0.5);
    break;
  case S1_OUT_S2_OUT:
    Serial.println("Sensor 1 and 2 are outside of black line");
    // Stop moving
    move(STOP, 0);
    moveDuration(2);
    break;
  default:
    break;
  }
}

void isr_process_encoder1(void)
{
  if (digitalRead(Encoder_1.getPortB()) == 0)
  {
    Encoder_1.pulsePosMinus();
  }
  else
  {
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(void)
{
  if (digitalRead(Encoder_2.getPortB()) == 0)
  {
    Encoder_2.pulsePosMinus();
  }
  else
  {
    Encoder_2.pulsePosPlus();
  }
}

void move(int direction, int speed)
{
  int leftSpeed = 0;
  int rightSpeed = 0;
  if (direction == FORWARD)
  {
    leftSpeed = -speed;
    rightSpeed = speed;
  }
  else if (direction == REVERSE)
  {
    leftSpeed = speed;
    rightSpeed = -speed;
  }
  else if (direction == LEFT)
  {
    leftSpeed = -speed;
    rightSpeed = -speed;
  }
  else if (direction == RIGHT)
  {
    leftSpeed = speed;
    rightSpeed = speed;
  }
  else if (direction == STOP)
  {
    leftSpeed = 0;
    rightSpeed = 0;
  }
  Encoder_1.setTarPWM(leftSpeed);
  Encoder_2.setTarPWM(rightSpeed);
}

void moveDuration(float seconds)
{
  if (seconds < 0.0)
  {
    seconds = 0.0;
  }
  unsigned long endTime = millis() + seconds * 1000;
  while (millis() < endTime)
  {
    Encoder_1.loop();
    Encoder_2.loop();
  }
}