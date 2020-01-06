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

using namespace std;
using namespace glm;

const int BUFFERSIZE = 10000;
int exportPoints = 1;
int stepsize = 1;
string standartParams = "./main.out ";

double shortestDistance(dvec3 a,vector<dvec3>* b, int* handle) {
    double shortest = INFINITY;
    double current = 0;
    for(int i = 0; i < b->size(); i++) {
        current = glm::distance(a, (*b)[i]);
        if(current < shortest) {
            *handle = i;
            shortest = current;
        }
    }
    return shortest;
}

int main  (int argc, char *argv[]) {
    string object = argv[1];
    string curve = argv[2];
    string threads = argv[3];
    string interpolationMode = argv[4];
    string approxMode = argv[5];
    int maxStepSize = atoi(argv[6]);
    string outfile = argv[7];
    standartParams = standartParams + object + " " + curve + " ";
    string base = standartParams + "base.cmp 1 " + threads + " 2 1 1";
    std::system(base.c_str());
    vector<vector<dvec3>>* b = cmpReader::readBaseFromFile("base.cmp");

    std::string buffer;
    buffer.reserve(BUFFERSIZE);
    vector<string> Values = vector<string>(10);
    ofstream missingFile;
    ofstream overheadFile;
    ofstream valuesFile;
    ofstream biggestPoints;
    valuesFile.open("benchmarks/" + outfile + ".cmp");
    biggestPoints.open("wrongPoints.obj");
    Values[0] = "maxDistFault :"; 
    Values[1] = "maxDistRelFault :"; 
    Values[2] = "(distSum/count) :"; 
    Values[3] = "(angleFaultSum/count) :"; 
    Values[4] = "maxXFault :"; 
    Values[5] = "maxXFaultDistance :"; 
    Values[6] = "maxXRelativeFault :"; 
    Values[7] = "(xSum/count) :"; 
    Values[8] = "missing% :"; 
    Values[9] = "overhead% :";
    if(exportPoints == 1) {
        missingFile.open("missing.obj");
        overheadFile.open("overhead.obj");  
    }

    for(int steps = stepsize; steps <= maxStepSize; steps += stepsize) {
        
        string target = standartParams + "target.cmp " + to_string(steps) + " " + threads + " " + interpolationMode + " " + approxMode +" 1";
        std::system(target.c_str());


        vector<vector<dvec3>>* b = cmpReader::readBaseFromFile("base.cmp");
        vector<vector<dvec3>>* t = cmpReader::readTargetFromFile("target.cmp");
        int count = 0;
        int missing = 0;
        int overhead = 0;
        double distSum = 0;
        double angleFaultSum = 0;
        double xSum = 0;
        double maxDistFault = 0;
        double maxRelDistFault = 0;
        int maxDistFaultI = 0;
        int maxDistFaultJ = 0;
        double maxXFault = 0;
        int maxXFaultI = 0;
        int maxXFautlJ = 0;
        double maxXFaultDistance = 0;
        double maxXRelativeFault = 0;
        cout << "still running" << endl;
        for(int i = 0; i < t->size(); i++) {
            for(int j = 0; j < (*t)[i].size(); j++) {
                int nearest = -1;
                if((*b)[i].size() > 0){
                    double dist = shortestDistance((*t)[i][j], &(*b)[i], &nearest);
                    if(dist > maxDistFault){
                        maxDistFault = dist;        
                        maxDistFaultI = i;
                        maxDistFaultJ = j;
                    }
                    if((dist/std::max(1.0,distance(dvec3((*t)[i][j].x, 0 , 0), (*t)[i][j]))) > maxRelDistFault){
                        maxRelDistFault = (dist/std::max(1.0,distance(dvec3((*t)[i][j].x, 0 , 0), (*t)[i][j])));      
                    }

                    distSum += dist;
                    angleFaultSum += dist/(distance(dvec3((*b)[i][nearest].x, 0 , 0), (*b)[i][nearest]));
                    double xDist = std::abs((*t)[i][j].x - (*b)[i][nearest].x);
                    xSum += xDist;
                    if(xDist > maxXFault) {
                        maxXFault = xDist;        
                        maxXFaultI = i;
                        maxXFautlJ = j;
                        maxXFaultDistance = distance(dvec3((*t)[i][j].x, 0 , 0), (*t)[i][j]);
                    }
                    if((xDist/std::max(1.0,distance(dvec3((*t)[i][j].x, 0 , 0), (*t)[i][j]))) > maxXRelativeFault) {
                        maxXRelativeFault = (xDist/std::max(1.0,distance(dvec3((*t)[i][j].x, 0 , 0), (*t)[i][j])));
                    }
                    (*b)[i].erase((*b)[i].begin() + nearest);
                } else {
                    overhead++;
                    if(exportPoints == 1)
                        overheadFile << "v " << to_string((*t)[i][j].x) << " " << to_string((*t)[i][j].y) << " " << to_string((*t)[i][j].z) << "\n" ;
                }
                count++;
            }
            if((*b)[i].size() > 0 && exportPoints == 1){
                for(int j = 0; j < (*b)[i].size(); j++) {
                    if (buffer.length() + 100 >= BUFFERSIZE)
                {
                    missingFile << buffer;
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
            missing += (*b)[i].size();
        }
        if(exportPoints == 1) {
            missingFile << buffer;
        }

        int i = maxDistFaultI;
        int j = maxDistFaultJ;
        cout << i << endl << endl;
        biggestPoints << "v " << to_string((*t)[i][j].x) << " " << to_string((*t)[i][j].y) << " " << to_string((*t)[i][j].z) << "\n" ;
        i = maxXFaultI;
        j = maxXFautlJ;
        biggestPoints << "v " << to_string((*t)[i][j].x) << " " << to_string((*t)[i][j].y) << " " << to_string((*t)[i][j].z) << "\n" ;

        Values[0] += "(" + to_string(steps) + "," + to_string(maxDistFault) + ")"; 
        Values[1] += "(" + to_string(steps) + "," + to_string(maxRelDistFault) + ")"; 
        Values[2] += "(" + to_string(steps) + "," + to_string((distSum/count)) + ")"; 
        Values[3] += "(" + to_string(steps) + "," + to_string((angleFaultSum/count)) + ")"; 
        Values[4] += "(" + to_string(steps) + "," + to_string(maxXFault) + ")"; 
        Values[5] += "(" + to_string(steps) + "," + to_string(maxXFaultDistance) + ")"; 
        Values[6] += "(" + to_string(steps) + "," + to_string(maxXRelativeFault) + ")"; 
        Values[7] += "(" + to_string(steps) + "," + to_string((xSum/count)) + ")"; 
        Values[8] += "(" + to_string(steps) + "," + to_string(((double)100 * missing/count)) + ")"; 
        Values[9] += "(" + to_string(steps) + "," + to_string(((double)100 * overhead/count)) + ")"; 
    }

    for(string value : Values) {
        valuesFile << value << endl;
    }
}
