#include <EnableInterrupt.h>

#define RAIN_PIN 0
#define LED_PIN 1

volatile boolean f_rain_intr = 0;

int counter = 0;

void count_pulses(void){
  f_rain_intr = 1;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RAIN_PIN, INPUT);

  enableInterrupt(RAIN_PIN, count_pulses, FALLING);

}

void loop() {

	if (f_rain_intr == 1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
	  //   f_wdt=0;       // reset flag
		delay(300);	// soft debounce

	    f_rain_intr = 0;

	    counter = counter + 1;
	    digitalWrite(LED_PIN, HIGH);    
	    delay(300);  // Switch the LED on for 0.5 Seconds
	    digitalWrite(LED_PIN, LOW);
	    // _delay_ms(500);  // Switch the LED on for 0.5 Seconds
    
	}

}
