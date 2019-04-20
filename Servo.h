#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define V_SERVO_MIN  480 //480
#define V_SERVO_MAX  550 //550
#define H_SERVO_MIN  260 //255
#define H_SERVO_MAX  340 //345

#define MIN_TICK 1
#define MAX_TICK 10

#define LOG_EACH_MOVE false

static float tick;

enum ServoRotateDir {
  TO_MIN,
  TO_MAX
};

class Servo {
private:
  uint8_t port;
  float currPos;
  int minPos;
  int maxPos;
  bool flipped;

public:
  Servo(uint8_t port, float currPos, int minPos, int maxPos, bool flipped) {
    this->port = port;
    this->currPos = currPos;
    this->minPos = minPos;
    this->maxPos = maxPos;
    this->flipped = flipped;
  }

  void resetToMaxPos() {
    this->currPos = maxPos;
  }

  void resetToMinPos() {
    this->currPos = minPos;
  }
  
  float getCurrPos() {
    if (flipped)
      return maxPos - currPos + minPos;
    else
      return currPos;
  }

  uint8_t getPort() {
    return port;
  }

  bool isCurrPosWithinValidRange() {
    return isPosWithinValidRange(currPos);
  }
  
  bool isPosWithinValidRange(float pos) {
    return pos >= minPos && pos <= maxPos;
  }

  bool isAtMinPos() {
    return currPos <= minPos;
  }

  bool isAtMaxPos() {
    return currPos >= maxPos;
  }

  bool moveOneTick(ServoRotateDir rotateDir) {
    #if LOG_EACH_MOVE
      Serial.print("moveOneTick ");
      Serial.print(currPos);
      Serial.print(rotateDir == TO_MAX ? " TO_MAX" : " TO_MIN");
      Serial.print(isAtMaxPos() ? " isAtMaxPos() == true" : " isAtMaxPos() == false");
      Serial.println(isAtMinPos() ? " isAtMinPos() == true" : " isAtMinPos() == false");
    #endif
    
    if (rotateDir == TO_MAX ? isAtMaxPos() : isAtMinPos()) {
      return false;  // already at the desired end
    }
    float newPos = currPos + (rotateDir == TO_MAX ? tick : -tick);
    if (isPosWithinValidRange(newPos)) {
      currPos = newPos;
    }
    else {
      // pull currPos to the very end, in case it is off by a fraction of a tick
      currPos = (rotateDir == TO_MAX ? maxPos : minPos);
    }
    return true;  // was able to move further
  }

  void rotateServoToCurrPos() {
    pwm.setPWM(getPort(), 0, round(getCurrPos()));
  }

  static void initPwm() {
    Serial.println("Initialising PWM...");
    pwm.begin();
    pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  }
};
