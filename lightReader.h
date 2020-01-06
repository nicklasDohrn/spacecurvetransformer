#ifndef LIGHTREADER_H
#define LIGHTREADER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "lightReader.h"
#include "types.h"

using namespace glm;
using namespace std;

class lightReader
{
    private:
    public:
        static int readLightProfileFromFile(std::string path);
        static profile* getProfile();
};

#endif
