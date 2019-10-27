#ifndef OBJECTREADER_H
#define OBJECTREADER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "objectReader.h"
#include "types.h"

using namespace glm;
using namespace std;

class objectReader
{
    private:
    public:
        static int readTrianglesFromFile(std::string path);
        static int readPointsFromFile(std::string path);
        static vector<glm::dvec3>* getPoints();
        static vector<triangle>* getTriangles();
        static vector<vector<int>>* getTriangleLookup();
};

#endif
