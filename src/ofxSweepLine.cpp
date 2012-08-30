//
//  ofxSweepLine.cpp
//  an implementation of the Bentley–Ottmann line 
//  intersection algorithm for openFrameworks
//
//  by Alex Wolfe @ the Studio for Creative Inquiry

#include <iostream>
#include <math.h>
#include "ofxSweepLine.h"

LineSeg::LineSeg(){}
LineSeg::LineSeg(int i, int e, ofVec2f l, ofVec2f r){
    pIndex = i;
    edge = e;
    left = l;
    right = r;
    yCoord = getYCoord(l.x);
}

float LineSeg::getYCoord(int time){
    float slope = (left.y-right.y)/(left.x-right.x);
    yCoord = slope*(time-left.x)+left.y;
    return yCoord;
}
/*---------------------------------------------*/
EventPoint::EventPoint(){
}
EventPoint::EventPoint(Type t, ofVec2f p, LineSeg i1, LineSeg i2, int pI, int lI, int e){
    type = t;
    point = p;
    intersection.first = i1;
    intersection.second = i2;
    pIndex = pI;
    lIndex = lI;
    edge = e;
}
bool EventPoint::pointSorter(ofVec2f a, ofVec2f b){
    if(a.x > b.x) return true;
    if(a.x < b.x) return false;
    if(a.y > b.y) return true;
    if(a.y < b.y) return false;
}
bool EventPoint::eventPointSorter(EventPoint a, EventPoint b){
    ofVec2f pA = a.point;
    ofVec2f pB = b.point;
    return(pointSorter(pA, pB));
}

void EventPoint::printPoint(){
    cout << this->type << " " << this->point<< " " << this->pIndex<< " " << this->edge <<endl;
}
/*---------------------------------------------*/

EventQueue::EventQueue(){}
EventQueue::EventQueue(vector<ofPolyline> p){
    //pull out all the endpoints of the line seqments
    //and put into the queue.
    
    //the problem when dealing with polylines, is that
    //the aren't intrinsically straight, usually they're
    //not. treat each segment of the polyline as a seperate
    //line.
    
    for(int i=0; i<p.size(); i++){
        ofPolyline line = p[i];
        for(int j=0; j<line.getVertices().size()-1; j++){
            ofVec2f l = p[i][j];
            ofVec2f r = p[i][j+1];
            if(!(l.x < r.x)){
                ofVec2f tmp = l;
                l = r;
                r = tmp;
            }
            EventPoint lPoint, rPoint;
            LineSeg newLine;
            
            lPoint.type = EventPoint::LEFT;
            lPoint.pIndex = i;
            lPoint.edge = j;
            lPoint.point = l;
            
            rPoint.type = EventPoint::RIGHT;
            rPoint.pIndex = i;
            rPoint.edge = j;
            rPoint.point = r;
            
            newLine.pIndex = i;
            newLine.edge = j;
            newLine.left = l;
            newLine.right = r;
            newLine.yCoord = newLine.getYCoord(lPoint.point.x);
            newLine.lIndex = lines.size();
            lines.push_back(newLine);
            
            
            rPoint.lIndex = lines.size()-1;
            lPoint.lIndex = lines.size()-1;
            
            pointSet.insert(lPoint);
            pointSet.insert(rPoint);
        }
    }
}
void EventQueue::sanityTest(){
    set<EventPoint>::iterator it;
    cout<< "ordered point set" << endl;
    for ( it=pointSet.begin() ; it != pointSet.end(); it++ ){
        cout << it->type << " " << it->point<< " " << it->pIndex<< " " << it->edge <<endl;
    }
}
/*---------------------------------------------*/


ofxSweepLine::ofxSweepLine(vector<ofPolyline> p){
    eq = *new EventQueue(p);
}
ofxSweepLine::ofxSweepLine(){
}
void ofxSweepLine::test(){
    set<LineSeg, lineSegCompare>::iterator itr;
    cout<<"printing tree"<<endl;
    for (itr=sweptLines.begin(); itr!=sweptLines.end(); itr++)
        cout << (*itr).pIndex << " " << (*itr).edge << " " <<(*itr).yCoord << endl;
}
vector<ofVec2f> ofxSweepLine::sweep(vector<ofPolyline> p){
    eq = *new EventQueue(p); //based on b-tree, auto sorted    
    //  sweptLines = *new std::set<LineSeg, lineSegCompare>; 
    
    pair<set<LineSeg, lineSegCompare>::iterator,bool> inserted;
    set<LineSeg, lineSegCompare>::iterator itr;
    vector<ofVec2f> ret;
    
    while(!eq.pointSet.empty()){   //While (x is nonempty) {
        EventPoint e = *eq.pointSet.begin(); //get next Event
        ofVec2f i;
        if(e.type == EventPoint::LEFT){
            int above, below;
            LineSeg curLine = eq.lines[e.lIndex];
            //DEBUG:: check to see if inserted.second is true;
            inserted = sweptLines.insert(curLine);
            itr = inserted.first;
            test();
            if(itr != sweptLines.begin()){
                cout << (itr--)->yCoord << endl;
                above = (itr--)->lIndex;
            }
            else 
                above = -1;
            if(itr++ != sweptLines.end())
                below = (itr++)->lIndex;
            else
                below = -1;
            
            if( above>=0 && testIntersect(curLine.left, curLine.right, eq.lines[above].left, eq.lines[above].right, i)){
                EventPoint newPoint(EventPoint::INTERSECTION, i, (LineSeg)curLine, 
                                    eq.lines[above], -1,-1,-1);
                eq.pointSet.insert(newPoint);
                
            }
            if(below>=0 && testIntersect(curLine.left, curLine.right, eq.lines[below].left, eq.lines[below].right, i)){
                EventPoint newPoint(EventPoint::INTERSECTION, i, (LineSeg)curLine, 
                                    eq.lines[below], -1,-1,-1);
                eq.pointSet.insert(newPoint);
                
            }
        }
        else if(e.type == EventPoint::RIGHT){
            //set<LineSeg, lineSegCompare>::iterator itr;
            int above, below;
            itr = sweptLines.find(eq.lines[e.lIndex]);
            //find the lines directly above and below this one in the list
            //if they exist, check for an intersection
            if(itr != sweptLines.begin() && itr++ != sweptLines.end()){
                above = (itr--)->lIndex;
                below = (itr++)->lIndex;
                if(itr++ != sweptLines.end() && testIntersect( eq.lines[above].left, eq.lines[above].right, eq.lines[below].left, eq.lines[below].right, i)){
                    EventPoint newPoint(EventPoint::INTERSECTION, i, eq.lines[above], 
                                        eq.lines[below], -1,-1,-1);
                    eq.pointSet.insert(newPoint);
                }
            }
            if(itr != sweptLines.end())
               sweptLines.erase(itr);
        }
        else{ //intersection
            int intersect1, intersect2;
            LineSeg* above;
            LineSeg* below;
            // set<LineSeg, lineSegCompare>::iterator itr;
            pair<set<LineSeg>::iterator,bool> check;
            
            ret.push_back(e.point); //store the intersection in the output vector
            
            //remove intersecting lines from the set, update their y coord and reinsert
            //to maintain proper sorting
            
            
            //gah, might want to redo all of this so its just a pointer to the 
            //line seg stored in the line vector rather than the lines themselves
            //so you don't have to update twice
            test();
            
            itr = sweptLines.find(e.intersection.first);
            intersect1 = itr->lIndex;
            eq.lines[intersect1].getYCoord(e.point.x+1);
            sweptLines.erase(itr);
            itr = sweptLines.insert(eq.lines[intersect1]).first;
            //err keep thinking about this, resinsertion should replace
            //the two lines in the proper place
            //-----------
            test();
            
            if(itr++ != sweptLines.end()){
                *below = *(itr++);
                below->yCoord = below->getYCoord(e.point.x+1);
                if(testIntersect(eq.lines[intersect1].right, eq.lines[intersect1].left, below->right, below->left, i)){
                    EventPoint newPoint(EventPoint::INTERSECTION, i, eq.lines[intersect1], 
                                        (LineSeg)*below, -1,-1,-1);
                    eq.pointSet.insert(newPoint);
                }
            }
            test();
            
            itr = sweptLines.find(e.intersection.second);
            intersect2 = itr->lIndex;
            eq.lines[intersect2].getYCoord(e.point.x+1);
            sweptLines.erase(itr);
            check = sweptLines.insert(eq.lines[intersect2]);
            itr = check.first;
            
            test();
            
            if(itr != sweptLines.begin()){
                *above = *(itr--);
                above->yCoord = above->getYCoord(e.point.x+1);
                if(testIntersect(eq.lines[intersect2].right, eq.lines[intersect2].left, above->right, above->left, i)){
                    EventPoint newPoint(EventPoint::INTERSECTION, i, eq.lines[intersect2], 
                                        (LineSeg)*above, -1,-1,-1);
                    eq.pointSet.insert(newPoint);
                }
            }
        }
        eq.pointSet.erase(eq.pointSet.begin());
    }
    return ret;
}

bool ofxSweepLine::testIntersect(ofVec2f a1, ofVec2f a2, ofVec2f b1, ofVec2f b2, ofVec2f &i){
    
    ofVec2f a = a2-a1;
    ofVec2f b = b2-b1;
    ofVec2f c = b2-a2;
    
    double f = perpDot(a,b);
    if(!f)
        return false; //lines are parallel
    
    double aa = perpDot(a,c);
    double bb = perpDot(b,c);
    
    if(f<0){
        if( aa>0 || bb>0 || aa<f || bb<f)
            return false;
    }
    else{
        if(aa<0 || bb<0 || aa>f || bb>f)
            return false;
    }
    double mu = 1.0-(aa/f);
    ofVec2f intersection = ((b2-b1)*mu) + b1;
    i.set(intersection.x, intersection.y);
    if(i!= a1 && i!= a2 && i != b1 && i !=b2)
        return true;
    else {
        return false;
    }
}
double ofxSweepLine::perpDot(ofVec2f a , ofVec2f b){
    return (a.y*b.x)-(a.x*b.y);
}



