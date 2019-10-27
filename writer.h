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
        static void openFile(string path, int mode);
        static void writePoints(vector< vector<newPoint> >* points);
        static void writeTriangles(vector< vector<int> >* triangles);
        static void writeTimes();
        static void close();
};

#endif
