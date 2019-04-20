#include "Spider.h"

Spider spider;

void setup() {
  Serial.begin(9600);
  Serial.println("Spider starting");
  Servo::initPwm();
  InputOutput::initInsAndOuts();
}

void loop() {
  // update the length of the tick, in case the speed knob has been turned
  tick = InputOutput::determineTickLengthFromPotentiometer();

  // to make the spider blink, turn LEDs on when 1st leg is lifting, off when forward, on when lowering, off when backwards
  // to blink only one eye when turning, track the direction of the movement
  LegPhase legPhase = legs[0]->getPhase();
  bool turnLedOn = (legPhase == LIFT || legPhase == LOWER);
  bool rightLedOn = turnLedOn && (spider.getMoveDir() != ROTATE_LEFT);
  bool leftLedOn = turnLedOn && (spider.getMoveDir() != ROTATE_RIGHT);
  InputOutput::turnLedsOnOff(rightLedOn, leftLedOn);

  if (InputOutput::isStrategyButtonPressed()) {
    // it will remember this, and switch to the next strategy only once all legs are down
    spider.switchToNextStrategyWhenPossible();
  }

  spider.moveOneTick();
}
