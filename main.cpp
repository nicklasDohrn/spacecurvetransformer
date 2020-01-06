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
#include "lightReader.h"
#include "lightReader.h"
#include "transformer.h"
#include "timer.h"
#include "writer.h"
#include "types.h"

using namespace std;
using namespace glm;

progConfig conf;

int handleArguments(int argc, char *argv[]) {

    //Printing help message
    if( argc < 2 || (string)argv[1] == "-h" || (string)argv[1] == "--help" ) {
        cout << "common use:" << endl;
        cout << "./executable <object> <curve> || ./executable <object> <curve> <profile>" << endl;
        cout << "___________" << endl;
        cout << "you can also specify special arguments to change the default settings:" << endl;
        cout << "-t (--threads)         | number of threads used, can be 1 to 100, example: -t 13" << endl;
        cout << "-s (--sampling-steps)  | the number of points skipped between two segments, example: -s 2" << endl;
        cout << "-o (--outfile)  | the file to put the result into: -o ./out/custom_out.obj" << endl;
        cout << "-n (--lightfile-normal)  | the file to put the lightroom for the original into: -n ./out/custom_light.obj" << endl;
        cout << "-l (--lightfile-transformed)  | the file to put the lightroom for the trasnformation into: -l ./out/custom_light.obj" << endl;
        return -1;
    }

    //argument handling
    bool valid = false;
    int argCount = 0;
    for(int i=1; i < argc; i++) {
        string arg = (string)argv[i];
        if ( arg == "-t" || arg == "--threads" ) {
            conf.threads = stoi(argv[++i]);
        } else if (arg == "-s" || arg == "--sampling-steps" ) {
            conf.samplingStep = stoi(argv[++i]);
        } else if (arg == "-o" || arg == "--outfile" ) {
            conf.outFile = argv[++i];
        } else if (arg == "-n" || arg == "--lightfile-normal" ) {
            conf.lightOrigFile = argv[++i];
        } else if (arg == "-l" || arg == "--lightfile-transformed" ) {
            conf.lightTransFile = argv[++i];
        } else {
            if(argCount == 0) {
                conf.object = argv[i];
            } else if(argCount == 1) {
                conf.curve = argv[i];
            } else if(argCount == 2) {
                conf.profile = argv[i];
            } 
            argCount++;
        }
    }
    //decision, which mode to use
    if(argCount == 2) {
        cout << "normal mode, to enable lightroom checking, provide profile file." << endl;
        return mode::DEFAULT;
    } else if(argCount == 3) {
        cout << "enabled lightroom checking" << endl;
        return mode::LIGHTPROFILE;
    } else if (argCount > 3) {
        cout << "to many arguments, make sure u used the correct flags for your arguments, see -h for more info" << endl;
        return mode::ABORT;
    }
    cout << "to few arguments, see -h for more info" << endl;
    return mode::ABORT;
}

void setDefaultValues() {
    conf.approxMode = 1;
    conf.interpolationMode = 2;
    conf.threads = 8;
    conf.samplingStep = 16;
    conf.writeMode = 0;
    conf.outFile = "transformed.obj";
    conf.lightOrigFile = "light_original.obj";
    conf.lightTransFile = "light_transformed.obj";
}



int main  (int argc, char *argv[]) {
    setDefaultValues();

    int transformation_mode = handleArguments(argc, argv);

    if (transformation_mode == mode::ABORT) return -1;
    
    //reading all items for point processing
    curveReader::readFromFile(conf.curve); 
    objectReader::readPointsFromFile(conf.object); 

    //preopening output files to process them faster async
    writer::openFile(conf.outFile,TRANSFORMATION);
    if(transformation_mode == mode::LIGHTPROFILE) {
        writer::openFile(conf.lightTransFile,LIGHT_TRANSFORMED);
        writer::openFile(conf.lightOrigFile,LIGHT_ORIGINAL);
    }
    transformer t = transformer(conf);

    t.processPointsAsync();

    //reading triangle data while handling points
    objectReader::readTrianglesFromFile(conf.object);

    //writing point data (async writes)
    t.joinAndWritePoints();
    if(transformation_mode == mode::LIGHTPROFILE) {
        lightReader::readLightProfileFromFile(conf.profile);
        t.createAndWriteLightProfileAsync();
    }

    //handling triangles
    t.createTriangles();
    t.writeTriangles();

    writer::writeTimes();

    return 0;
}