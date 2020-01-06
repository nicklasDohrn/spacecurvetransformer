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

vector<vector<dvec3>> *cmpPointsBase;
vector<vector<dvec3>> *cmpPointsTarget;

void insertPoints (const string &s, vector<vector<dvec3>>* points) {
    double res[] = {0,0,0}; 
    int number = 0;
    bool sign = false;
    int i;
    int oldIndex = 0;
    for(i = 0; s[i] != ' '; i++) {
        oldIndex = oldIndex*10 + (s[i] - 48);
    }
    i++;

    for(i = i; i<s.length() + 1; i++) {
        switch (s[i]) {
            case '-':
                sign = true;
                break;
            case ' ':
            case '\0':
                if(sign)
                    res[number] = -res[number];
                res[number] = res[number] / 1000000.0;
                number++;
                sign = false;
                if(number % 3 == 0) {
                    (*points)[oldIndex].push_back(dvec3(res[0],res[1],res[2]));
                    res[0] = 0;
                    res[1] = 0;
                    res[2] = 0;
                    number = 0;
                }
                break;
            case '.':
                break;
            default:
                res[number] = res[number] * 10 + (s[i] - 48);
        }
    }
}

vector<vector<dvec3>>* cmpReader::readBaseFromFile(string path)
{
    string line;
    ifstream stream (path);
    bool firstline = true;
    int lines = 0;

    if (stream.is_open()) {
        cout << "reading cmpFile" << endl;
        getline (stream,line);
        int oldPointsCount = stoi(line);

        cout << "cmpPointsFound: " << oldPointsCount << endl;
        cmpPointsBase = new vector<vector<dvec3>>(oldPointsCount);
        while (getline (stream,line) ){
            insertPoints(line, cmpPointsBase);
        }
    }
    return cmpPointsBase;
}

vector<vector<dvec3>>* cmpReader::readTargetFromFile(string path)
{
    string line;
    ifstream stream (path);
    bool firstline = true;
    int lines = 0;

    if (stream.is_open()) {
        cout << "reading cmpFile" << endl;
        getline (stream,line);
        int oldPointsCount = stoi(line);

        cout << "cmpPointsFound: " << oldPointsCount << endl;
        cmpPointsTarget = new vector<vector<dvec3>>(oldPointsCount);
        while (getline (stream,line) ){
            insertPoints(line, cmpPointsTarget);
        }
    }
    return cmpPointsTarget;
}
