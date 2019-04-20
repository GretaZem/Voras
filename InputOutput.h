const int distanceTriggerPin = 9;
const int distanceEchoPin = 10;
const int rightLedPin = 12;
const int leftLedPin = 13;
const int strategyButtonPin = 2;

// set the next line to false to use hardcoded tick length (when potentiometer is not connected to the pin below)
#define SPEED_CONTROLLED_BY_POTENTIOMETER true
const int speedControlInPin = A0;  // Analog input pin that the potentiometer is attached to

int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

class InputOutput {
public:
  static void initInsAndOuts() {
    pinMode(distanceTriggerPin, OUTPUT);
    pinMode(distanceEchoPin, INPUT);
    pinMode(rightLedPin, OUTPUT);
    pinMode(leftLedPin, OUTPUT);
    pinMode(strategyButtonPin, INPUT);
  }

  static void turnLedsOnOff(bool rightLedOn, bool leftLedOn) {
    digitalWrite(rightLedPin, (rightLedOn ? HIGH : LOW));
    digitalWrite(leftLedPin, (leftLedOn ? HIGH : LOW));    
  }

  static float determineTickLengthFromPotentiometer() {
    if (SPEED_CONTROLLED_BY_POTENTIOMETER) {
      int speedControlValue = analogRead(speedControlInPin);
      tick = map(speedControlValue, 0, 1000, 10 * MIN_TICK, 10 * MAX_TICK) / 10.0;
      //Serial.print("Tick length: ");
      //Serial.println(tick);
      return tick;
    }
    // no potentiometer connected -- use a reasonable (average) hard-coded speed
    tick = (MIN_TICK + MAX_TICK) / 2.0;
    return tick;
  }

  static boolean isStrategyButtonPressed() {
    boolean result = false;
    // read the pushbutton input pin:
    buttonState = digitalRead(strategyButtonPin);
  
    // compare the buttonState to its previous state
    if (buttonState != lastButtonState) {
      // if the state has changed, increment the counter
      if (buttonState == HIGH) {
        // if the current state is HIGH then the button went from off to on:
        result = true;
        Serial.print("Button on.");
      } else {
        // if the current state is LOW then the button went from on to off:
        Serial.println("Button off.");
      }
      // Delay a little bit to avoid bouncing
      delay(50);
    }
    // save the current state as the last state, for next time through the loop
    lastButtonState = buttonState;
    return result;
  }
  
  static int getDistanceToObstacle(int veryLongDistance) {
    digitalWrite(distanceTriggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(distanceTriggerPin, HIGH);
    delayMicroseconds(1176); //20cm
    digitalWrite(distanceTriggerPin, LOW);
    long duration = pulseIn(distanceEchoPin, HIGH, 10000); // waits for the echo, returns the sound wave travel time in microseconds
    int distance = duration*0.034/2;
    Serial.print("Distance: ");
    Serial.println(distance);
    if (distance == 0) {
      // we receive 0 when the reply does not come back within the timeout
      // return a long number in that case
      return veryLongDistance;
    }
    return distance;
  }
};

