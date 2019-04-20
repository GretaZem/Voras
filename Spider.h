#include "Leg.h"
#include "MoveStrategy.h"
#define LEGS_IN_USE 8

Leg leg1(1, 0, 1, SIDE_L, H_SERVO_MIN+140, H_SERVO_MAX+140, false, V_SERVO_MIN+5, V_SERVO_MAX+5, true, LIFT);
Leg leg2(2, 2, 3, SIDE_R, H_SERVO_MIN+40, H_SERVO_MAX+40, false, V_SERVO_MIN-320, V_SERVO_MAX-320, false, LOWER);
Leg leg3(3, 4, 5, SIDE_L, H_SERVO_MIN+20, H_SERVO_MAX+20, false, V_SERVO_MIN, V_SERVO_MAX, true, LOWER);
Leg leg4(4, 6, 7, SIDE_R, H_SERVO_MIN+130, H_SERVO_MAX+130, false, V_SERVO_MIN-310, V_SERVO_MAX-310, false, LIFT);
Leg leg5(5, 8, 9, SIDE_L, H_SERVO_MIN-60, H_SERVO_MAX-60, false, V_SERVO_MIN-90, V_SERVO_MAX-90, true, LIFT);
Leg leg6(6, 10, 11, SIDE_R, H_SERVO_MIN+90, H_SERVO_MAX+90, false, V_SERVO_MIN-210, V_SERVO_MAX-210, false, LOWER);
Leg leg7(7, 12, 13, SIDE_L, H_SERVO_MIN-40, H_SERVO_MAX-40, false, V_SERVO_MIN-90, V_SERVO_MAX-90, true, LOWER);
Leg leg8(8, 14, 15, SIDE_R, H_SERVO_MIN+125, H_SERVO_MAX+125, false, V_SERVO_MIN-240, V_SERVO_MAX-240, false, LIFT);

Leg* legs[] = {&leg1, &leg2, &leg3, &leg4, &leg5, &leg6, &leg7, &leg8};

class Spider {
public:
  Spider() {
    currStrategy->init(FWD);
    Serial.print(LEGS_IN_USE);
    Serial.println(" legs initialised");
  }

  void moveOneTick() {
    for (int i = 0; i < LEGS_IN_USE; i++) {
      legs[i]->moveOneTick();
    }

    if (switchToNextStrategy && allLegsAtLowestPos()) {
      doSwitchToNextStrategy();
      initAllLegs();  // return the state of all legs to how they were upon initial creation
    }
            
    for (int i = 0; i < LEGS_IN_USE; i++) {  
      if (legs[i]->isPhaseComplete()) {
        LegPhase nextPhase = legs[i]->determineNextPhase(currStrategy->getMoveDir());
        if (nextPhase == LIFT) {
          // for better stability and to avoid bobbing, only start lifting (some) legs after all legs have been lowered
          if (allLegsAtLowestPos()) {
            legs[i]->setCurrPhase(nextPhase);
          }
        }
        else {
          legs[i]->setCurrPhase(nextPhase);
        }
      }
    }

    currStrategy->movedOneTick(allLegsAtLowestPos());
  }

  bool allLegsAtLowestPos() {
    for (int i = 0; i < LEGS_IN_USE; i++) {
      if (!legs[i]->isAtLowestPos()) {
        return false;
      }
    }
    return true;
  }

  bool allLegsPhaseComplete() {
    for (int i = 0; i < LEGS_IN_USE; i++) {
      if (!legs[i]->isPhaseComplete()) {
        return false;
      }
    }
    return true;
  }

  MoveDir getMoveDir() {
    return currStrategy->getMoveDir();
  }

  void switchToNextStrategyWhenPossible() {
    switchToNextStrategy = true;
    Serial.println("switchToNextStrategyWhenPossible()");
  }

private:
  DemoMoveStrategy demoMoveStrategy;
  DefaultMoveStrategy defaultMoveStrategy;
  MoveStrategy *currStrategy = &demoMoveStrategy;
  bool switchToNextStrategy = false;

  void doSwitchToNextStrategy() {
    if (currStrategy == &demoMoveStrategy) {
      currStrategy = &defaultMoveStrategy;
      Serial.println("Switched to defaultMoveStrategy");
    }
    else {
      currStrategy = &demoMoveStrategy;
      Serial.println("Switched to demoMoveStrategy");
    }
    currStrategy->init(FWD);
    switchToNextStrategy = false;
  }

  void initAllLegs() {
    for (int i = 0; i < LEGS_IN_USE; i++) {
      legs[i]->init();
    }
  }
};
