#include <iostream>
#include <iomanip> 
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono> 
#include "timer.h"

using namespace std;

static vector<Timing> timings;
static vector<Info> info;

void timer::startTimer(string name) {
        for(int i = 0; i < timings.size(); i++) {
            if(timings[i].name.compare(name) == 0) {
                timings[i].startTimer();
                return;
            }
        }
        timings.push_back(Timing{name});
        timings[timings.size()-1].startTimer();
    }

    void timer::stopTimer(string name) {
        for(int i = 0; i < timings.size(); i++) {
            if(timings[i].name.compare(name) == 0) {
                timings[i].stopTimer();
            }
        }
    }

    vector<Timing> timer::getAllTimer() {
        return timings;
    }

    void timer::setInfo(string name, int value) {
        info.push_back(Info{
            name, value
        });
    }

    vector<Info> timer::getInfo() {
        return info;
    }