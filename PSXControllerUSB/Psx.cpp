/*  PSX Controller Decoder Library (Psx.cpp)
	Written by: Kevin Ahrendt June 22nd, 2008

	Controller protocol implemented using Andrew J McCubbin's analysis.
	http://www.gamesx.com/controldata/psxcont/psxcont.htm

	Shift command is based on tutorial examples for ShiftIn and ShiftOut
	functions both written by Carlyn Maw and Tom Igoe
	http://www.arduino.cc/en/Tutorial/ShiftIn
	http://www.arduino.cc/en/Tutorial/ShiftOut

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
*/
#include "Psx.h"

Psx::Psx()
{
}

void Psx::shift(uint8_t dataOut, uint8_t* dataIn)      // Does the actual shifting, both in and out simultaneously
{
  for (uint8_t i = 0; i<8; ++i)
    {

      digitalWrite(_cmndPin,dataOut & 0x01);
      dataOut>>=1;
      digitalWrite(_clockPin, LOW);

      delayMicroseconds(_delay);

      for (uint8_t j=0;j<_n_controllers;j++)
        {
          dataIn[2*j]= (dataIn[2*j]<<1)| !digitalRead(_dataPin+j); //the pointed mem is actually u16, write only H/L part
        }

      digitalWrite(_clockPin, HIGH);
      delayMicroseconds(_delay);
    }
}


void Psx::setupPins(uint8_t dataPin, uint8_t cmndPin, uint8_t attPin, uint8_t clockPin, uint8_t delay,uint8_t n_controllers)
{

  for (uint8_t i=0; i<n_controllers; ++i){
    pinMode(dataPin+i, INPUT);
    digitalWrite(dataPin+i, HIGH);	// Turn on internal pull-up
  }
  _dataPin = dataPin;

  pinMode(cmndPin, OUTPUT);
  _cmndPin = cmndPin;

  pinMode(attPin, OUTPUT);
  _attPin = attPin;
  digitalWrite(_attPin, HIGH);

  pinMode(clockPin, OUTPUT);
  _clockPin = clockPin;
  digitalWrite(_clockPin, HIGH);

  _delay = delay;
  _n_controllers=n_controllers;
}


void Psx::read(uint16_t* state)
{
  digitalWrite(_attPin, LOW);

  uint8_t* lowState=(uint8_t*)state;

  shift(0x01,lowState);
  shift(0x42,lowState);
  shift(0xFF,lowState);


  for (uint8_t i=0; i<_n_controllers; ++i)
    {
      state[i]=0;
    }

  shift(0xFF,lowState);
  shift(0xFF,lowState+1);

  digitalWrite(_attPin, HIGH);

}
