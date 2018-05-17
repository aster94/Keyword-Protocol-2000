/*
KWP2000.h

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef KWP2000_h
#define KWP2000_h

#include "Arduino.h"

//CHIEDERE SE dovrebbero essere una lista di define o enumeratori
#define GSXR_600_2011  061
#define GSXR_750_2011  071
#define GSXR_1000_2011  011

#define GSXR_600_2012  062
#define GSXR_750_2012  072
#define GSXR_1000_2012  012

#define NINJA_636_2011  161
#define NINJA_1000_2011  111

class KWP2000{
  public:

    KWP2000(uint8_t model, uint32_t baudrate/*serial port, pins, baudrate, motorbike*/);
    int begin();
    int initKline();
    int stopKline();
  
  private:

    // private declatations:
    uint8_t _model;
    uint32_t _baudrate;

    // private functions
    //uint8_t getError(uint8_t);


};

#endif