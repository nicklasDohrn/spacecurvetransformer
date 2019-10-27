//Reads curve.csv files to Plane Arrays
#ifndef LINEREADER_H
#define LINEREADER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "curveReader.h"
#include "types.h"

using namespace glm;
using namespace std;

class curveReader
{
    public:
        static int readFromFile(std::string path);
        static vector<plane>* getPlanes();
};

#endif
