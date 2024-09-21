/**
 *   GPStar Attenuator - Ghostbusters Proton Pack & Neutrona Wand.
 *   Copyright (C) 2023-2024 Michael Rajotte <michael.rajotte@gpstartechnologies.com>
 *                         & Dustin Grau <dustin.grau@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <https://www.gnu.org/licenses/>.
 *
 */

// PROGMEM macro
#define PROGMEM_READU32(x) pgm_read_dword_near(&(x))
#define PROGMEM_READU16(x) pgm_read_word_near(&(x))
#define PROGMEM_READU8(x) pgm_read_byte_near(&(x))

// 3rd-Party Libraries
#include <millisDelay.h>
#include <ezButton.h>
#include <ht16k33.h>
#include <Wire.h>
#include <SerialTransfer.h>

// Local Files
#include "Configuration.h"
#include "Communication.h"
#include "Header.h"
#include "Bargraph.h"
#include "Serial.h"
#include "System.h"

void setup() {
  // Enable Serial connection(s) and communication with GPStar Proton Pack PCB.
  Serial.begin(9600);
  packComs.begin(Serial);

  // Assume the Super Hero arming mode with Afterlife (default for Haslab).
  SYSTEM_YEAR = SYSTEM_AFTERLIFE;

  // Boot into proton mode (default for pack and wand).
  STREAM_MODE = PROTON;

  if(!b_wait_for_pack) {
    // If not waiting for the pack set power level to 5.
    POWER_LEVEL = LEVEL_5;
  }
  else {
    // When waiting for the pack set power level to 1.
    POWER_LEVEL = LEVEL_1;
  }

  // Begin at menu level one. This affects the behavior of the rotary dial.
  MENU_LEVEL = MENU_1;

  // Debounce the toggle switches and encoder pushbutton.
  switch_left.setDebounceTime(switch_debounce_time);
  switch_right.setDebounceTime(switch_debounce_time);
  encoder_center.setDebounceTime(switch_debounce_time);

  // Rotary encoder on the top of the Attenuator.
  pinMode(r_encoderA, INPUT_PULLUP);
  pinMode(r_encoderB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(r_encoderA), readEncoder, CHANGE);

  // Built-in LED.
  pinMode(LED_BUILTIN, OUTPUT);

  // Setup the bargraph after a brief delay.
  delay(10);
  setupBargraph();

  // Get initial switch/button states.
  switchLoops();

  // Delay to allow any other devices to start up first.
  delay(100);

  // Initialize critical timers.
  if(b_wait_for_pack) {
    ms_packsync.start(0);
  }
}

void loop() {
  if(b_wait_for_pack) {
    if(ms_packsync.justFinished()) {
      // Tell the pack we are trying to sync.
      attenuatorSerialSend(A_SYNC_START);

      #if defined(__XTENSA__)
        // ESP - Turn off built-in LED.
        digitalWrite(BUILT_IN_LED, LOW);
      #endif

      digitalWrite(LED_BUILTIN, LOW);
      ms_blink_leds.stop();

      // Pause and try again in a moment.
      ms_packsync.start(i_sync_initial_delay);
    }

    checkPack();

    if(!b_wait_for_pack) {
      // Indicate that we are no longer waiting on the pack.
      digitalWrite(LED_BUILTIN, HIGH);
      ms_blink_leds.start(i_blink_leds);
    }
  }
  else {
    // When not waiting for the pack go directly to the main loop.
    mainLoop();
  }
}