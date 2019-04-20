#include "Servo.h"
//#define LOG_MOVE_TICKS

enum MoveDir {
  FWD,
  BWD,
  ROTATE_LEFT,
  ROTATE_RIGHT,
};

enum LegPhase {
  UNKNOWN,
  STAY,
  LIFT,
  FORWARD,
  LOWER,
  BACKWARD,
  COMPLETE
};

enum Side {
  SIDE_L,
  SIDE_R
};

const char* getMoveDirStr(MoveDir moveDir) {
  switch (moveDir) {
    case FWD:
      return "Forward";
    case BWD:
      return "Backward";
    case ROTATE_LEFT:
      return "Rotate left";
    case ROTATE_RIGHT:
      return "Rotate right";
  }
  return "Unknown";
}

const char* getPhaseStr(LegPhase phase) {
  switch (phase) {
    case STAY:
      return "Stay";
    case LIFT:
      return "Lift";
    case FORWARD:
      return "Forward";
    case LOWER:
      return "Lower";
    case BACKWARD:
      return "Backward";
    case COMPLETE:
      return "Complete";
  }
  return "Unknown";
}

class Leg {
private:
  uint8_t legNum;
  Side side;
  Servo hServo;
  Servo vServo;
  LegPhase initPhase;  // initial phase for this leg
  LegPhase phase;
  LegPhase lastActionPhase;  // only action phase like LIFT/FORWARD, not a COMPLETE phase

public:
  Leg(uint8_t legNum,
      uint8_t hServoPort,
      uint8_t vServoPort,
      Side side,
      int hServoMinPos,
      int hServoMaxPos,
      bool hServoFlipped,
      int vServoMinPos,
      int vServoMaxPos,
      bool vServoFlipped,
      LegPhase phase) 
    : hServo(hServoPort, (phase == LOWER ? hServoMaxPos : hServoMinPos), hServoMinPos, hServoMaxPos, hServoFlipped), 
      vServo(vServoPort, (phase == LOWER ? vServoMaxPos : vServoMinPos), vServoMinPos, vServoMaxPos, vServoFlipped) 
  {
    this->legNum = legNum;
    this->side = side;
    this->initPhase = phase;
    init();
  }

  void init() {
    this->setCurrPhase(initPhase);
    if (initPhase == LOWER) {
      hServo.resetToMaxPos();
      vServo.resetToMaxPos();
    }
    else {
      hServo.resetToMinPos();
      vServo.resetToMinPos();
    }
  }

  void moveOneTick() {
    bool moved = false;
    switch (phase) {
      case LIFT:
        moved = vServo.moveOneTick(TO_MAX);
        break;
      case FORWARD:
        moved = hServo.moveOneTick(side == SIDE_L ? TO_MAX : TO_MIN);
        break;
      case LOWER:
        moved = vServo.moveOneTick(TO_MIN);
        break;
      case BACKWARD:
        moved = hServo.moveOneTick(side == SIDE_L ? TO_MIN : TO_MAX);
        break;
    }
    if (!moved) {
      phase = COMPLETE;
    }
    hServo.rotateServoToCurrPos();
    vServo.rotateServoToCurrPos();
    
/*
      Serial.print("Phase: ");
      Serial.println(getPhaseStr(phase));
      Serial.print(hServo.getPort());
      Serial.print(" motor pos ");
      Serial.println(hServo.getCurrPos());
      Serial.print(vServo.getPort());
      Serial.print(" motor pos ");
      Serial.println(vServo.getCurrPos());
      Serial.println("");
      Serial.println(moved ? "moved" : "did not move");
*/
  }

  bool isPhaseComplete() {
    return phase == COMPLETE;
  }

  LegPhase getPhase() {
    return phase;
  }

  LegPhase determineNextPhase(MoveDir moveDir) {
    LegPhase nextPhase = UNKNOWN;
    if (moveDir == FWD) {
      switch (lastActionPhase) {
        case LIFT:
          nextPhase = FORWARD;
          break;
        case FORWARD:
          nextPhase = LOWER;
          break;
        case LOWER:
          nextPhase = BACKWARD;
          break;
        case BACKWARD:
          nextPhase = LIFT;
          break;
      }
    }
    else if (moveDir == BWD) {
      switch (lastActionPhase) {
        case LIFT:
          nextPhase = BACKWARD;
          break;
        case FORWARD:
          nextPhase = LIFT;
          break;
        case LOWER:
          nextPhase = FORWARD;
          break;
        case BACKWARD:
          nextPhase = LOWER;
          break;
      }
    }
    else if (moveDir == ROTATE_RIGHT) {
      switch (lastActionPhase) {
        case LIFT:
          nextPhase = (side == SIDE_L ? FORWARD : BACKWARD);
          break;
        case FORWARD:
          nextPhase = (side == SIDE_L ? LOWER : LIFT);
          break;
        case LOWER:
          nextPhase = (side == SIDE_L ? BACKWARD : FORWARD);
          break;
        case BACKWARD:
          nextPhase = (side == SIDE_L ? LIFT : LOWER);
          break;
      }
    }
    else if (moveDir == ROTATE_LEFT) {
      switch (lastActionPhase) {
        case LIFT:
          nextPhase = (side == SIDE_L ? BACKWARD : FORWARD);
          break;
        case FORWARD:
          nextPhase = (side == SIDE_L ? LIFT : LOWER);
          break;
        case LOWER:
          nextPhase = (side == SIDE_L ? FORWARD : BACKWARD);
          break;
        case BACKWARD:
          nextPhase = (side == SIDE_L ? LOWER : LIFT);
          break;
      }
    }
    #ifdef LOG_MOVE_TICKS
      Serial.print("MoveDir: ");
      Serial.print(getMoveDirStr(moveDir));
      Serial.print(", leg #");
      Serial.print(legNum);
      Serial.print(" ");
      Serial.print(getPhaseStr(lastActionPhase));
      Serial.print(" >> ");
      Serial.print(getPhaseStr(phase));
      Serial.print(", vPos: ");
      Serial.print(vServo.getCurrPos());
      Serial.print(", hPos: ");
      Serial.println(hServo.getCurrPos());  
    #endif
    
    return nextPhase;
  }

  void setCurrPhase(LegPhase nextPhase) {
    phase = nextPhase;
    if (phase != COMPLETE) {
      lastActionPhase = phase;
    }
  }

  bool isAtLowestPos() {
    return vServo.isAtMinPos();
  }
};
