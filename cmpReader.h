//Reads curve.csv files to Plane Arrays
#ifndef CMPREADER_H
#define CMPREADER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "cmpReader.h"
#include "types.h"

using namespace glm;
using namespace std;

class cmpReader
{
    public:
        static vector<vector<dvec3>>* readBaseFromFile(std::string path);
        static vector<vector<dvec3>>* readTargetFromFile(std::string path);
};

#endif