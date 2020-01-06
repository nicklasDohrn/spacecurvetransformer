//Reads curve.csv files to Plane Arrays
#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;

struct CubicCurve {
    dvec3 p[4];
    dvec3* getPoint(double t) {
        double it = 1.0-t;
        return &(it*it*it*p[0] +
               3*it*it*t*p[1] + 
               3*it*t*t*p[2] + 
               t*t*t*p[3]);
    }
    dvec3* getDerivative(double t) {
        double it = 1.0-t;
        return &(3*it*it*(p[1]-p[0]) +
               6*it*t*(p[2]-p[1]) +
               3*t*t*(p[3]-p[2]));
    }
};

#endif