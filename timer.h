#ifndef TIMER_H
#define TIMER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "transformer.h"
#include "types.h"
#include "timer.h"

using namespace std;



class timer 
{

   public: 
        static void startTimer(string name);
        static void stopTimer(string name);
        static vector<Timing> getAllTimer();
        static vector<Info> getInfo();
        static void setInfo(string name, int value);
};

#endif