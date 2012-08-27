//
//  ofxSweepLine.h
//  an implementation of the Bentley–Ottmann line 
//  intersection algorithm for openFrameworks
//
//  Created by Alex Wolfe @ the Studio for Creative Inquiry
//

#pragma once
#include "ofMain.h"
#include <math.h>
#include <list>
#include <set>
#include <map>


class LineSeg{

public: 
    float getYCoord(int time);
    int pIndex; //index into the polyline vector
    int edge; // p[i] to p[i+1]
    ofVec2f left; //leftmost point
    ofVec2f right; //rightmost point
    int yCoord;
};

class EventPoint{
    public:
    enum Type{
        RIGHT,
        LEFT,
        INTERSECTION
    };
        Type type;
        int pIndex; //index of original polyline in the vector
        int edge; // p[i] to p[i+1]
        int lIndex; //index into LineSeg vector 
        ofVec2f point; //event Vertex
        pair<LineSeg, LineSeg> intersection;
        bool pointSorter(ofVec2f p1, ofVec2f p2);
        bool eventPointSorter(EventPoint a, EventPoint b);
        void addUpper(LineSeg upper);
        void addLower(LineSeg lower);
        void printPoint();
};

class EventQueue {        
    public:
        struct EventPointCompare{
            bool operator()(const EventPoint& l, const EventPoint& r) const
            {
                if(l.point.x < r.point.x) return true;
                if(l.point.x > r.point.x) return false;
                if(l.point.y < r.point.y) return true;
                if(l.point.y > r.point.y) return false;
            }
        };
        std::set<EventPoint, EventPointCompare> pointSet;
        EventQueue();
        EventQueue(vector<ofPolyline> p);
        vector<LineSeg> lines;
        void sanityTest();

};

class ofxSweepLine{
public:
//    class lineSegCompare { // a more complicated comparison function which varies according to specified parameter s
//    public:
//        lineSegCompare(int t_) : t(t_) {};		
//        bool operator()(LineSeg x, LineSeg y) { 
//            return x.getYCoord(t) < y.getYCoord(t); 
//        } // returns x>y if s>0, else x<y
//    private:
//        int t;
//    };
    static float sweepPos;
    
    struct lineSegCompare {
        bool operator() (const LineSeg& x, const LineSeg& y) const
        {return x.yCoord < y.yCoord; }
    };
    std::set<LineSeg, lineSegCompare> sweptLines;
    std::set<ofVec2f> intersections;
    EventQueue eq;
    
    
    ofxSweepLine();
   // ~ofxSweepLine();
    ofxSweepLine(vector<ofPolyline> p);
    
    vector<ofVec2f> sweep(vector<ofPolyline> p);
    bool testIntersect(ofVec2f a1, ofVec2f a2, ofVec2f b1, ofVec2f b2, ofVec2f &i);
    double perpDot(ofVec2f a , ofVec2f b);
    void test();
    
private:       
    float isLeft(ofVec2f p0, ofVec2f p1, ofVec2f p2);
    
};

//
//class RBNode{
//    friend class RBTree;
//public:
//    RBNode();
//    RBNode(LineSeg* newEntry);
//    LineSeg * getLine();
//protected:
//    LineSeg * line;
//    int key;
//    bool red;
//    RBNode * left;
//    RBNode * right;
//    RBNode * parent;
//    
//};
//class RBTree {
//public:
//    RBTree();
//    ~RBTree();
//    LineSeg * deleteNode(RBNode *);
//    RBNode * insert(LineSeg *);
//    RBNode * getPred(RBNode *);
//    RBNode * getSucc(RBNode *);
//    RBNode * find(int key);
//    
//protected:
//    RBNode *root;
//    RBNode *nilNode;
//    void leftRotate(RBNode *);
//    void rightRotate(RBNode *);
//    void insertHelper(RBNode *);
//    void deleteFixUp(RBNode *);
//};
