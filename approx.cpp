
#include <vector>
#include <unistd.h>
#include <vector>
#include <thread>
#include <sys/shm.h>
#include <sys/wait.h>
#include <malloc.h>
#include <stdlib.h>
#include <sstream>
#include <errno.h>
#include <error.h>
#include <math.h>
#include <glm/gtx/vector_angle.hpp>
#include "transformer.h"
#include "types.h"
#include "approx.h"

#define SMALL_NUM   0.0000001
#define BASESTEP    0.001

double getDistanceLine(const dvec3 *point,const dvec3 *l1, const dvec3 *l2) {
    return length((*point - *l1) * (*point - *l2));
}

double bilinearApprox(double fa, double fb, double a, double b, cubicCurve* segment, const dvec3* p) {
    double half = a + (b-a)/2;
    if ((b-a) <= SMALL_NUM)
        return half;
    double fhalf = dot(*p-segment->getPoint(half), segment->getTangent(half));
    if(sign(fhalf) != sign(fa) && sign(fhalf) != sign(fb)) {
        double t1 = bilinearApprox(fa,fhalf,a,half,segment,p);
        double t2 = bilinearApprox(fhalf,fb,half,b,segment,p);
        if(abs(dot(*p-segment->getPoint(t1), segment->getTangent(t1))) < abs(dot(*p-segment->getPoint(t2), segment->getTangent(t2))))
            return t1;
        return t2;
    }
    if(sign(fhalf) != sign(fa)) {
        return bilinearApprox(fa,fhalf,a,half,segment,p);
    } else if  (sign(fhalf) != sign(fb)) {
        return bilinearApprox(fhalf,fb,half,b,segment,p);
    } else
        return half;
}

double approx::approxT (const dvec3* p, cubicCurve* segment, double t, int type) {
    switch (type) {
        case 0:
            return t;
        case 1:
            double t1 = t;
            int count = 0;
            do{
                count++;
                t = t1;
                dvec3 cu = segment->getPoint(t);
                dvec3 c1u = segment->getTangent(t);
                dvec3 c2u = segment->getSecondDerivative(t);
                t1 = t - (dot(*p-cu, c1u))/(dot(*p-cu,c2u)-dot(c1u,c1u));
            } while(std::abs(t) - std::abs(t1) > SMALL_NUM && count < 50 && (t1 < 1 || t1 > 0));

            if(std::abs(t) - std::abs(t1) > SMALL_NUM) {
                double fa = dot(*p-segment->getPoint(0), segment->getTangent(0));
                double fb = dot(*p-segment->getPoint(1), segment->getTangent(1));
                t = bilinearApprox(fa,fb,0,1,segment,p);
            }
            return t;
    }
    return 0;
}

