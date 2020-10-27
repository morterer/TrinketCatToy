#include <Adafruit_SoftServo.h>

#define MIN_DELAY 10
#define MAX_DELAY 50


class Sweeper {
    Adafruit_SoftServo servo; // the servo
    int posCurrent;           // current servo position
    int posTarget;            // target servo position
    int posMin;               // the lowest servo position
    int posMax;               // the highest servo position
    int  updateInterval;      // interval between updates
    unsigned long lastUpdate; // last update of position

  public:
    Sweeper(int min, int max) {
      updateInterval = random(MIN_DELAY,MAX_DELAY);
      posMin = min;
      posMax = max;

      posCurrent = min;
      posTarget = random(min,max);
    }

    void attach(int pin) {
      servo.attach(pin);
    }

    void detach() {
      servo.detach();
    }

    void refresh() {
      servo.refresh();
    }

    void update(unsigned long currentMillis) {
      if ((currentMillis - lastUpdate) > updateInterval) { // time to update
        lastUpdate = millis();
        if(posCurrent > posTarget){
          posCurrent --;
        } else {
          posCurrent ++;
        }
        servo.write(posCurrent);
        if(posCurrent == posTarget){ // if the servo is at the target position
          updateInterval = random(MIN_DELAY,MAX_DELAY);
          posTarget = random(posMin,posMax);
        }
      }      
    }
};


Sweeper sweeper1(0,180);  // how far to sweep left-right
Sweeper sweeper2(0,40);   // how far to sweep up-down

// used for counting in the interrupt
volatile uint8_t counter = 0;
unsigned long currentMillis = 0;

void setup() {
  // Set up the interrupt that will refresh the servo for us automagically
  OCR0A = 0xAF;            // any number is OK
// interrupt for Trinket
  TIMSK |= _BV(OCIE0A);    // Turn on the compare interrupt (below!)
// interrupt for Arduino Uno
//  TIMSK0 |= _BV(OCIE0A);    // Turn on the compare interrupt (below!)
  
  sweeper1.attach(1);
  sweeper2.attach(4);
}

void loop() {
  currentMillis = millis();
  sweeper1.update(currentMillis);
  sweeper2.update(currentMillis);
}

// We'll take advantage of the built in millis() timer that goes off
// to keep track of time, and refresh the servo every 20 milliseconds
SIGNAL(TIMER0_COMPA_vect) {
  // this gets called every 2 milliseconds
  counter += 2;
  // every 20 milliseconds, refresh the servos!
  if (counter >= 20) {
    counter = 0;
    sweeper1.refresh();
    sweeper2.refresh();
  }
}
