#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <Servo.h>

#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <MPR121_defs.h>
#include <Wire.h>

// num of electrodes on the mpr121
#define numElectrodes 12

// pin for mpr121
#define MPR_PIN 3

// data in for neopixel ring
#define LED_PIN    6

// number of leds on ring
#define LED_COUNT 16

// declare the neopixel object
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

bool ledTouch = false;

// pin for servo
#define FIRST_SERVO_PIN 10
#define SECOND_SERVO_PIN 11

// define servo library instances
Servo firstServo;
Servo secondServo;

void setup() {
  Serial.begin(9600);

  Wire.begin();

  // setup the MPR121
  MPR121.begin(0x5A);
  //  MPR121.setInterruptPin(4);
  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);
  MPR121.updateTouchData();

  // initializing neopixel object
  // source: strandtest example in adafruit neopixel library
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)

  // telling the library where we have a servos attached
  firstServo.attach(FIRST_SERVO_PIN);
  secondServo.attach(SECOND_SERVO_PIN);
  //  // debugging servo
  //  myServo.write(0);
  //  delay(1000);
  //  myServo.write(180);
  //  delay(1000);
}

// source: strandtest example in adafruit neopixel library
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void loop() {
  // check for new MPR121 data
  // source: https://www.youtube.com/watch?v=tTMsbL0eH_M&ab_channel=talofer99
  if (MPR121.touchStatusChanged()) {
    MPR121.updateTouchData();

    // looping through to check each electrode
    for (int i = 0; i < numElectrodes; i++) {
      // if touching is detected
      if (MPR121.isNewTouch(i)) {

        // LED: if electrode 1 is touched
        if (i == 1) {
          ledTouch = true;
          Serial.println("LED touch!");
          if (ledTouch == true) {
            rainbow(0.5);
          }
        }


        // SERVO: if electrode 11 is touched
        if (i == 11) {
          Serial.println("SERVO touch!");
          // running between 0 and 179 degrees
          for (int j = 0; j < 180; j++) {
            // write angle to the servo
            firstServo.write(j);
            secondServo.write(j);
            delay(5);
          }
        }

        // if a release is detected
      } else if (MPR121.isNewRelease(i)) {

        // LED: if electrode 1 is released
        if (i == 1) {
          strip.clear();
          ledTouch = false;
          if (ledTouch == false) {
            Serial.println("LED release!");
            // turn the leds off
            strip.clear();
          }
        }
        // SERVO: if electrode 11 is released
        if (i == 11) {
          Serial.println("SERVO release!");
          // go back to 0 degrees
          for (int j = 180; j > 0; j--) {
            firstServo.write(j);
            secondServo.write(j);
            delay(5);
          }
        }
      }
    }
  }
}
