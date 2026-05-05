/**
 *   GPStar Proton Stream Target Trainer
 *   Copyright (C) 2023-2026 GPStar Technologies <contact@gpstartechnologies.com>
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

#pragma once

/*
 * Hardware Pins
 */
#define PSTT_SERVO_PIN 47
#define PSTT_STATUS_LED_PIN 38
#define PSTT_BUTTON 11
#define PSTT_JEWEL_LED_PIN 41 // Data pin for the addressable LEDs.

/*
 * Servo Motor
 */
Servo pstt_servo;
millisDelay ms_servo_timer;

/*
 * Reset Button
 */
ezButton switch_pstt(PSTT_BUTTON); // Button for the PSTT. Used to manually change the servo position.

/*
 * Jewel indicator LEDs.
 */
#define JEWEL_LED_MAX 14 // The maximum number of indicator LEDs.

/*
 * Adafruit LED declaration.
 */
int8_t led_pins[8] = { PSTT_JEWEL_LED_PIN, -1, -1, -1, -1, -1, -1, -1 };
Adafruit_NeoPXL8 pstt_led_output((JEWEL_LED_MAX + 2), led_pins);

struct PSTTLeds {
  uint8_t index;

  PSTTLeds& operator[](uint8_t i) {
    index = i;
    return *this;
  }

  PSTTLeds& operator=(uint32_t a) {
    pstt_led_output.setPixelColor(index, a);
    return *this;
  }

  explicit operator bool() { return pstt_led_output.getPixelColor(index); }
};

PSTTLeds pstt_jewel_leds;

enum PSTT_TARGET_STATUS : uint8_t {
  PSTT_READY = 1,
  PSTT_DISABLED = 2,
  PSTT_CALIBRATE = 3
} PSTT_STATUS = PSTT_DISABLED;

/*
 * Target Health
 */
#define PSTT_MAX_HEALTH 1000
#define PSTT_LOW_HEALTH 440
#define PSTT_EXTREME_LOW_HEALTH 220
#define PSTT_MIN_HEALTH 0
uint16_t pstt_current_health = PSTT_MAX_HEALTH;

/*
 * Health Regeneration Timer
 */
millisDelay ms_pstt_health_regen;
const uint16_t i_pstt_health_regen_delay = 1000;

/*
 * Health Regeneration Amount
 */
#define PSTT_HEALTH_REGEN 100

/*
 * Target Blink Rate
 */
millisDelay ms_pstt_blink_rate;
#define PSTT_BLINK_RATE_DEFAULT_DELAY 400
uint16_t i_pstt_blink_rate_delay = PSTT_BLINK_RATE_DEFAULT_DELAY;
bool b_colourChange = true;

/*
 * Damage Values - Neutrona Wand by Power Level
 */
#define NEUTRONA_WAND_PWR_1 10
#define NEUTRONA_WAND_PWR_2 14
#define NEUTRONA_WAND_PWR_3 20
#define NEUTRONA_WAND_PWR_4 25
#define NEUTRONA_WAND_PWR_5 33

/*
 * Data structure object for target customizations which are saved into NVS.
 * WARNING: Do not reorder fields without changing field names or types, as this
 * would cause size validation to pass but load data into incorrect fields.
 */
struct __attribute__((packed)) UserTargetConfig {
  // Target Health Settings
  uint16_t maxHealth = PSTT_MAX_HEALTH;
  uint16_t lowHealth = PSTT_LOW_HEALTH;
  uint16_t extremeLowHealth = PSTT_EXTREME_LOW_HEALTH;

  // Health Regeneration Settings
  uint16_t healthRegen = PSTT_HEALTH_REGEN;
  uint16_t healthRegenDelay = i_pstt_health_regen_delay;

  // Damage Values - Neutrona Wand by Power Level
  uint16_t wandPower1 = NEUTRONA_WAND_PWR_1;
  uint16_t wandPower2 = NEUTRONA_WAND_PWR_2;
  uint16_t wandPower3 = NEUTRONA_WAND_PWR_3;
  uint16_t wandPower4 = NEUTRONA_WAND_PWR_4;
  uint16_t wandPower5 = NEUTRONA_WAND_PWR_5;
} targetConfig;
