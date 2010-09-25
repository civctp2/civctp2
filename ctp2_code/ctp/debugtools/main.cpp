// testing high-resolution timer
// This program demonstrates the accuracy comparison between standard clock()
// and Timer class.
// It measures the elapsed time of each step in the loop. High precision timer
// should measure at least 1 millisecond defference.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-04-20
// UPDATED: 2006-04-20
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <ctime>
#include "Timer.h"
using namespace std;

int main(int argc, char *argv[])
{
    double start, stop, tick1, tick2;

    // first, use std clock() function to measure elapsed time ////////////////
    start = stop = tick1 = tick2 = (double)clock(); // start timer and remember initial ticks

    while((stop-start)/CLOCKS_PER_SEC < 1)    // loop for 1 sec
    {
        cout << (tick2 - tick1) / CLOCKS_PER_SEC * 1000 << " ms." << endl;

        // reset timer
        tick1 = tick2;
        tick2 = stop = (double)clock();
    }

    // pause until user input
    cout << "\n\nPress Enter key to run Timer class...\n\n";
    char c;
    cin.get(c);



    // second, use Timer::getElapsedTime() ////////////////////////////////////
    Timer t;

    // start timer
    t.start();
    tick1 = tick2 = t.getElapsedTimeInMilliSec();

    while(t.getElapsedTime() < 1)       // loop for 1 sec
    {
        cout << (tick2 - tick1) << " ms." << endl;

        tick1 = tick2;
        tick2 = t.getElapsedTimeInMilliSec();
    }

    cout << CLOCKS_PER_SEC << endl;
    cout << CLK_TCK << endl;
    cout << clock()/CLOCKS_PER_SEC << endl;

    return 0;
}
