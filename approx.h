#ifndef APPROX_H
#define APPROX_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "transformer.h"
#include "types.h"
#include "approx.h"

using namespace glm;
using namespace std;

class approx 
{
   public: 
        static double approxT(const dvec3* point, cubicCurve* segment, double t, int type);
        static int calculateEvolute(const plane *a,const plane *b, line* l);
};

#endif