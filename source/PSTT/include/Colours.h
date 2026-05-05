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
  C_BLUEGREEN,
  C_REDGREEN,
  C_ORANGEPURPLE,
  C_REDPURPLE,
  C_PASTEL,
  C_RAINBOW,
  C_CUSTOM,
};

inline uint32_t maximizeBrightness(uint32_t rgb, uint8_t limit = 255)  {
  uint8_t red = (uint8_t)(rgb >> 16);
  uint8_t green = (uint8_t)(rgb >> 8);
  uint8_t blue = (uint8_t)(rgb >> 0);
  uint8_t max = red;
  if(green > max) max = green;
  if(blue > max) max = blue;

  // stop div/0 when color is black
  if(max > 0) {
    uint16_t factor = ((uint16_t)(limit) * 256) / max;
    red =   (red   * factor) / 256;
    green = (green * factor) / 256;
    blue =  (blue  * factor) / 256;
  }
  return pstt_led_output.Color(red, green, blue);
}

uint8_t getBrightness(uint8_t i_percent = 100) {
  // Brightness here is a percentage, to be converted to a range 0-255.
  if(i_percent > 100) {
    i_percent = 100;
  }
  return (uint8_t) ((255 * i_percent) / 100);
}

// Adafruit implementation of FastLED nscale8 function.
uint32_t nscale8(uint32_t colour, uint8_t scale) {
  uint8_t r = (uint8_t)(colour >> 16);
  uint8_t g = (uint8_t)(colour >>  8);
  uint8_t b = (uint8_t)colour;

  r = (r * scale) >> 8;
  g = (g * scale) >> 8;
  b = (b * scale) >> 8;

  // This function should only ever be used on correctly-ordered colors, so always output RGB.
  return pstt_led_output.Color(r, g, b);
}

// Adafruit implementation of FastLED nscale8_video function.
uint32_t nscale8_video(uint32_t colour, uint8_t scale) {
  uint8_t nonzeroscale = (scale != 0) ? 1 : 0;
  uint8_t r = (uint8_t)(colour >> 16);
  uint8_t g = (uint8_t)(colour >>  8);
  uint8_t b = (uint8_t)colour;

  r = (r == 0) ? 0 : ((r * scale) >> 8) + nonzeroscale;
  g = (g == 0) ? 0 : ((g * scale) >> 8) + nonzeroscale;
  b = (b == 0) ? 0 : ((b * scale) >> 8) + nonzeroscale;

  // This function should only ever be used on correctly-ordered colors, so always output RGB.
  return pstt_led_output.Color(r, g, b);
}

// Adafruit implementation of FastLED blur1d function.
void blur1d(Adafruit_NeoPXL8 &leds, uint16_t numLeds, uint8_t blurAmount) {
  uint8_t keep = 255 - blurAmount;
  uint8_t seep = blurAmount >> 1;
  uint32_t carryover = leds.Color(0,0,0);
  for(uint16_t i = 0; i < numLeds; ++i) {
    uint32_t cur = leds.getPixelColor(i);
    uint32_t part = cur;
    part = nscale8(part, seep);
    cur = nscale8(cur, keep);
    cur += carryover;
    if(i)
      leds.setPixelColor(i-1, (leds.getPixelColor(i-1) + part)); //leds[i - 1] += part;
    leds.setPixelColor(i, cur); //leds[i] = cur;
    carryover = part;
  }
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

    case C_BLUEGREEN:
      // Alternate between blue (145) and green (96).
      if(i_curr_colour != 145 && i_curr_colour != 96) {
        i_curr_colour = 145; // Reset if out of range.
      }

      i_cycle = 50;
      i_count++;

      if(i_count % i_cycle == 0) {
        if(i_curr_colour == 96) {
          i_curr_colour = 145;
          i_count = 1; // Reset counter.
        }
        else {
          i_curr_colour = 96;
          i_count = 1; // Reset counter.
        }
      }

      i_output_colour = i_curr_colour;
      i_saturation = 255;
    break;

    case C_REDGREEN:
      // Alternate between red (0) and green (96).
      if(i_curr_colour != 0 && i_curr_colour != 96) {
        i_curr_colour = 0; // Reset if out of range.
      }

      i_cycle = 50;
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

      i_cycle = 7;
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

    case C_REDPURPLE:
      // Alternate between orange (0) and purple (210).
      if(i_curr_colour != 0 && i_curr_colour != 210) {
        i_curr_colour = 0; // Reset if out of range.
      }

      i_cycle = 7;
      i_count++;

      if(i_count % i_cycle == 0) {
        if(i_curr_colour == 0) {
          i_curr_colour = 210;
          i_count = 1; // Reset counter.
        }
        else {
          i_curr_colour = 210;
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
      // Cycle through all colours (0-255) at full saturation.
      i_count++;

      if(i_count % i_cycle == 0) {
        i_curr_colour = (i_curr_colour + 5) % 255;
        i_count = 1; // Reset counter.
      }

      i_output_colour = i_curr_colour;
      i_saturation = 255;
    break;

    case C_CUSTOM:
      //i_output_colour = i_spectral_custom_colour;
      //i_saturation = i_spectral_custom_saturation;
    break;
  }

  return pstt_led_output.ColorHSV(getHue16(i_output_colour), i_saturation, i_brightness);
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
    uint32_t grb = pstt_led_output.Color(g, r, b);
    return pstt_led_output.gamma32(grb);
  }
  else {
    return pstt_led_output.gamma32(rgb); // Return RGB object.
  }
}

uint32_t getHueColour(uint8_t i_colour, uint8_t i_brightness = 255) {
  return getHueAsRGB(i_colour, i_brightness);
}
