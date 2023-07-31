/*  DaemonBite PSX USB Adapter
 *  Author: Hendrik Osterholz
 *
 *  Copyright (c) 2021 Hendrik Osterholz
 *
 *  GNU GENERAL PUBLIC LICENSE
 *  Version 3, 29 June 2007
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "Gamepad.h"

#define GAMEPAD_COUNT 2 // Set to 1 to 4


// ATT: 20 chars max (including NULL at the end) according to Arduino source code.
// Additionally serial number is used to differentiate arduino projects to have different button maps!
const char *gp_serial = "PSX to USB";

#include "Psx.h"

#define dataPin   18  //this is the first data pin. Controllers 2..n use dataPin+1..dataPin+n-1
#define cmndPin   2
#define attPin    3
#define clockPin  4

/* -------------------------------------------------------------------------
PSX controller socket (looking face-on at the front of the socket):
_________________________
| 9 8 7 | 6 5 4 | 3 2 1 |
\_______________________/

PSX controller plug (looking face-on at the front of the controller plug):
_________________________
| 1 2 3 | 4 5 6 | 7 8 9 |
\_______________________/

PSX       Arduino Pro Micro
--------------------------------------
1 DATA    A0  PF7
2 CMD     2   PD1
3 +7.6V
4 GND     GND
5 VCC     VCC
6 ATT     3   PD0
7 CLK     4   PD4
8 N/C
9 ACK

------------------------------------------------------------------------- */

Psx Psx;

// Set up USB HID gamepads
Gamepad_ Gamepad[GAMEPAD_COUNT];

// Controllers
uint16_t buttons[GAMEPAD_COUNT];
uint16_t buttonsPrev[GAMEPAD_COUNT];
uint8_t gp = 0;

void setup()
{
  Psx.setupPins(dataPin, cmndPin, attPin, clockPin, 10, GAMEPAD_COUNT);
  for (uint8_t i=0;i<GAMEPAD_COUNT;++i){
    buttonsPrev[i]=0;
  }
}

void loop() {
  // Send data to USB if values have changed
  Psx.read(buttons);
  for(uint8_t gp=0; gp<GAMEPAD_COUNT; gp++)
  {
    // Has any buttons changed state?
    if (buttons[gp] != buttonsPrev[gp] )
    {
      Gamepad[gp]._GamepadReport.buttons = buttons[gp] >> 4;
      Gamepad[gp]._GamepadReport.Y = ((buttons[gp] & psxDown) >> 1) - ((buttons[gp] & psxUp) >> 3);
      Gamepad[gp]._GamepadReport.X = ((buttons[gp] & psxRight) >> 2) - (buttons[gp] & psxLeft);
      buttonsPrev[gp] = buttons[gp];
      Gamepad[gp].send();
    }
  }

}
