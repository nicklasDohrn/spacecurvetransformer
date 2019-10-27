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

ofstream outFile;

const int BUFFERSIZE = 100000;
int writeMode;

void writer::openFile(string path, int mode){
    outFile.open(path);
    writeMode = mode;
}

void writer::writePoints(vector< vector<newPoint> >* points) {
    timer::startTimer("writingpoints");
    vector<newPoint>* point_segment;
    std::string buffer;
    buffer.reserve(BUFFERSIZE);
    if(writeMode == 1) {
        vector<vector<dvec3>> orderedPoints = vector<vector<dvec3>>();
        for(int i=0; i < points->size(); i++){
            point_segment = &(*points)[i];
            for(int j = 0; j < point_segment->size(); j++) {
                if((*point_segment)[j].oldIndex >= orderedPoints.size()) {
                    orderedPoints.resize((*point_segment)[j].oldIndex + 1);
                }
                orderedPoints[(*point_segment)[j].oldIndex].push_back(dvec3((*point_segment)[j].vector.x,(*point_segment)[j].vector.y,(*point_segment)[j].vector.z));
            }
        }
        cout << "ordered points" << endl;

            buffer.append(to_string(orderedPoints.size()));
            buffer.append(1, '\n');
        for(int i=0; i < orderedPoints.size(); i++){
            buffer.append(to_string(i));
            buffer.append(1, ' ');
            for(int j = 0; j < orderedPoints[i].size(); j++) {
                if (buffer.length() + 100 >= BUFFERSIZE)
                {
                    outFile << buffer;
                    buffer.resize(0);
                }
                buffer.append(to_string(orderedPoints[i][j].x));
                buffer.append(1, ' ');
                buffer.append(to_string(orderedPoints[i][j].z));
                buffer.append(1, ' ');
                buffer.append(to_string(-orderedPoints[i][j].y));
                buffer.append(1, ' ');
            }
            buffer.append(1, '\n');
        }
        outFile << buffer;
        timer::stopTimer("writingpoints");
        outFile.close();
        return;
    }
    cout << "writing points" << endl;
    for(int i=0; i < points->size(); i++){
        point_segment = &(*points)[i];
        for(int j = 0; j < point_segment->size(); j++) {
            if (buffer.length() + 100 >= BUFFERSIZE)
            {
                outFile << buffer;
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
    outFile << buffer;
    timer::stopTimer("writingpoints");
}

void writer::writeTriangles(vector< vector<int> >* triangles) {
    if(writeMode != 0) {
        cout << "omitting triangles in comparable mode" << endl;
        return;
    }
    cout << "writing triangles" << endl;
    std::string buffer;
    buffer.reserve(BUFFERSIZE);
    for(vector<int> newT : *triangles) {
            if (buffer.length() + 100 >= BUFFERSIZE)
            {
                outFile << buffer;
                buffer.resize(0);
            }
        buffer.append(1, 'f');
        for(int i = 0; i < newT.size(); i++) {
            buffer.append(1, ' ');
            buffer.append(to_string(newT[i] + 1));   
        }
        buffer.append(1, '\n');
    }
    outFile << buffer;
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

