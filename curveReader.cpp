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

vector<plane> *planes;

void insertPlane (const string &s, int pos) {
    double res[] = {0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,}; 
    int number = 0;
    bool sign = false;
    int pointPos = -1;
    int divider = 1;
    bool mantisse = false;
    int exponent = 0;

    for(int i = 0; i<s.length(); i++) {
        switch (s[i]) {
            case '-':
                sign = true;
                break;
            case 'E':
                mantisse = true;
                i++;
                break;
            case '\0':
            case '\r':
            case ';':
                if(sign)
                    res[number] = -res[number];
                if(pointPos != -1){
                    int end = 0;
                    if(mantisse){
                        end = i-5;
                    }else{
                        end = i-1;
                    }
                    for(int j = pointPos; j < end; j++) {
                        divider *= 10;
                    }
                    res[number] = res[number] / divider;
                    divider = 1;
                    pointPos = -1;
                }
                if(mantisse) {
                    for(int j = 0; j < exponent; j++) {
                        divider *= 10;
                    }
                    res[number] = res[number] / divider;
                    divider = 1;
                    exponent = 0;
                    mantisse = false;
                }
                number++;
                sign = false;
                break;
            case '.':
                pointPos = i;
                break;
            default:
                if(mantisse)
                    exponent = exponent * 10 + (s[i] - 48);
                else
                    res[number] = res[number] * 10 + (s[i] - 48);
        }
    }
    (*planes)[pos] = {
                (float)res[0],
                dvec3(res[1],res[2],res[3]),
                dvec3(res[4],res[5],res[6]),
                dvec3(res[7],res[8],res[9]),
                dvec3(res[10],res[11],res[12]),
            };
}

int curveReader::readFromFile(string path)
{
    string line_plane;
    ifstream line (path);
    bool firstline = true;
    int lines = 0;
    int linesIndex = 0;

    if (line.is_open()) {
        getline (line,line_plane);
        while ( getline (line,line_plane) ){
            lines++;
        }
        line.clear();
        line.seekg( 0, std::ios::beg );
        planes = new vector<plane>(lines);
        getline (line,line_plane);
        while ( getline (line,line_plane) ){
            insertPlane(line_plane, linesIndex++);
        }
    } else {cout << "did not find line" << endl; return -1;}
    return 0;
}

vector<plane>* curveReader::getPlanes() {
    return planes; 
}
