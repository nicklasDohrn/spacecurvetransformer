#ifndef WRITER_H
#define WRITER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "types.h"

using namespace glm;
using namespace std;

class writer 
{

    public: 
        //opens File, options are: "transformation, light_transformed, light_original"
        //make sure to open a file for the 
        static void openFile(string path, int fileType);
        //writes points for the transformed model
        static void writePoints(vector< vector<newPoint> >* points);
        //writes triangles for the transformed model
        static void writeTriangles(vector< vector<int> >* triangles);
        //writes lightroom intersection points for the original model
        static void writeOldPoints(vector<vec3>* points);
        //writes lightroom intersection points for the transformed model
        static void writeLightPoints(vector<vec3>* points, int mode);
        static void writeTimes();
        static void close();
};

#endif
