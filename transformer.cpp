
#include <vector>
#include <unistd.h>
#include <vector>
#include <thread>
#include <sys/shm.h>
#include <sys/wait.h>
#include <malloc.h>
#include <stdlib.h>
#include <sstream>
#include <errno.h>
#include <error.h>
#include <math.h>
#include <glm/gtx/vector_angle.hpp>
#include "objectReader.h"
#include "curveReader.h"
#include "lightReader.h"
#include "transformer.h"
#include "writer.h"
#include "types.h"
#include "approx.h"
#include "timer.h"

#define SMALL_NUM   0.00000001

int *pageTablePosition;
int pagesPerThread;
int pageTableId;
int pageId;
int base_pid, pid;
int pageTableEntries;
int relative_pid;
int threadPages;
thread t[100];
thread writerThread;
thread lightThread;

vector< vector<newPoint> > pageTable;
vector<vector<int>> newTriangles;
vector<plane> *c;
vector<dvec3> *p;
vector<vec3> lightOldPoints;
vector<vec3> lightNewPoints;


double getDistancePlane(const dvec3 *point,const dvec3 *planePoint, const dvec3 *planeNormal) {
    return dot(*planeNormal, (*planePoint - *point));
}

dvec3 bezierAlignedQuadLerp(const plane *a,const plane *b, dvec3 target, double t, double xValue, dvec3 tangent) {
    //quaternion slerp to get correctly aproximated up and side vectors
    dmat3 aMat = mat3(a->normal.x, a->side.x, a->up.x,
                      a->normal.y, a->side.y, a->up.y,
                      a->normal.z, a->side.z, a->up.z);
    dmat3 bMat = mat3(b->normal.x, b->side.x, b->up.x,
                      b->normal.y, b->side.y, b->up.y,
                      b->normal.z, b->side.z, b->up.z);
    dquat aQuat = quat_cast(aMat);
    dquat bQuat = quat_cast(bMat);
    dmat3 tMat = inverse(mat3_cast(slerp(aQuat, bQuat, t)));

    //rotating coordinates towards tangent
    dvec3 side = normalize(cross(tMat[2],tangent));
    dvec3 up = -normalize(cross(side,tangent));

    //transforming coordinates
    double y = dot(side, target);
    double z = dot(up, target); 
    return dvec3(xValue, y, z);
}

dvec3 quadSlerp(const plane *a,const plane *b, dvec3 target, double t, double xValue) {
    dmat3 aMat = mat3(a->normal.x, a->side.x, a->up.x,
                      a->normal.y, a->side.y, a->up.y,
                      a->normal.z, a->side.z, a->up.z);
    dmat3 bMat = mat3(b->normal.x, b->side.x, b->up.x,
                      b->normal.y, b->side.y, b->up.y,
                      b->normal.z, b->side.z, b->up.z);
    dquat aQuat = quat_cast(aMat);
    dquat bQuat = quat_cast(bMat);
    dmat3 tMat = inverse(mat3_cast(slerp(aQuat, bQuat, t)));
    double y = dot(tMat[1], target);
    double z = dot(tMat[2], target);

            
    return dvec3(xValue, y, z);
}

double lerp(dvec3 a, dvec3 b, dvec3 target, double t) {
    return dot(a, target)*(1-t) + dot(b, target) * t;
};

double slerp(dvec3 a, dvec3 b, dvec3 target, double t) {
    double omega = acos(dot(a,b));
    if(isnan(omega))
        return lerp(a, b, target, t);
    dvec3 sVec = (sin((1-t)*omega)/sin(omega))*a+(sin(t*omega)/sin(omega))*b;
    double ret = dot(sVec, target);
    if(isnan(ret)){
        return lerp(a, b, target, t);
    }
    return ret;
}

void transformAndInsert(double abRatio, const plane *a,const plane *b, const dvec3 *p, cubicCurve* segment, int oldIndex, int direction, int threadId, int mode) {
            double shiftX = abRatio*(b->t -a->t);
            shiftX += a->t;
            dvec3 pOnCurve = segment->getPoint(abRatio);
            dvec3 tangentOnCurve = segment->getTangent(abRatio);
            double shiftY;
            double shiftZ;
            dvec3 temp;
            if(mode == 3) {
                temp = bezierAlignedQuadLerp(a, b, ((*p)-pOnCurve), abRatio, shiftX, tangentOnCurve);
            } else if(mode == 2) {
                temp = quadSlerp(a, b, ((*p)-pOnCurve), abRatio, shiftX);

            } else if(mode == 1){
                temp = dvec3(shiftX,
                        slerp(a->side, b->side, ((*p)-pOnCurve), abRatio),
                        slerp(a->up, b->up, ((*p)-pOnCurve), abRatio));
            } else {
                temp = dvec3(shiftX,
                        lerp(a->side, b->side, ((*p)-pOnCurve), abRatio),
                        lerp(a->up, b->up, ((*p)-pOnCurve), abRatio));
            }
            pageTable[threadId].push_back(newPoint{
                oldIndex,
                direction,
                dvec3(temp.x, temp.y, temp.z)
            });
}

cubicCurve createBezier(const plane *a, const plane *b) {
    double dist = (b->t - a->t) / 3;
    return {

            (dist * 3),
        {
            dvec3(a->point.x,a->point.y,a->point.z),
            dvec3(a->point.x + (dist * a->normal.x),
                  a->point.y + (dist * a->normal.y),
                  a->point.z + (dist * a->normal.z)),
            dvec3(b->point.x - (dist * b->normal.x),
                  b->point.y - (dist * b->normal.y),
                  b->point.z - (dist * b->normal.z)),
            dvec3(b->point.x,b->point.y,b->point.z)
        }
    };
}

void approxX (const dvec3* point, const double* t, cubicCurve* segment) {
    ////cout << getDistancePlane(point, segment->getPoint(*t), segment->getTangent(*t)) << "\n";
}

void checkArea (const plane *a,const plane *b, 
                const vector<dvec3> *points, int threadId, int interpolationMode, int approxMode) {     
    double t;
    const dvec3 *point;
    cubicCurve segment = createBezier(a,b);
    for(int i = 0; i < points->size(); i++) {
        point = &(*points)[i];
        double distA = getDistancePlane(point, &(a->point), &(a->normal));
        double distB = getDistancePlane(point, &(b->point), &(b->normal));
        if(distA>= 0 && distB < 0 || distA<= 0 && distB > 0){
            t = distA /(distA - distB);
            t = approx::approxT(point, &segment, t, approxMode);
            if(distA <= 0) {
                transformAndInsert(t, a, b, point, &segment, i, INVERTED, threadId, interpolationMode);
            } else {
                transformAndInsert(t, a, b, point, &segment, i, NORMAL, threadId, interpolationMode);
            }
        }
    }
}

void handleChunk(int id, int chunkSize, int precision, int interPolationMode, int approxMode) {
    for(int i = chunkSize * id; i < chunkSize * (id + 1); i++) {
        if(i%1000==0) {
            cout << "scanning Plane: " << (*c)[i].t << "t  (" 
                << (*c)[i].point.x << " " << (*c)[i].point.y << " " << (*c)[i].point.z << ")point  (" 
                << (*c)[i].normal.x << " " << (*c)[i].normal.y << " " << (*c)[i].normal.z << ")normal  (" 
                << (*c)[i].side.x << " " << (*c)[i].side.y << " " << (*c)[i].side.z << ")side  (" 
                << (*c)[i].up.x << " " << (*c)[i].up.y << " " << (*c)[i].up.z << ")up" << endl;
        }
        if(i%precision == 0 && (i+precision) >= c->size()) {
            checkArea(&((*c)[i]),&((*c)[c->size()-1]), p, id, interPolationMode, approxMode);
            break;
        }else if(i%precision == 0){
            checkArea(&((*c)[i]),&((*c)[i+precision]), p, id, interPolationMode, approxMode);
        }
    }
}

void prepareTriangles(const plane *start, vector<dvec3> *points, vector<triangle> *triangles, const vector<vector<int>> *triangle_lookup) {
    int pIndex = 0;
    for(dvec3 p : *points) {
        double side = getDistancePlane(&p, &(start->point), &(start->normal));
        for(int tIndex : (*triangle_lookup)[pIndex]){
            if(side < 0) {
                (*triangles)[tIndex].diff += INVERTED;
            } else {
                (*triangles)[tIndex].diff += NORMAL;
            }
        }
        pIndex++;
    }
    for(triangle t : *triangles) {
        if(t.diff % 3 == 0) {
            t.diff = 0;
        }
    }
}

void swap( int* x, int* y ) 
{
    if (x != y)
    {
        *x ^= *y;
        *y ^= *x;
        *x ^= *y;
    }
}

void addPointToTriangles(int page, int oldPointIndex, int newPointIndex, vector<triangle*> triangles) {
    for(triangle* t : triangles) {
        t->diff += pageTable[page][oldPointIndex].direction;
        int lastEntry = t->points.size() -1;
        if(lastEntry != -1 && (
            pageTable[t->pages[lastEntry]][t->points[lastEntry]].oldIndex == pageTable[page][oldPointIndex].oldIndex ||
           pageTable[t->pages[lastEntry]][t->points[lastEntry]].direction == pageTable[page][oldPointIndex].direction)) {
            t->points.push_back(oldPointIndex);
            t->newPoints.push_back(newPointIndex);
            t->pages.push_back(page);
        } else {
            t->points.insert(t->points.begin(), oldPointIndex);
            t->newPoints.insert(t->newPoints.begin(), newPointIndex);
            t->pages.insert(t->pages.begin(), page); 
        }
        bool invert = false;
        if(t->diff % 3 == 0) {
            if(t->points.size() == 3) {
                for(int i = 0; i < 3; i++) {
                    if(pageTable[t->pages[0]][t->points[0]].oldIndex == t->oldPoints[i] && 
                        pageTable[t->pages[1]][t->points[1]].oldIndex == t->oldPoints[(i+1) % 3]) {
                        if(t->diff > 0){
                            invert = true;
                            break;
                        }
                        break;
                    } else if(pageTable[t->pages[1]][t->points[1]].oldIndex == t->oldPoints[i] && 
                        pageTable[t->pages[0]][t->points[0]].oldIndex == t->oldPoints[(i+1) % 3]) {
                        if(t->diff < 0){
                            invert = true;
                            break;
                        }
                        break;
                    }
                }
            } else if(t->points.size() > 3) {
                int i;
                for(i = 0; i < t->points.size()-1; i++) {
                    if(pageTable[t->pages[i]][t->points[i]].oldIndex != pageTable[t->pages[i+1]][t->points[i+1]].oldIndex){
                        break;
                    }
                }

                for(int j = 0; j < 3; j++) {
                    if(pageTable[t->pages[i]][t->points[i]].oldIndex == t->oldPoints[j] && 
                        pageTable[t->pages[(i+1)]][t->points[(i+1)]].oldIndex == t->oldPoints[(j+1) % 3]) {
                        if(pageTable[t->pages[1]][t->points[1]].direction == 1){
                            invert = true;
                            break;
                        } else if(pageTable[t->pages[1]][t->points[1]].direction == 1)
                            break;
                    } else if (pageTable[t->pages[i+1]][t->points[i+1]].oldIndex == t->oldPoints[j] && 
                        pageTable[t->pages[(i)]][t->points[(i)]].oldIndex == t->oldPoints[(j+1) % 3]){
                        if(pageTable[t->pages[1]][t->points[1]].direction == -1){
                            invert = true;
                            break;
                        } else if(pageTable[t->pages[1]][t->points[1]].direction == -1)
                            break;
                    }
                }
            }
            if(t->points.size() >= 3){
                vector<int> temp;
                if(!invert){
                    for(int i = 0; i < t->newPoints.size(); i++){
                        temp.push_back(t->newPoints[i]);
                    }
                } else {
                    for(int i = t->newPoints.size()-1; i >= 0; i--){
                        temp.push_back(t->newPoints[i]);
                    }
                }
                newTriangles.push_back(temp);
            }
            t->diff = 0;
            t->points.clear();
            t->newPoints.clear();
            t->pages.clear();
        }
    }
}

void transformer::processPointsAsync(){
    timer::startTimer("processingPoints");
    c = curveReader::getPlanes();
    p = objectReader::getPoints();
    int N = p->size();
    int chunk;
    // Get the size of the work that each process will have to do.
    chunk = (c->size() / config.threads) + 1;
    printf("curve size %d\n", c->size());
    printf("using %d threads for %d points with precision %d \n", config.threads, N, config.samplingStep);
    timer::setInfo("curve size", c->size());
    timer::setInfo("threads", config.threads);
    timer::setInfo("segmentlength", config.samplingStep);
    timer::setInfo("points", N);
    pageTableEntries = config.threads;
    
    //create pagetable and threadpositions
    pageTable.resize(pageTableEntries);

    //create threads
    for (int i = 0; i < config.threads; i++) {
        t[i] = thread(handleChunk, i, chunk, config.samplingStep, config.interpolationMode, config.approxMode);
    }
}

void transformer::joinAndWritePoints() {
    for (int i = 0; i < config.threads; i++) {
        t[i].join();         
    }
    timer::stopTimer("processingPoints");
    if(config.writeMode == 0)
        writerThread = thread(writer::writePoints, &pageTable);
    else
        writer::writePoints(&pageTable);
}

void transformer::createTriangles() {

    timer::startTimer("createTriangles");
    vector<plane> *c = curveReader::getPlanes();
    vector<dvec3> *p = objectReader::getPoints();
    vector<triangle> *tri = objectReader::getTriangles();
    vector<vector<int>> *l = objectReader::getTriangleLookup();
    vector<triangle*> triangles_by_point;
    prepareTriangles(&(*c)[0],p,tri,l);
    int pointIndex = 0;
    for(int i = 0; i < pageTable.size(); i++){
        for(int j = 0; j < pageTable[i].size(); j++){
            triangles_by_point.clear();
            for(int k = 0; k < (*l)[pageTable[i][j].oldIndex].size(); k++) {
                triangles_by_point.push_back(&((*tri)[(*l)[pageTable[i][j].oldIndex][k]]));
            }

            addPointToTriangles(i, j, pointIndex++, triangles_by_point);
        
        }
    }
    timer::stopTimer("createTriangles");
}

void transformer::writeTriangles() {
    writerThread.join();
    auto start = std::chrono::high_resolution_clock::now();
    writer::writeTriangles(&newTriangles);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    //cout << "writing triangles took: " << elapsed.count() << endl;
    t[0].join();
}

float sign (vec3 p1, vec3 p2, vec3 p3)
{
    return (p1.y - p3.y) * (p2.z - p3.z) - (p2.y - p3.y) * (p1.z - p3.z);
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

void calculateLightroom(profile *pro) {
    int oldFound[p->size()];
    vector<newPoint>* point_segment;
    for(int i=0; i < pageTable.size(); i++){
        point_segment = &pageTable[i];
        for(int j = 0; j < point_segment->size(); j++) {
            for(int k = 0; k < pro->triangles.size(); k++) {
                if(pointInTriangle((*point_segment)[j].vector, 
                                    pro->points[pro->triangles[k][0]],
                                    pro->points[pro->triangles[k][1]],
                                    pro->points[pro->triangles[k][2]])){
                                        
                    lightNewPoints.push_back((*point_segment)[j].vector);
                    if(!oldFound[(*point_segment)[j].oldIndex]){
                        lightOldPoints.push_back((*p)[(*point_segment)[j].oldIndex]);
                    }
                }
            }
        }
    }
    writer::writeLightPoints(&lightNewPoints,LIGHT_TRANSFORMED);
    writer::writeLightPoints(&lightOldPoints,LIGHT_ORIGINAL);
    cout << "finished profile" << endl;
}

void transformer::createAndWriteLightProfileAsync() {
    profile *p = lightReader::getProfile();
    t[0] = thread(calculateLightroom, p);
}
