/*
  MIT License

  Copyright (c) 2026 Stefan Mader

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//------
// From 286 assembly to 386 flatmode C rather fastly converted DOS replay routine for the:
//------
// Surprise! productions Adlib Tracker Version 2.0 >>> Beta Release <<< (SADT2) by Erik Pojar / S!P
//------
// To be used with WatcomC++/OpenWatcom/DJGPP
//------
// This file is not intended to look good and maybe there are some conversion errors left.
// Conversion done by me (Stefan Mader,Mad^Still^Bauknecht^Puls4r)
//------
// From the Surprise Adlib Tracker 2.0 Author
// Welcome to the SADT V2.0 Editor / Player for Adlib!
// I know this program is out-dated by new soundcards, and I have not used this
// program for a long time now.
// But still I like the sound of FM chips and therefore I wanted to share it with
// YOU.You are free to use this program in any form you like with 3 restrictions:
//
//        * This program package may not be sold.
//          (this means especially PD companies or other assholes who try
//           to earn money with the work of idealistic programmers..)
//
//        * If you use this program, you HAVE to send me a postcard
//          (and a nice one..) My address is at the end of the file.
//
//        * If you use the replay routine in a commercial production
//          (a game or so..) you HAVE to send me a copy of it.
//
// I will kick everybody's ass who violates any of these restrictions.
// I would be pleased if you send me music you composed with my little editor.
// If you have any ideas or comments to make, send them to me immediately.
// ------------------------------------------------------------------------------
//    Erik Pojar                 Voice: ++43 (1) 2574457 (regular times please!)
//    Siegfriedgasse 55
//    1210 Wien                  epojar@stud1.tuwien.ac.at
//    Austria
// ------------------------------------------------------------------------------
//------
#ifndef __SA2_HPP__
#define __SA2_HPP__

extern volatile unsigned short current_bpm;

void adlibTrackerLock(void *data, unsigned long size); // actually this currently doesn't lock all data (WatcomC stuff that is initialized with 0 lands in bss section :/ )
void adlibTrackerUnlock(void *data, unsigned long size);

bool prepare_song(unsigned char *data);
void play_line();
void stop_all();

bool set_main_volume(unsigned char bl);

#endif // __SA2_HPP__
