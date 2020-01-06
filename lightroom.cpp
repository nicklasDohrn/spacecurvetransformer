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
#include "cmpReader.h"
#include "objectReader.h"

using namespace std;
using namespace glm;

const int BUFFERSIZE = 10000;
int exportPoints = 1;
int stepsize = 1;
string standartParams = "./main.out ";

float sign (vec3 p1, vec3 p2, vec3 p3)
{
    return (- p1.z - p3.y) * (p2.z - p3.z) - (p2.y - p3.y) * (p1.y - p3.z);
}

bool pointInTriangle (vec3 pt, vec3 v1, vec3 v2, vec3 v3)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

int main  (int argc, char *argv[]) {
    string object = argv[1];
    string curve = argv[2];
    string threads = argv[3];
    string lightProfile = argv[4];
    string outfile = argv[5];
    string origPoints = argv[6];
    standartParams = standartParams + object + " " + curve + " ";
    string base = standartParams + "base.cmp 8 " + threads + " 2 1 1";
    std::system(base.c_str());
    vector<vector<dvec3>>* b = cmpReader::readBaseFromFile("base.cmp");
    cout << "read all" << endl;
    std::string buffer;
    buffer.reserve(BUFFERSIZE);
    ofstream valuesFile;
    valuesFile.open(outfile);
    objectReader::readPointsFromFile(lightProfile);
    objectReader::readTrianglesFromFile(lightProfile);
    vector<dvec3>* tPoints = objectReader::getPoints();
    vector<triangle>* tTriangles = objectReader::getTriangles();
    
    bool found = false;
    vector<int> oldPoints = vector<int>();
    for(int i = 0; i < b->size(); i++) {
        for(int j = 0; j < (*b)[i].size(); j++) {
            for(int k = 0; k < tTriangles->size(); k++){
                if(pointInTriangle((*b)[i][j], 
                                    (*tPoints)[(*tTriangles)[k].oldPoints[1]],
                                    (*tPoints)[(*tTriangles)[k].oldPoints[2]],
                                    (*tPoints)[(*tTriangles)[k].oldPoints[0]])){
                        
                        found = true;

                        if (buffer.length() + 100 >= BUFFERSIZE)
                        {
                            valuesFile << buffer;
                            buffer.resize(0);
                        }
                        buffer.append("v ");
                        buffer.append(to_string((*b)[i][j].x));
                        buffer.append(1, ' ');
                        buffer.append(to_string((*b)[i][j].y));
                        buffer.append(1, ' ');
                        buffer.append(to_string((*b)[i][j].z));
                        buffer.append(1, ' ');
                        buffer.append(1, '\n');
                }
            }
        }

        if(found){
            oldPoints.push_back(i);
            found = false;
        }
    }
    valuesFile << buffer;
    buffer.resize(0);
    objectReader::readPointsFromFile(object);
    tPoints = objectReader::getPoints();
    ofstream orig;
    orig.open(origPoints);
    for(int i = 0; i < oldPoints.size(); i++) {

        if (buffer.length() + 100 >= BUFFERSIZE)
        {
            orig << buffer;
            buffer.resize(0);
        }
        cout << "writing old point" << endl;
        buffer.append("v ");
        buffer.append(to_string((*tPoints)[oldPoints[i]].x));
        buffer.append(1, ' ');
        buffer.append(to_string((*tPoints)[oldPoints[i]].z));
        buffer.append(1, ' ');
        buffer.append(to_string(-(*tPoints)[oldPoints[i]].y));
        buffer.append(1, ' ');
        buffer.append(1, '\n');
    }
    orig << buffer;
    buffer.resize(0);
}
