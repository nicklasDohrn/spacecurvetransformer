// Main class
#include <iostream>
#include <iomanip> 
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <forward_list>
#include <iterator>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include <chrono> 
#include "curveReader.h"
#include "beziercurve.h"

using namespace std;
using namespace glm;


CubicCurve createBezier(const Plane *a, const Plane *b) {
    float dist = (b->t - a->t) / 3;
    return {
        {
            dvec3(a->point.x,a->point.y,a->point.z),
            dvec3(a->point.x + (dist * a->normal.x),
                  a->point.y + (dist * a->normal.y),
                  a->point.z + (dist * a->normal.z)),
            dvec3(b->point.x - (dist * b->normal.x),
                  b->point.y - (dist * b->normal.y),
                  b->point.z - (dist * b->normal.z)),
            dvec3(b->point.x,b->point.y,b->point.z)
        }
    };
}

int main  (int argc, char *argv[]) {

    curveReader::readFromFile(argv[1]);
    vector<Plane> *c = curveReader::getPlanes();

    ofstream point_file (argv[2]);
    CubicCurve segment;
    int segment_size = 512;
    for(int i = 0; i < c->size() - 1; i++) {
        if(i) {
        } 
        if(i%segment_size == 0){
            if(i+segment_size > c->size()-1)
                segment = createBezier(&(*c)[i],&(*c)[c->size() - 1]);
            else
                segment = createBezier(&(*c)[i],&(*c)[i+segment_size]);
            for(int j = 0; j < 50; j++) {
                dvec3 point = segment.getPoint((float)j / 50);

                point_file << "v " << point.x << " "<< point.y << " "<< point.z << " " << endl;
            }
        }
    }


}