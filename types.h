#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;
using namespace std;


enum direction {
    NORMAL = 1,
    INVERTED = -1
};

struct triangle {
    int diff;
    vector<int> points;
    vector<int> pages;
    vector<int> newPoints;
    int oldPoints[3];
};

struct Info{
    string name;
    int value;
};

struct newPoint {
    int oldIndex;
    int direction;
    dvec3 vector;
};

struct plane {
    float t;
    dvec3 point;
    dvec3 normal;
    dvec3 side;
    dvec3 up;
};

struct line {
    dvec3 point1;
    dvec3 point2;
};

struct cubicCurve {
    double length;
    dvec3 p[4];
    dvec3 getPoint(double t) {
        double it = 1.0-t;
        return  it*it*it*p[0] +
               3*it*it*t*p[1] + 
                3*it*t*t*p[2] + 
                   t*t*t*p[3];
    }
    dvec3 getTangent(double t) {
        double it = 1.0-t;
        return 3*it*it*(p[1]-p[0]) +
                6*it*t*(p[2]-p[1]) +
                 3*t*t*(p[3]-p[2]);
    }
    dvec3 getSecondDerivative(double t) {
        return 6*(1-t)*p[0] + 
             6*(3*t-2)*p[1] + 
             6*(1-3*t)*p[2] +
                   6*t*p[3];
    }
};

struct Timing {
    string name;
    uint64_t start;
    uint64_t stop;
    void startTimer() {
        start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    }
    void stopTimer() {
        stop = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    }
    double getTime() {
        return ((double)(stop - start))/1000000000.0;
    }
};

#endif
