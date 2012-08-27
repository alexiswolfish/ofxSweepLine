//
//  ofxSweepLine.cpp
//  an implementation of the Bentley–Ottmann line 
//  intersection algorithm for openFrameworks
//
//  by Alex Wolfe @ the Studio for Creative Inquiry

#include <iostream>
#include <math.h>
#include "ofxSweepLine.h"

float LineSeg::getYCoord(int time) const{
    float slope = (left.y-right.y)/(left.x-right.x);
    return slope*(time-left.x)+left.y;
}
/*---------------------------------------------*/
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
    for ( it=pointSet.begin() ; it != pointSet.end(); it++ ){
       cout << it->type << " " << it->point<< " " << it->pIndex<< " " << it->edge <<endl;
    }
}
/*---------------------------------------------*/

float ofxSweepLine::sweepPos;

ofxSweepLine::ofxSweepLine(vector<ofPolyline> p){
    eq = *new EventQueue(p);
}
ofxSweepLine::ofxSweepLine(){
    sweepPos = 0;
}
void ofxSweepLine::test(){
    set<LineSeg, lineSegCompare>::iterator itr;
    cout<<"printing tree"<<endl;
    for (itr=sweptLines.begin(); itr!=sweptLines.end(); itr++)
        cout << (*itr).pIndex << " " << (*itr).edge << " " << (*itr).left << " " << endl;
}
vector<ofVec2f> ofxSweepLine::sweep(vector<ofPolyline> p){
    eq = *new EventQueue(p); //based on b-tree, auto sorted    
    sweepPos = eq.pointSet.begin()->point.x;
    sweptLines = *new std::set<LineSeg, lineSegCompare>; 
    
    pair<set<LineSeg, lineSegCompare>::iterator,bool> inserted;
    set<LineSeg, lineSegCompare>::iterator itr;
    vector<ofVec2f> ret;

    while(!eq.pointSet.empty()){   //While (x is nonempty) {
        EventPoint e = *eq.pointSet.begin(); //get next Event
        ofVec2f i;
        if(e.type == EventPoint::LEFT){
            LineSeg* above;
            LineSeg* below;
            LineSeg curLine = eq.lines[e.lIndex];
            //DEBUG:: check to see if inserted.second is true;
            inserted = sweptLines.insert(curLine);
            itr = inserted.first;
            if(itr != sweptLines.begin())
                *above = *(itr--);
            else {
                above = NULL;
            }
            if(itr++ != sweptLines.end())
                *below = *(itr++);
            else {
                below = NULL;
            }

            if( above != NULL && testIntersect(curLine.left, curLine.right, above->left, above->right, i)){
                EventPoint newPoint;
                newPoint.type = EventPoint::INTERSECTION;
                newPoint.point = i;
                newPoint.intersection.first = (LineSeg)curLine;
                newPoint.intersection.second = (LineSeg)*above;
                newPoint.pIndex= -1;
                newPoint.edge = -1;
                newPoint.lIndex = -1;
                eq.pointSet.insert(newPoint);
                
            }
            if(below != NULL && testIntersect(curLine.left, curLine.right, below->left, below->right, i)){
                EventPoint newPoint;
                newPoint.type = EventPoint::INTERSECTION;
                newPoint.point = i;
                newPoint.intersection.first = (LineSeg)curLine;
                newPoint.intersection.second = (LineSeg)*below;
                eq.pointSet.insert(newPoint);

            }
        }
        else if(e.type == EventPoint::RIGHT){
            //set<LineSeg, lineSegCompare>::iterator itr;
            LineSeg* above;
            LineSeg* below;
            itr = sweptLines.find(eq.lines[e.lIndex]);
            //find the lines directly above and below this one in the list
            //if they exist, check for an intersection
            if(itr != sweptLines.begin() && itr++ != sweptLines.end()){
                *above = *(itr--);
                *below = *(itr++);
                if(itr++ != sweptLines.end() && testIntersect(above->left, above->right, below->left, below->right, i)){
                    EventPoint newPoint;
                    newPoint.type = EventPoint::INTERSECTION;
                    newPoint.point = i;
                    newPoint.intersection.first = (LineSeg)*above;
                    newPoint.intersection.second = (LineSeg)*below;
                    eq.pointSet.insert(newPoint);
                }
            }
            sweptLines.erase(itr);
        }
        else{ //intersection
            LineSeg* intersect1;
            LineSeg* intersect2;
            LineSeg* above;
            LineSeg* below;
            
           // set<LineSeg, lineSegCompare>::iterator itr;
            
            ret.push_back(e.point); //store the intersection in the output vector
           
            //above and below should in theory now be swapped *** DEBUG
            //after the intersection
            
            itr = sweptLines.find(e.intersection.first);
            *intersect1 = *itr; //will = nLines.end() if not found
            intersect1->yCoord = intersect1->getYCoord(e.point.x+1);
            if(itr++ != sweptLines.end()){
                *below = *(itr++);
                below->yCoord = below->getYCoord(e.point.x+1);
                if(testIntersect(intersect1->right, intersect1->left, below->right, below->left, i)){
                    EventPoint newPoint;
                    newPoint.type = EventPoint::INTERSECTION;
                    newPoint.point = i;
                    newPoint.intersection.first = (LineSeg)*intersect1;
                    newPoint.intersection.second = (LineSeg)*below;
                    eq.pointSet.insert(newPoint);
                }
            }
            itr = sweptLines.find(e.intersection.second);
            *intersect2 = *itr;
             intersect2->yCoord = intersect2->getYCoord(e.point.x+1);
            if(itr != sweptLines.begin()){
                *above = *(itr--);
                above->yCoord = above->getYCoord(e.point.x+1);
                if(testIntersect(intersect2->right, intersect2->left, above->right, above->left, i)){
                    EventPoint newPoint;
                    newPoint.type = EventPoint::INTERSECTION;
                    newPoint.point = i;
                    newPoint.intersection.first = (LineSeg)*intersect2;
                    newPoint.intersection.second = (LineSeg)*above;
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



