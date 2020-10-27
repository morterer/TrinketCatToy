#include <Adafruit_SoftServo.h>

#define MIN_DELAY 10    // minimum servo speed
#define MAX_DELAY 50    // maximum servo speed


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
      if ((currentMillis - lastUpdate) > updateInterval) {  // time to update
        lastUpdate = millis();
        if(posCurrent > posTarget){                         // compute the new position
          posCurrent --;
        } else {
          posCurrent ++;
        }
        servo.write(posCurrent);                            // move servo to new position
        if(posCurrent == posTarget){                        // if the servo is at the target position
          updateInterval = random(MIN_DELAY,MAX_DELAY);     // randomly choose new speed
          posTarget = random(posMin,posMax);                // randomly choose new position
        }
      }      
    }
};

Sweeper sweeper1(0,180);          // how far servo can move left-right
Sweeper sweeper2(0,40);           // how far servo can move up-down

volatile uint8_t counter = 0;     // used for counting in the interrupt
unsigned long currentMillis = 0;  // used to manage servo timing

void setup() {
  // Taken from Adafruit_SoftServo sample code available on Github
  // Set up the interrupt that will refresh the servo for us automagically
  OCR0A = 0xAF;             // any number is OK
  // interrupt for Trinket
  TIMSK |= _BV(OCIE0A);     // Turn on the compare interrupt (below!)
  
  sweeper1.attach(1);       // attach servo to pin 1
  sweeper2.attach(4);       // attach servo to pin 4
}

void loop() {
  currentMillis = millis();
  sweeper1.update(currentMillis);
  sweeper2.update(currentMillis);
}

// Taken from Adafruit_SoftServo sample code available on Github
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
