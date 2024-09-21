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

#pragma once

/*
  * Rotary Dial Center Press
  *
  * Performs action based on a short or long press of this button.
  *  Short: Action
  *  Long: Navigation
  */
void checkRotaryPress() {
  // Reset on each loop as we need to detect for change.
  CENTER_STATE = NO_ACTION;

  // Determine whether the rotary dial (center button) got a short or long press.
  if(encoder_center.isPressed()) {
    // Start all timers when the rotary dial is pressed.
    ms_center_double_tap.start(i_center_double_tap_delay);
    ms_center_long_press.start(i_center_long_press_delay);
    b_center_pressed = true; // Denote the dial center button was pressed.

    // Track what state the right toggle was in at the start of the center press.
    b_right_toggle_center_start = b_right_toggle_on;
  }

  if(b_center_pressed) {
    if(encoder_center.isReleased() && encoder_center.getCount() >= 1) {
      // If released and we already counted 1 press, this is a "double tap".
      CENTER_STATE = DOUBLE_PRESS;
      b_center_pressed = false;
      encoder_center.resetCount();
      ms_center_double_tap.stop();
    }
    else if(ms_center_double_tap.remaining() < 1 && encoder_center.getCount() == 1) {
      // If the double-tap counter ran out with only a single press, this was a "short" press.
      CENTER_STATE = SHORT_PRESS;
      b_center_pressed = false;
      encoder_center.resetCount();
      ms_center_double_tap.stop();
      ms_center_long_press.stop();
    }
    else if(ms_center_long_press.remaining() < 1) {
      if(b_right_toggle_center_start != b_right_toggle_on) {
        // A state change occurred for the right toggle, which we interpret as a lock-out toggle.
        b_center_lockout = !b_center_lockout;
        CENTER_STATE = NO_ACTION; // Don't count this as a long press.
        b_center_pressed = false;
        encoder_center.resetCount();
        return; // We're done here as we've performed the state change.
      }
      else {
        // Consider a long-press event if the timer is run out before released.
        CENTER_STATE = LONG_PRESS;
        b_center_pressed = false;
        encoder_center.resetCount();
      }
    }
  }

  switch(CENTER_STATE) {
    case SHORT_PRESS:
      // Perform action for short press based on current menu level.
      switch(MENU_LEVEL) {
        case MENU_1:
          // A short, single press should start or stop the music.
          attenuatorSerialSend(A_MUSIC_START_STOP);
        break;

        case MENU_2:
          // A short, single press should advance to the next track.
          attenuatorSerialSend(A_MUSIC_NEXT_TRACK);
        break;
      }
    break;

    case DOUBLE_PRESS:
      // Perform action for double tap based on current menu level.
      switch(MENU_LEVEL) {
        case MENU_1:
          // A double press should mute the pack and wand.
          attenuatorSerialSend(A_TOGGLE_MUTE);
        break;

        case MENU_2:
          // A double press should move back to the previous track.
          attenuatorSerialSend(A_MUSIC_PREV_TRACK);
        break;
      }
    break;

    case LONG_PRESS:
      // Toggle between the menu levels on a long press.
      // Also provides audio cues as to which menu is in use.
      switch(MENU_LEVEL) {
        case MENU_1:
          MENU_LEVEL = MENU_2; // Change menu level.
        break;
        case MENU_2:
          MENU_LEVEL = MENU_1; // Change menu level.
        break;
      }
    break;

    default:
      // eg. NO_ACTION - No-op
    break;
  }
}

/*
 * Determines if encoder was turned CW or CCW.
 */
void readEncoder() {
  if(digitalRead(r_encoderA) == digitalRead(r_encoderB)) {
    i_encoder_pos++; // Clockwise
  }
  else {
    i_encoder_pos--; // Counter-clockwise
  }

  i_val_rotary = i_encoder_pos / 2.5;
}

/*
 * Rotary Dial Rotation
 *
 * Performs action based turning the dial.
 */
void checkRotaryEncoder() {
  // Take action if rotary encoder value was turned CW.
  if(i_val_rotary > i_last_val_rotary) {
    if(!ms_rotary_debounce.isRunning()) {
      if(b_firing && i_speed_multiplier > 2) {
        // Tell the pack to cancel the current overheat warning.
        // Only do so after 5 turns of the dial (CW).
        i_rotary_count++;
        if(i_rotary_count % 5 == 0) {
          attenuatorSerialSend(A_WARNING_CANCELLED);
          i_rotary_count = 0;
        }
      }
      else {
        // Perform action based on the current menu level.
        switch(MENU_LEVEL) {
          case MENU_1:
            // Tell pack to increase overall volume.
            attenuatorSerialSend(A_VOLUME_INCREASE);
          break;

          case MENU_2:
            // Tell pack to increase effects volume.
            attenuatorSerialSend(A_VOLUME_SOUND_EFFECTS_INCREASE);
          break;
        }
      }

      ms_rotary_debounce.start(rotary_debounce_time);
    }
  }

  // Take action if rotary encoder value was turned CCW.
  if(i_val_rotary < i_last_val_rotary) {
    if(!ms_rotary_debounce.isRunning()) {
      if(b_firing && i_speed_multiplier > 2) {
        // Tell the pack to cancel the current overheat warning.
        // Only do so after 5 turns of the dial (CCW).
        i_rotary_count++;
        if(i_rotary_count % 5 == 0) {
          attenuatorSerialSend(A_WARNING_CANCELLED);
          i_rotary_count = 0;
        }
      }
      else {
        // Perform action based on the current menu level.
        switch(MENU_LEVEL) {
          case MENU_1:
            // Tell pack to decrease overall volume.
            attenuatorSerialSend(A_VOLUME_DECREASE);
          break;

          case MENU_2:
            // Tell pack to decrease effects volume.
            attenuatorSerialSend(A_VOLUME_SOUND_EFFECTS_DECREASE);
          break;
        }
      }

      ms_rotary_debounce.start(rotary_debounce_time);
    }
  }

  // Remember the last rotary value for comparison later.
  i_last_val_rotary = i_val_rotary;

  if(ms_rotary_debounce.justFinished()) {
    ms_rotary_debounce.stop();
  }
}

/*
 * Perform debounce and get current button/switch states.
 *
 * Required by the ezButton objects.
 */
void switchLoops() {
  switch_left.loop();
  switch_right.loop();
  encoder_center.loop();
}

/*
 * Monitor for interactions by user or serial comms.
 */
void mainLoop() {
  checkPack();
  switchLoops();
  checkRotaryPress();
  if(!b_center_lockout) {
    checkRotaryEncoder();
  }

  if(!b_wait_for_pack) {
    // Blink ourselves an LED to show we're fully synchronized and still functioning.
    if(ms_blink_leds.justFinished()) {
      digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) == HIGH ? LOW : HIGH);
      ms_blink_leds.repeat();
    }
  }

  /*
   * Left Toggle - Uses a pull-up resistor, so setting LOW indicates ON.
   *
   * Paired:
   * When paired with the gpstar Proton Pack controller, will turn the
   * pack on or off. When the pack is on the bargraph will automatically
   * enable and display an animation which matches the Neutrona Wand
   * bargraph (whether stock 5-LED version or 28-segment by Frutto).
   *
   * Standalone:
   * When not paired with the gpstar Proton Pack controller, will turn
   * on the bargraph which will display some pre-set pattern.
   */

  // Turns the pack on or off (when paired) via left toggle.
  if(switch_left.isPressed() || switch_left.isReleased()) {
    if(switch_left.getState() == LOW) {
      attenuatorSerialSend(A_TURN_PACK_ON);
      b_pack_on = true;
      b_left_toggle_on = true;
    }
    else {
      attenuatorSerialSend(A_TURN_PACK_OFF);
      b_pack_on = false;
      b_left_toggle_on = false;
    }
  }

  // Turn on the bargraph when certain conditions are met.
  // This supports pack connection or standalone operation.
  if(b_pack_on) {
    if(BARGRAPH_STATE == BG_OFF && !(b_overheating || b_pack_alarm)) {
      bargraphReset(); // Enable bargraph for use (resets variables and turns it on).
      BARGRAPH_PATTERN = BG_POWER_RAMP; // Bargraph idling loop.
    }
  }
  else {
    if(switch_left.getState() == HIGH) {
      bargraphOff(); // Clear all bargraph elements and turn off the device.
    }
  }

  // Update bargraph elements, leveraging cyclotron speed modifier.
  // In reality this multiplier is a divisor to the standard delay.
  bargraphUpdate(i_speed_multiplier);

  if(ms_packsync.justFinished()) {
    // The pack just went missing, so treat as disconnected.
    b_wait_for_pack = true;
    ms_packsync.start(i_sync_initial_delay);
  }
}