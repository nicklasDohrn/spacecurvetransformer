#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "objectReader.h"
#include "types.h"

using namespace glm;
using namespace std;

int pointsLength = 0;
int trianglesLength = 0;

int triangle_index = -1;

vector<dvec3> *points;
vector<vector<int>> triangle_lookup;
vector<triangle> triangles;

void insertPoint (const string &s, int pos) {
    double res[] = {0,0,0}; 
    int number = 0;
    bool sign = false;
    for(int i = 2; i<s.length() + 1; i++) {
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
                break;
            case '.':
                break;
            default:
                res[number] = res[number] * 10 + (s[i] - 48);
        }
    }

    (*points)[pos] = dvec3(res[0],-res[2],res[1]);
}
void insertTriangles (const string &s) { 
    int res[] = {0,0,0}; 
    int number = 0;
    int point = true;
    bool sign = false;
    for(int i = 2; i<s.length(); i++) {
        switch (s[i]) {
            case ' ':
                number++;
                point = true;
                break;
            case '/':
                point = false;
                break;
            default:
                if(point)
                    res[number] = res[number] * 10 + (s[i] - 48);
        }
    }
    //offset to indices starting with 0
    for(int i = 0; i < 3; i++) {
        res[i] -= 1;
    }

    triangle_index++;
    for(int i = 0; i < 3; i++) {
        triangles[triangle_index].oldPoints[i] = res[i];
        triangle_lookup[res[i]].push_back(triangle_index);
    }
}

int objectReader::readPointsFromFile(string path) {
    string input_line;
    ifstream object (path);
    if (object.is_open()) {
        while ( getline (object,input_line) ){
            if(input_line[0] == 'v' && input_line[1] == ' ') {
                pointsLength++;
            } else if (input_line[0] == 'f' && input_line[1] == ' ') {
                break;
            }
        }
        object.clear();
        object.seekg( 0, std::ios::beg );
        points = new vector<dvec3>(pointsLength);
        int pointsIndex = 0;
        while ( getline (object,input_line) ){
            if(input_line[0] == 'v' && input_line[1] == ' ') {
                insertPoint(input_line, pointsIndex++);
            } else if(input_line[0] == 'f' && input_line[1] == ' ')  {
                break;
            }
        }
        object.close();
        cout << "read Points" << endl;
    } else {cout << "did not find object" << endl; return -1;}
    return 0;
}

int objectReader::readTrianglesFromFile(string path)
{
    
    string input_line;
    ifstream object (path);
    if (object.is_open()) {
        while ( getline (object,input_line) ){
            if (input_line[0] == 'f' && input_line[1] == ' ') {
                trianglesLength++;
            }
        }
        object.clear();
        object.seekg( 0, std::ios::beg );
        triangles.resize(trianglesLength);
        triangle_lookup.resize(pointsLength);
        while ( getline (object,input_line) ){
            if(input_line[0] == 'f' && input_line[1] == ' ')  {
                insertTriangles(input_line);
            }
        }
        object.close();
        cout << "read triangles" << endl;
    } else {cout << "did not find object" << endl; return -1;}
    return 0;
}

vector<dvec3>* objectReader::getPoints()
{
    return points;
}

vector<triangle>* objectReader::getTriangles()
{
    return &triangles;
}

vector<vector<int>>* objectReader::getTriangleLookup()
{
    return &triangle_lookup;
}
