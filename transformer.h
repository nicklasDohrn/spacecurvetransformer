#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "transformer.h"
#include "types.h"

using namespace glm;
using namespace std;

class transformer 
{
   private:
        progConfig config;
   public: 
        transformer(progConfig conf) {
            config = conf;
        };
        void processPointsAsync();
        void joinAndWritePoints();
        void createAndWriteLightProfileAsync();
        void createTriangles();
        void writeTriangles();
};

#endif
