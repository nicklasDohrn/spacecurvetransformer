
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

double getDistancePlane(const dvec3 *point,const dvec3 planePoint, const dvec3 planeNormal) {
    return dot((planePoint - *point), planeNormal);
}

int approx::calculateEvolute(const plane *a,const plane *b, line* l)
{
    dvec3 u = a->normal * b->normal;
    double ax = (u.x >= 0 ? u.x : -u.x);
    double ay = (u.y >= 0 ? u.y : -u.y);
    double az = (u.z >= 0 ? u.z : -u.z);

    if ((ax+ay+az) < SMALL_NUM) {
        dvec3   v = b->point -  a->point;
        if (dot(a->normal, v) == 0) 
            return 1;                  
        else
            return 0; 
    }
    int maxc; 
    if (ax > ay) {
        if (ax > az)
             maxc =  1;
        else maxc = 3;
    }
    else {
        if (ay > az)
             maxc =  2;
        else maxc = 3;
    }

    dvec3 iP;                
    double d1, d2;            
    d1 = -dot(a->normal, a->point);  
    d2 = -dot(b->normal, b->point); 

    switch (maxc) {
        case 1:
            iP.x = 0;
            iP.y = (d2*a->normal.z - d1*b->normal.z) /  u.x;
            iP.z = (d1*b->normal.y - d2*a->normal.y) /  u.x;
            break;
        case 2:                     
            iP.x = (d1*b->normal.z - d2*a->normal.z) /  u.y;
            iP.y = 0;
            iP.z = (d2*a->normal.x - d1*b->normal.x) /  u.y;
            break;
        case 3:
            iP.x = (d2*a->normal.y - d1*b->normal.y) /  u.z;
            iP.y = (d1*b->normal.x - d2*a->normal.x) /  u.z;
            iP.z = 0;
    }
    l->point1 = iP;
    l->point2 = iP + u;
    return 2;
}

double getRatio(const dvec3* point,dvec3 pointSegment,  const line* evolute, int mode) {
    double evPoint = getDistanceLine(point, &evolute->point1, &evolute->point2);
    double evSegment = getDistanceLine(&pointSegment, &evolute->point1, &evolute->point2);
    double ratio = (evSegment/evPoint);
    if(mode == INVERTED) {
        return -ratio;
    }
    return ratio;

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

