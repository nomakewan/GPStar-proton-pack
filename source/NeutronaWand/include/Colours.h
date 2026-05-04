/**
 *   GPStar Neutrona Wand - Ghostbusters Proton Pack & Neutrona Wand.
 *   Copyright (C) 2023-2026 Michael Rajotte <michael.rajotte@gpstartechnologies.com>
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

/**
 * Colours are based on the RGB pattern; for GRB the Red/Green values should be switched.
 * Brightness is based on varying the intensity of each (0-255) using a relative value.
 *
 * However, colours based on Hue use a colour degree, a Saturation, and Value (brightness).
*/

enum colours {
  C_BLACK,
  C_WHITE,
  C_WARM_WHITE,
  C_PINK,
  C_PASTEL_PINK,
  C_RED,
  C_LIGHT_RED,
  C_RED2,
  C_RED3,
  C_RED4,
  C_RED5,
  C_ORANGE,
  C_BEIGE,
  C_YELLOW,
  C_CHARTREUSE,
  C_GREEN,
  C_DARK_GREEN,
  C_MINT,
  C_AQUA,
  C_LIGHT_BLUE,
  C_MID_BLUE,
  C_NAVY_BLUE,
  C_BLUE,
  C_PURPLE,
  C_REDGREEN,
  C_ORANGEPURPLE,
  C_PASTEL,
  C_RAINBOW,
  C_CUSTOM,
};

uint8_t getBrightness(uint8_t i_percent = 100) {
  // Brightness here is a percentage, to be converted to a range 0-255.
  if(i_percent > 100) {
    i_percent = 100;
  }
  return (uint8_t) ((255 * i_percent) / 100);
}

uint16_t getHue16(uint8_t hue) {
  return map(hue, 0, 255, 0, 65535);
}

// Special values for colour cycles: current hue (colour) and when to change colour.
// Unlike the pack, there is no need for tracking across multiple devices (only wand barrel).
uint8_t i_curr_colour = 0;
uint8_t i_count = 1;

uint32_t getHue(uint8_t i_colour, uint8_t i_brightness = 255, uint8_t i_saturation = 255) {
  // Brightness here is a value from 0-255 as limited by byte (uint8_t) type.

  // For colour cycles, this indicates how often to change colour.
  uint8_t i_cycle = 2;
  uint8_t i_output_colour = 0;

  // For the GPStar and Frutto barrel we need to reduce the cycle count so the colours are more perceptible.
  if(WAND_BARREL_LED_COUNT == LEDS_48 || WAND_BARREL_LED_COUNT == LEDS_50) {
    i_cycle = 255;
  }

  // Returns a CHSV object with a hue (colour), full saturation, and stated brightness.
  switch(i_colour) {
    case C_WHITE:
    default:
      // Turns on at 0 saturation, which is white.
      i_output_colour = 100;
      i_saturation = 0;
    break;

    case C_WARM_WHITE:
      // Tuned values to emulate warm white incandescent light.
      i_output_colour = 36;
      i_saturation = 183;
    break;

    case C_BLACK:
      // Set all values to 0 to turn off LED.
      i_output_colour = 0;
      i_saturation = 0;
      i_brightness = 0;
    break;

    case C_CUSTOM:
      i_output_colour = i_spectral_wand_custom_colour;
      i_saturation = i_spectral_wand_custom_saturation;
    break;

    case C_PINK:
      i_output_colour = 244;
    break;

    case C_PASTEL_PINK:
      i_output_colour = 244;
      i_saturation = 128;
    break;

    case C_RED:
      i_output_colour = 0;
    break;

    case C_LIGHT_RED:
      i_output_colour = 0;
      i_saturation = 192;
    break;

    case C_RED2:
      i_output_colour = 5;
    break;

    case C_RED3:
      i_output_colour = 10;
    break;

    case C_RED4:
      i_output_colour = 15;
    break;

    case C_RED5:
      i_output_colour = 20;
    break;

    case C_ORANGE:
      i_output_colour = 32;
    break;

    case C_BEIGE:
      i_output_colour = 43;
      i_saturation = 128;
    break;

    case C_YELLOW:
      i_output_colour = 64;
    break;

    case C_CHARTREUSE:
      i_output_colour = 80;
    break;

    case C_GREEN:
      i_output_colour = 96;
    break;

    case C_DARK_GREEN:
      i_output_colour = 96;
      i_brightness = 128 * (255 / i_brightness);
    break;

    case C_MINT:
      i_output_colour = 112;
      i_saturation = 120;
    break;

    case C_AQUA:
      i_output_colour = 128;
    break;

    case C_LIGHT_BLUE:
      i_output_colour = 145;
    break;

    case C_MID_BLUE:
      i_output_colour = 160;
    break;

    case C_NAVY_BLUE:
      i_output_colour = 170;
      i_saturation = 200;
      i_brightness = 112 * (255 / i_brightness);
    break;

    case C_BLUE:
      i_output_colour = 180;
    break;

    case C_PURPLE:
      i_output_colour = 192;
    break;

    case C_REDGREEN:
      // Alternate between red (0) and green (96).
      if(i_curr_colour != 0 && i_curr_colour != 96) {
        i_curr_colour = 0; // Reset if out of range.
      }

      if(WAND_ACTION_STATUS == ACTION_IDLE) {
        // Used to slow down colour transitions during the barrel fade effect.
        if(WAND_BARREL_LED_COUNT == LEDS_5) {
          i_cycle = 50;
        }
      }

      i_count++;

      if(i_count % i_cycle == 0) {
        if(i_curr_colour == 0) {
          i_curr_colour = 96;
          i_count = 1; // Reset counter.
        }
        else {
          i_curr_colour = 0;
          i_count = 1; // Reset counter.
        }
      }

      i_output_colour = i_curr_colour;
      i_saturation = 255;
    break;

    case C_ORANGEPURPLE:
      // Alternate between orange (15) and purple (210).
      if(i_curr_colour != 15 && i_curr_colour != 210) {
        i_curr_colour = 15; // Reset if out of range.
      }

      if(WAND_ACTION_STATUS == ACTION_IDLE) {
        // Used to slow down colour transitions during the barrel fade effect.
        if(WAND_BARREL_LED_COUNT == LEDS_5) {
          i_cycle = 50;
        }
      }

      i_count++;

      if(i_count % i_cycle == 0) {
        if(i_curr_colour == 15) {
          i_curr_colour = 210;
          i_count = 1; // Reset counter.
        }
        else {
          i_curr_colour = 15;
          i_count = 1; // Reset counter.
        }
      }

      i_output_colour = i_curr_colour;
      i_saturation = 255;
    break;

    case C_PASTEL:
      // Cycle through all colours (0-255) at half saturation.
      i_count++;

      if(i_count % i_cycle == 0) {
        i_curr_colour = (i_curr_colour + 5) % 255;
        i_count = 1; // Reset counter.
      }

      i_output_colour = i_curr_colour;
      i_saturation = 128;
    break;

    case C_RAINBOW:
      if(WAND_ACTION_STATUS == ACTION_IDLE) {
        // Used to slow down colour transitions during the barrel fade effect.
        if(WAND_BARREL_LED_COUNT == LEDS_48 || WAND_BARREL_LED_COUNT == LEDS_50) {
          i_cycle = 20;
        }
      }

      // Cycle through all colours (0-255) at full saturation.
      i_count++;

      if(i_count % i_cycle == 0) {
        i_curr_colour = (i_curr_colour + 5) % 255;
        i_count = 1; // Reset counter.
      }

      i_output_colour = i_curr_colour;
      i_saturation = 255;
    break;
  }

  return wand_led_output.ColorHSV(getHue16(i_output_colour), i_saturation, i_brightness);
}

uint32_t getHueAsRGB(uint8_t i_colour, uint8_t i_brightness = 255, bool b_grb = false) {
  // Get the initial colour using the HSV scheme.
  uint32_t rgb = getHue(i_colour, i_brightness);

  // Convert from HSV to RGB.
  //hsv2rgb_rainbow(hsv, rgb);

  if(b_grb) {
    // Swap red/green values before returning.
    uint8_t r = (uint8_t)(rgb >> 16);
    uint8_t g = (uint8_t)(rgb >>  8);
    uint8_t b = (uint8_t)(rgb >>  0);
    uint32_t grb = wand_led_output.Color(g, r, b);
    return wand_led_output.gamma32(grb);
  }
  else {
    return wand_led_output.gamma32(rgb); // Return RGB object.
  }
}

uint32_t getHueAsGRB(uint8_t i_colour, uint8_t i_brightness = 255) {
  // Forward to getHueAsRGB() with the flag set for GRB colour swap.
  return getHueAsRGB(i_colour, i_brightness, true);
}

uint32_t getHueColour(uint8_t i_colour, WAND_BARREL_LED_COUNTS NUM_LEDS_ENUM, uint8_t i_brightness = 255) {
  switch(NUM_LEDS_ENUM) {
    case LEDS_48:
    case LEDS_50:
    case LEDS_2:
      // All other LEDs are considered RGB
      return getHueAsRGB(i_colour, i_brightness);
    break;

    case LEDS_5:
    default:
      // Stock LEDs are GRB
      return getHueAsGRB(i_colour, i_brightness);
    break;
  }
}
