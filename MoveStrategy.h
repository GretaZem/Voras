#include "InputOutput.h"

class MoveStrategy {
public:
  virtual void init(MoveDir moveDir) {
    this->moveDir = moveDir;
  }
  virtual MoveDir pickNextMoveDir(MoveDir currMoveDir) = 0;
  MoveDir getMoveDir() {
    return moveDir;
  }

  virtual void movedOneTick(bool allLegsAtLowestPos) {
    if (allLegsAtLowestPos) {
      if (--remainCyclesInDir <= 0) {
        MoveDir prevMoveDir = moveDir;
        moveDir = pickNextMoveDir(moveDir);
        if (moveDir != prevMoveDir) {
          Serial.print("Changed moveDir to ");
          Serial.println(getMoveDirStr(moveDir));
        }
      }
      else {
        Serial.print("remainCyclesInDir == ");
        Serial.println(remainCyclesInDir);
      }
    }
  }
  
protected:
  MoveDir moveDir;
  int remainCyclesInDir = 1;
};

class DemoMoveStrategy : public MoveStrategy {
public:
  virtual void init(MoveDir moveDir) {
    MoveStrategy::init(moveDir);
    remainCyclesInDir = 10;
  }
    
  virtual MoveDir pickNextMoveDir(MoveDir currMoveDir) {
    remainCyclesInDir = 10;
    switch (currMoveDir) {
      case FWD:
        return BWD;
      case BWD:
        return ROTATE_LEFT;
      case ROTATE_LEFT:
        return ROTATE_RIGHT;
      case ROTATE_RIGHT:
        return FWD;
    }
    return currMoveDir;
  }
};

class DefaultMoveStrategy : public MoveStrategy {
public:
  virtual void init(MoveDir moveDir) {
    MoveStrategy::init(moveDir);
    remainCyclesInDir = 1;
  }

  virtual MoveDir pickNextMoveDir(MoveDir currMoveDir) {
    if (InputOutput::getDistanceToObstacle(1000) <= maxDistanceToObstacle) {
      retreat = true;
    }
    if (retreat) {
      switch (currMoveDir) {
        case FWD:
          remainCyclesInDir = 5;
          return BWD;
        case BWD:
          remainCyclesInDir = 10;
          retreat = false;
          return ROTATE_LEFT;
      }
    }
    remainCyclesInDir = 1;
    return FWD;
  }
  
private:
  int maxDistanceToObstacle = 20;
  bool retreat = false;
};

