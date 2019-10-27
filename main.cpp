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
#include "objectReader.h"
#include "curveReader.h"
#include "transformer.h"
#include "timer.h"
#include "writer.h"

using namespace std;
using namespace glm;

int defaultPrecision = 1;
int defaultThreads = 8;
int defaultInterpolationMode = 2;
int defaultWriteMode = 0;
int defaultxApprox = 1;



int main  (int argc, char *argv[]) {
    int samplingStep;
    int threads;
    int interpolationMode;
    int approxMode;
    int writeMode;
    timer::startTimer("gesamt");
    if(argc < 8) {
        cout << "missing at least one special argument, therefore running with default values" << endl;
        samplingStep = defaultPrecision;
        threads = defaultThreads;
        interpolationMode = defaultInterpolationMode;
        approxMode = defaultxApprox;
        writeMode = defaultWriteMode;
    } else { 
        samplingStep = atoi(argv[4]);
        threads = atoi(argv[5]);
        interpolationMode = atoi(argv[6]);
        approxMode = atoi(argv[7]);
        writeMode = atoi(argv[8]);
    }

    if(argc < 4) {
        //cout << "missing arguments, at least provide: executable object curve outfile" << endl;
        return -1;
    }
    timer::startTimer("readingCurve");
    curveReader::readFromFile(argv[2]); 
    timer::stopTimer("readingCurve");
    vector<plane> *c = curveReader::getPlanes();

    timer::startTimer("readingPoints");
    objectReader::readPointsFromFile(argv[1]); 
    timer::stopTimer("readingPoints");
    vector<dvec3> *p = objectReader::getPoints();
    transformer trans;
        
    /*3: out file, 7: write mode
        write modes:
            0: obj format
    *///        1: comparable format
    writer::openFile(argv[3], writeMode);
    trans = transformer();
    /*  4: threads, 5: sampling step size, 6: interpolation mode
        Interpolation modes: 
            0: coordinate wise lerp
            1: coordinate wise slerp
    *///    2: quaternion lerp (guaranteed orthogonal interpolations))
    trans.processPointsAsync(threads,samplingStep,interpolationMode, approxMode);
    if(writeMode == 0) {
        timer::startTimer("readingTriangles");
        objectReader::readTrianglesFromFile(argv[1]);
        timer::stopTimer("readingTriangles");
    }
    
    trans.joinAndWritePoints(writeMode);

    if(writeMode == 0) {
        trans.createTriangles();
        timer::startTimer("writeTriangles");
        trans.writeTriangles();
        timer::stopTimer("writeTriangles");
    }
    timer::stopTimer("gesamt");
    writer::writeTimes();

    return 0;
}
