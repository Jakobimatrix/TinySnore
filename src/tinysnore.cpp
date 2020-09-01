/*
tinysnore.cpp - Library for putting ATtiny*5s to sleep!
Created by Connor Nishijima, May 12th 2017.
Released under the GPLv3 license.
*/

#include "tinysnore.h"

#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void snore(uint32_t snore_time){
  const int NUM_WD_CONFIGURATIONS = 10;
  const int WD_TIMEOUT_MS[NUM_WD_CONFIGURATIONS] = {16, 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000};

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_bod_disable();

  int timeout_pointer = NUM_WD_CONFIGURATIONS - 1;
  while (snore_time > WD_TIMEOUT_MS[0]) {
    if ( snore_time < WD_TIMEOUT_MS[timeout_pointer]){
      timeout_pointer--;
      continue;
    }
    ts_set_sleep(timeout_pointer);
    ts_system_sleep();
    snore_time -= WD_TIMEOUT_MS[timeout_pointer];
  }
  if(snore_time > 0){
    delay(snore_time);
  }
}

void ts_system_sleep(){
  sleep_mode();                        // System sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
}

void ts_set_sleep(int ii){
  byte bb;
  int ww;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  ww = bb;

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  // NOTHING HERE BY DEFAULT
}
