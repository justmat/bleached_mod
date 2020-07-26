/* Sonoclast pd knobs for TeensyLC
 
  * Modified by andrew for bleached (Teensy 3.2) 02/22/2019
  * Modified yet again by mat 7/26/2020

   Copyright March 2019 - havencking@gmail.com

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

   The Sonoclast pd knobs is a MIDI CC controller designed particularly
   for use with pure data.  However it could be used to control any
   software that recognizes MIDI CC messages.  Customize the Control
   Channels below if you like.

   Compiling instructions:

   Follow these instructions to install the Arduino software and Tensyduino:
   https://www.pjrc.com/teensy/td_download.html

   In the Arduino software be sure to select the following:
   Tools > Board > Teensy3.2
   Tools > USB Type > Serial + MIDI
*/

// led state
bool led_state = false;

// Map MIDI CC channels to knobs numbered left to right.
#define CC01  102
#define CC02  103
#define CC03  104
#define CC04  105
#define CC05  106
#define CC06  107
#define CC07  108
#define CC08  109
#define CC09  110
#define CC10  111
#define CC11  112
#define CC12  113
#define CC13  114

// Map the TeensyLC pins to each potentiometer numbered left to right.
#define POT01 0
#define POT02 1
#define POT03 2
#define POT04 3
#define POT05 4
#define POT06 5
#define POT07 6


// Use this MIDI channel.
#define MIDI_CHANNEL 1

// Send MIDI CC messages for all 7 knobs after the main loop runs this many times.
// This prevents having to twiddle the knobs to update the receiving end.
// 10,000 loops is roughly 10 seconds.
//#define LOOPS_PER_REFRESH 10000

// potentiometer read parameters
#define POT_BIT_RES         10 // 10 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

// Track the knob state.
uint16_t prev_pot_val[7] = {0xffff,
                             0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
                            };

// an array of potentiometer pins
uint8_t pot[7] = {POT01,
                   POT02, POT03, POT04, POT05, POT06, POT07
                  };

// an array of CC numbers
uint8_t cc_set_1[6] = {
                  CC02, CC03, CC04, CC05, CC06, CC07
                 };

// an array of CC numbers
uint8_t cc_set_2[6] = {
                  CC08, CC09, CC10, CC11, CC12, CC13
                 };

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 5;

// Count the number of main loops.
uint16_t loop_count = 0;

void setup() {
  // serial monitoring for debugging
  Serial.begin(38400);
  // led pin mode
  pinMode(13, OUTPUT);
  // potentiometers
  analogReadResolution(POT_BIT_RES);
  analogReadAveraging(POT_NUM_READS);
}

void loop() {
  // Read each knob, and send MIDI CC only if the value changed.
  for (uint8_t i = 0; i < 7; i++) {
    uint16_t pot_val = analogRead(pot[i]);
    if ((pot_val < prev_pot_val[i] - nbrhd) ||
        (pot_val > prev_pot_val[i] + nbrhd)) {
          // use pot1 to control led  
          if (i == 0 and pot_val > 511) {
            led_state = true;
            digitalWrite(13, HIGH);
          }
          else if (i == 0 and pot_val < 511) {
            led_state = false;
            digitalWrite(13, LOW);
          }
          // use led state to decide which cc_set to use
          if (i != 0 and (led_state)) {
            //Serial.print(cc_set_2[i - 1]);
            usbMIDI.sendControlChange(cc_set_2[i - 1], pot_val >> (POT_BIT_RES - 7), MIDI_CHANNEL);
          }
          else if (i != 0 and not (led_state)) {
            //Serial.print(cc_set_1[i - 1]);
            usbMIDI.sendControlChange(cc_set_1[i - 1], pot_val >> (POT_BIT_RES - 7), MIDI_CHANNEL);
          }
          //Serial.println("");
          prev_pot_val[i] = pot_val;
    }
  }

  // Periodically send MIDI CC for every knob so that the receiving end matches the knobs
  // even when changing pure data patches.
//  if (loop_count > LOOPS_PER_REFRESH) {
//    for (uint8_t i = 0; i < 7; i++) {
//      usbMIDI.sendControlChange(cc[i], analogRead(pot[i]) >> (POT_BIT_RES - 7), MIDI_CHANNEL);
//    }
//    loop_count = 0;
//  }
//  loop_count++;
}
