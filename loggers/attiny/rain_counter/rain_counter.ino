#include <EnableInterrupt.h>
#include <avr/sleep.h>

// Routines to set and claer bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// all pin numbers are arduino pin defn
#define RAIN_PIN 0
#define LED_PIN 1
#define REPORT_PIN 2

// delays
#define DEBOUNCE_MS	200
#define LED_EVENT_MS 200
#define LED_REPORT_HIGH_MS 100
#define LED_REPORT_LOW_MS 200

volatile boolean f_rain_intr = 0;
volatile boolean f_report = 1;

// Variables for the Sleep/power down modes:
volatile boolean f_wdt = 1;

int counter = 0;

// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
  
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
 
  sleep_mode();                        // System actually sleeps here
 
  sleep_disable();                     // System continues execution here when watchdog timed out 
  
}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
 
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
 
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}

void count_pulses(void){
  f_rain_intr = 1;
}

void report_pulses(void){
	f_report = 1;
}

void setup() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  pinMode(LED_PIN, OUTPUT);
  pinMode(RAIN_PIN, INPUT_PULLUP);
  pinMode(REPORT_PIN, INPUT_PULLUP);

  enableInterrupt(RAIN_PIN, count_pulses, FALLING);
  enableInterrupt(REPORT_PIN, report_pulses, FALLING);

}

void loop() {

	int i = 0;

	if (f_rain_intr == 1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
	  //   f_wdt=0;       // reset flag
		delay(DEBOUNCE_MS);	// soft debounce

	    f_rain_intr = 0;

	    counter = counter + 1;
	    digitalWrite(LED_PIN, HIGH);    
	    delay(LED_EVENT_MS);  // Switch the LED on for 0.5 Seconds
	    digitalWrite(LED_PIN, LOW);
	    // _delay_ms(500);  // Switch the LED on for 0.5 Seconds
    
	}
	if (f_report == 1){
		delay(DEBOUNCE_MS);
		f_report = 0;

		for(i=0; i<counter; i++){
			digitalWrite(LED_PIN, HIGH);
			delay(LED_REPORT_HIGH_MS);
			digitalWrite(LED_PIN, LOW);
			delay(LED_REPORT_LOW_MS);
		}

		counter = 0;  
	}

	setup_watchdog(60000);
	system_sleep();

}
