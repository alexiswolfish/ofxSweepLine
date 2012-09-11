#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSweepLine.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    void clearDisplay(bool &pressed);
    void clear();
    
    /*-----ofxSweepLine-----*/
    ofxSweepLine sweepLine;
    vector<ofVec2f> intersections;
    
    /*-------Drawing--------*/
    ofPath curPath;
    ofRectangle canvas;
    ofColor curColor;

    /*---------Gui----------*/
    ofxPanel panelMain;
    ofxToggle pointsToggle, snapToggle;
    ofxButton newButton;
    ofxSlider<float> strokeWidthSlider;
    
    ofxPanel panelDisplay;
    ofxToggle intersectToggle;
    
    ofxPanel panelColor;
    ofxSlider<int> rSlider, gSlider, bSlider;
    
    ofRectangle swatch;
    
    bool cleared, swept, snap;
    ofVec2f lastPoint;
    float guiWidth, guiSpace, canvasHeight;
    ofColor main, comp1, comp2, comp3, comp4, comp5, comp6;
};
