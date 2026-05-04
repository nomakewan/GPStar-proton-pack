/**
 *   GPStar Proton Pack - Ghostbusters Proton Pack & Neutrona Wand.
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

enum colours : uint8_t {
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
  C_BLUEFADE,
  C_PASTEL,
  C_RAINBOW,
  C_CUSTOM_POWERCELL,
  C_CUSTOM_CYCLOTRON,
  C_CUSTOM_INNER_CYCLOTRON,
  C_HASLAB
};

enum device : uint8_t {
  POWERCELL,
  CYCLOTRON_OUTER,
  CYCLOTRON_INNER,
  CYCLOTRON_CAVITY,
  CYCLOTRON_PANEL,
  VENT_LIGHT
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
  return pack_led_output.Color(red, green, blue);
}

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
// This must match the number of device ENUM entries (though that is rarely changed).
uint8_t i_curr_colour[6] = { 0, 0, 0, 0, 0, 0 };
uint8_t i_count[6] = { 0, 0, 0, 0, 0, 0 };

uint8_t getDeviceColour(uint8_t i_device, uint8_t i_firing_mode, bool b_toggle) {
  // Toggle indicates use of Video Game colours, which is based on the firing mode.
  // Otherwise a default colour will be used based on the device itself.
  if(b_toggle) {
    switch(i_firing_mode != SETTINGS ? i_firing_mode : gpstarPack.getPreviousStreamMode()) {
      case PROTON:
      default:
        switch(i_device) {
          case POWERCELL:
            return C_MID_BLUE;
          break;

          case CYCLOTRON_OUTER:
          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
            return C_RED;
          break;

          case CYCLOTRON_CAVITY:
            // Cycles through 3 colours, changing on each call.
            // If starting at 0, value will increment to 1.
            // If value is above/divisible by 4, reset to 1.
            i_count[i_device]++;
            if(i_count[i_device] > 4 || i_count[i_device] % 4 == 0) {
              i_count[i_device] = 1; // Reset counter.
            }

            switch(i_count[i_device]) {
              case 1:
                return C_ORANGE;
              break;
              case 2:
                return C_WHITE;
              break;
              case 3:
              default:
                return C_YELLOW;
              break;
            }
          break;

          // VENT_LIGHT colour in PROTON mode will always be overridden by void ventLight()
          case VENT_LIGHT:
          default:
            return C_WHITE;
          break;
        }
      break;

      case SLIME:
        switch(i_device) {
          case POWERCELL:
            if(gpstarPack.getSystemTheme() == SYSTEM_1989) {
              return C_PINK;
            }
            else {
              return C_GREEN;
            }
          break;

          case CYCLOTRON_OUTER:
          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
          case VENT_LIGHT:
          default:
            if(gpstarPack.getSystemTheme() == SYSTEM_1989) {
              return C_PINK;
            }
            else {
              return C_GREEN;
            }
          break;
        }
      break;

      case STASIS:
        switch(i_device) {
          case POWERCELL:
          case CYCLOTRON_OUTER:
          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
          case VENT_LIGHT:
          default:
            return C_LIGHT_BLUE;
          break;
        }
      break;

      case MESON:
        switch(i_device) {
          case POWERCELL:
          case CYCLOTRON_OUTER:
          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
          case VENT_LIGHT:
          default:
            return C_ORANGE;
          break;
        }
      break;

      case SPECTRAL:
        switch(i_device) {
          case POWERCELL:
          case CYCLOTRON_OUTER:
          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
          case VENT_LIGHT:
          default:
            return C_RAINBOW;
          break;
        }
      break;

      case HOLIDAY_HALLOWEEN:
        switch(i_device) {
          case POWERCELL:
          case CYCLOTRON_OUTER:
          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
          case VENT_LIGHT:
          default:
            return C_ORANGEPURPLE;
          break;
        }
      break;

      case HOLIDAY_CHRISTMAS:
        switch(i_device) {
          case POWERCELL:
          case CYCLOTRON_OUTER:
          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
          case VENT_LIGHT:
          default:
            return C_REDGREEN;
          break;
        }
      break;

      case SPECTRAL_CUSTOM:
        switch(i_device) {
          case POWERCELL:
            return C_CUSTOM_POWERCELL;
          break;

          case CYCLOTRON_OUTER:
            return C_CUSTOM_CYCLOTRON;
          break;

          case CYCLOTRON_INNER:
          case CYCLOTRON_PANEL:
            return C_CUSTOM_INNER_CYCLOTRON;
          break;

          case VENT_LIGHT:
          default:
            return C_CUSTOM_CYCLOTRON;
          break;
        }
      break;
    }
  }
  else {
    switch(i_device) {
      case POWERCELL:
        return C_MID_BLUE;
      break;

      case CYCLOTRON_OUTER:
      case CYCLOTRON_INNER:
      case CYCLOTRON_PANEL:
        return C_RED;
      break;

      case CYCLOTRON_CAVITY:
        // Cycles through 3 colours, changing on each call.
        // If starting at 0, value will increment to 1.
        // If value is above/divisible by 4, reset to 1.
        i_count[i_device]++;
        if(i_count[i_device] > 4 || i_count[i_device] % 4 == 0) {
          i_count[i_device] = 1; // Reset counter.
        }

        switch(i_count[i_device]) {
          case 1:
            return C_ORANGE;
          break;
          case 2:
            return C_WHITE;
          break;
          case 3:
          default:
            return C_YELLOW;
          break;
        }
      break;

      // VENT_LIGHT colour in PROTON mode will always be overridden by void ventLight()
      case VENT_LIGHT:
      default:
        return C_WHITE;
      break;
    }
  }
}

uint32_t getHue(uint8_t i_device, uint8_t i_colour, uint8_t i_brightness = 255, uint8_t i_saturation = 255, bool b_fade = false) {
  // Brightness here is a value from 0-255 as limited by byte (uint8_t) type.

  // For colour cycles, i_cycle indicates how often to change colour.
  // This is device-dependent in order to provide a noticeable change.
  // Value must be >0 as this is used with modulo (cannot mod by 0).
  uint8_t i_cycle = 2;
  uint8_t i_output_colour = i_curr_colour[i_device];

  switch(i_device) {
    case CYCLOTRON_OUTER:
      if(gpstarPack.isThemeModern()) {
        i_cycle = 10;
      }
    break;
    case CYCLOTRON_INNER:
    case CYCLOTRON_PANEL:
      i_cycle = 6;
    break;
    default:
      // Do nothing.
    break;
  }

  // Returns a CHSV object with a hue (colour), full saturation, and stated brightness.
  switch(i_colour) {
    case C_HASLAB:
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

    case C_CUSTOM_POWERCELL:
      i_output_colour = i_spectral_powercell_custom_colour;
      i_saturation = i_spectral_powercell_custom_saturation;
    break;

    case C_CUSTOM_CYCLOTRON:
      i_output_colour = i_spectral_cyclotron_custom_colour;
      i_saturation = i_spectral_cyclotron_custom_saturation;
    break;

    case C_CUSTOM_INNER_CYCLOTRON:
      i_output_colour = i_spectral_cyclotron_inner_custom_colour;
      i_saturation = i_spectral_cyclotron_inner_custom_saturation;
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
      if(i_curr_colour[i_device] != 0 && i_curr_colour[i_device] != 96) {
        i_curr_colour[i_device] = 0; // Reset if out of range.
        i_output_colour = i_curr_colour[i_device];
      }

      if(!b_fade) {
        i_count[i_device]++;

        if(i_count[i_device] % i_cycle == 0) {
          if(i_curr_colour[i_device] == 0) {
            i_curr_colour[i_device] = 96;
            i_output_colour = i_curr_colour[i_device];
            i_count[i_device] = 1; // Reset counter.
          }
          else {
            i_curr_colour[i_device] = 0;
            i_output_colour = i_curr_colour[i_device];
            i_count[i_device] = 1; // Reset counter.
          }
        }
      }

      i_saturation = 255;
    break;

    case C_ORANGEPURPLE:
      // Alternate between orange (15) and purple (210).
      if(i_curr_colour[i_device] != 15 && i_curr_colour[i_device] != 210) {
        i_curr_colour[i_device] = 15; // Reset if out of range.
        i_output_colour = i_curr_colour[i_device];
      }

      if(!b_fade) {
        i_count[i_device]++;

        if(i_count[i_device] % i_cycle == 0) {
          if(i_curr_colour[i_device] == 15) {
            i_curr_colour[i_device] = 210;
            i_output_colour = i_curr_colour[i_device];
            i_count[i_device] = 1; // Reset counter.
          }
          else {
            i_curr_colour[i_device] = 15;
            i_output_colour = i_curr_colour[i_device];
            i_count[i_device] = 1; // Reset counter.
          }
        }
      }

      i_saturation = 255;
    break;

    case C_BLUEFADE:
      // Reset if out of range: blue (160) to light blue (146).
      // This is based on use of the 15-LED RGB Power Cell.
      if(i_count[i_device] < 146 || i_count[i_device] > 160) {
        i_count[i_device] = 160; // Reset if out of range.
      }

      // Cycles from dark to light blue (160-145) at full saturation.
      i_count[i_device]--;

      i_output_colour = i_count[i_device];
      i_saturation = 255;
    break;

    case C_PASTEL:
      // Cycle through all colours (0-255) at half saturation.
      if(!b_fade) {
        i_count[i_device]++;
      }

      if(i_count[i_device] % i_cycle == 0) {
        i_curr_colour[i_device] = (i_curr_colour[i_device] + 5) % 255;
        i_output_colour = i_curr_colour[i_device];
        i_count[i_device] = 1; // Reset counter.
      }

      if(b_fade) {
        i_output_colour = (i_curr_colour[i_device] - 5) % 255;
      }

      i_saturation = 128;
    break;

    case C_RAINBOW:
      // Cycle through all colours (0-255) at full saturation.
      if(!b_fade) {
        i_count[i_device]++;
      }

      if(i_count[i_device] % i_cycle == 0) {
        i_curr_colour[i_device] = (i_curr_colour[i_device] + 5) % 255;
        i_output_colour = i_curr_colour[i_device];
        i_count[i_device] = 1; // Reset counter.
      }

      if(b_fade) {
        i_output_colour = (i_curr_colour[i_device] - 5) % 255;
      }

      i_saturation = 255;
    break;
  }

  return pack_led_output.ColorHSV(getHue16(i_output_colour), i_saturation, i_brightness);
}

uint32_t getHueAsRGB(uint8_t i_device, uint8_t i_colour, uint8_t i_brightness = 255, bool b_grb = false, bool b_fade = false) {
  // Get the initial colour using the HSV scheme.
  uint32_t rgb = getHue(i_device, i_colour, i_brightness, 255, b_fade);

  // Convert from HSV to RGB.
  //hsv2rgb_rainbow(hsv, rgb);

  if(b_grb) {
    // Swap red/green values before returning.
    uint8_t r = (uint8_t)(rgb >> 16);
    uint8_t g = (uint8_t)(rgb >>  8);
    uint8_t b = (uint8_t)(rgb >>  0);
    uint32_t grb = pack_led_output.Color(g, r, b);
    return pack_led_output.gamma32(grb);
  }
  else {
    return pack_led_output.gamma32(rgb); // Return RGB object.
  }
}

uint32_t getHueAsGRB(uint8_t i_device, uint8_t i_colour, uint8_t i_brightness = 255) {
  // Forward to getHueAsRGB() with the flag set for GRB colour swap.
  return getHueAsRGB(i_device, i_colour, i_brightness, true);
}

uint32_t getHueAsGBR(uint8_t i_device, uint8_t i_colour, uint8_t i_brightness = 255) {
  // Get the initial colour using the HSV scheme.
  uint32_t rgb = getHue(i_device, i_colour, i_brightness);

  // Convert from RGB to GBR.
  uint8_t r = (uint8_t)(rgb >> 16);
  uint8_t g = (uint8_t)(rgb >>  8);
  uint8_t b = (uint8_t)(rgb >>  0);
  uint32_t gbr = pack_led_output.Color(g, b, r);

  // Swap colour values before returning.
  return pack_led_output.gamma32(gbr);
}
