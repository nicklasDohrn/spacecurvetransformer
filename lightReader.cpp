#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include "lightReader.h"
#include "types.h"

using namespace glm;
using namespace std;


profile *pro;

void insertPoint (const string &s) {
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

    pro->points.push_back(dvec3(res[0],-res[2],res[1]));
}
void insertLightTriangles (const string &s) { 
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
    pro->triangles.push_back(dvec3{
        res[0],
        res[1],
        res[2]
    });
}

int lightReader::readLightProfileFromFile(string path) {
    int pointsLength = 0;
    int trianglesLength = 0;
    string input_line;
    ifstream object (path);
    pro = new profile();
    if (object.is_open()) {
        while ( getline (object,input_line) ){
            if(input_line[0] == 'v' && input_line[1] == ' ')
                insertPoint(input_line);
            if(input_line[0] == 'f' && input_line[1] == ' ')
                insertLightTriangles(input_line);
        }
        object.close();
        cout << "read Profile" << endl;
    } else {cout << "did not find object" << endl; return -1;}
    return 0;
}

profile* lightReader::getProfile()
{
    return pro;
}
