#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono> 
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "writer.h"
#include "types.h"
#include "timer.h"


using namespace glm;
using namespace std;

ofstream file[3];

const int BUFFERSIZE = 100000;

void writer::openFile(string path, int fileType){
    file[fileType].open(path);
}

void writer::writeLightPoints(vector<vec3>* points, int fileType){
    std::string buffer;
    for(int i = 0; i < points->size(); i++) {
        if (buffer.length() + 100 >= BUFFERSIZE)
            {
                file[fileType] << buffer;
                buffer.resize(0);
            }
            buffer.append("v ");
            buffer.append(to_string((*points)[i].x));
            buffer.append(1, ' ');
            buffer.append(to_string((*points)[i].z));
            buffer.append(1, ' ');
            buffer.append(to_string(-(*points)[i].y));
            buffer.append(1, ' ');
            buffer.append(1, '\n');
    }
    buffer.append(1, '\n');
    file[fileType] << buffer;
}

void writer::writePoints(vector< vector<newPoint> >* points) {
    timer::startTimer("writingpoints");
    vector<newPoint>* point_segment;
    std::string buffer;
    buffer.reserve(BUFFERSIZE);
    cout << "writing points" << endl;
    for(int i=0; i < points->size(); i++){
        point_segment = &(*points)[i];
        for(int j = 0; j < point_segment->size(); j++) {
            if (buffer.length() + 100 >= BUFFERSIZE)
            {
                file[TRANSFORMATION] << buffer;
                buffer.resize(0);
            }
            buffer.append("v ");
            buffer.append(to_string((*point_segment)[j].vector.x));
            buffer.append(1, ' ');
            buffer.append(to_string((*point_segment)[j].vector.z));
            buffer.append(1, ' ');
            buffer.append(to_string(-(*point_segment)[j].vector.y));
            buffer.append(1, ' ');
            buffer.append(1, '\n');
        }
    }
    buffer.append(1, '\n');
    file[TRANSFORMATION] << buffer;
    timer::stopTimer("writingpoints");
}

void writer::writeTriangles(vector< vector<int> >* triangles) {
    cout << "writing triangles" << endl;
    std::string buffer;
    buffer.reserve(BUFFERSIZE);
    for(vector<int> newT : *triangles) {
            if (buffer.length() + 100 >= BUFFERSIZE)
            {
                file[TRANSFORMATION] << buffer;
                buffer.resize(0);
            }
        buffer.append(1, 'f');
        for(int i = 0; i < newT.size(); i++) {
            buffer.append(1, ' ');
            buffer.append(to_string(newT[i] + 1));   
        }
        buffer.append(1, '\n');
    }
    file[TRANSFORMATION] << buffer;
}

void writer::writeTimes() {
    ofstream timingFile;
    timingFile.open("timings.txt", std::ios_base::app);
    for(Info i : timer::getInfo()) {
        timingFile << i.name << ": " << i.value << endl;
    }
    for(Timing t : timer::getAllTimer()) {
        timingFile <<  t.name << " " << t.getTime() << endl;
    }
    timingFile << endl;
}

