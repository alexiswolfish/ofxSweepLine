#include "testApp.h"
//OfxSweepLine example
//--------------------------------------------------------------
void testApp::setup(){
    guiWidth = 200;
    guiSpace = 8;
    
    //OF colorTheme
    main.set(185,187,189);
    comp1.set(58,56,57);
    comp2.set(238,57,135);
    comp3.set(253,164,5);
    comp4.set(208,83,153);
    comp5.set(253,187,28);
    comp6.set(123,223,232);
    curColor.set(comp4);
    
    //GUI setup
    panelMain.setup("", "main.xml", guiSpace, guiSpace*4);
    panelMain.add(newButton.setup("NEW",guiWidth));
    panelMain.add(snapToggle.setup("SNAP", false, guiWidth));
    panelMain.add(strokeWidthSlider.setup("STROKE WIDTH", 2, 0.001, 5));
    
    newButton.addListener(this,&testApp::clearDisplay);
    
    panelDisplay.setup("display options", "display.xml", guiSpace, panelMain.getHeight()+guiSpace*5);
    panelDisplay.add(pointsToggle.setup("POINTS", false, guiWidth));
    panelDisplay.add(intersectToggle.setup("SWEEP", false, guiWidth));
    
    panelColor.setup("","color.xml", guiSpace, panelMain.getHeight()+panelDisplay.getHeight()+guiSpace*6);
    panelColor.add(rSlider.setup("R",curColor.r,0,255));
    panelColor.add(gSlider.setup("G",curColor.g,0,255));
    panelColor.add(bSlider.setup("B",curColor.b,0,255));
    
    canvas = *new ofRectangle(guiWidth+guiSpace*3, guiSpace*4, ofGetWindowWidth() - guiWidth-32, ofGetWindowHeight()-16-guiSpace*3);
    swatch = *new ofRectangle(guiSpace, panelColor.getPosition().y + panelColor.getHeight() + guiSpace, guiWidth,guiWidth/2);
    
    curPath.setFilled(false);
    cleared = false;
    swept = false;
    
    ofEnableSmoothing();
}

//--------------------------------------------------------------
void testApp::update(){

    curColor.set(rSlider.value, gSlider.value, bSlider.value);
    curPath.setStrokeWidth(strokeWidthSlider.value);
    curPath.setStrokeColor(curColor);
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(main);
    ofSetColor(comp1);
    ofRect(0, 0, ofGetWindowWidth(), guiSpace*3);
    ofSetColor(comp2);
    ofDrawBitmapString("ofxSweepLine SWEEPING EXAMPLE", guiSpace, guiSpace*2);
    ofSetColor(255, 255, 255);
    ofRect(canvas);
    ofSetColor(curColor);
    ofRect(swatch);
    
    panelMain.draw();
    panelDisplay.draw();
    panelColor.draw();
    
    curPath.draw();

    if(pointsToggle.value){
        sweepLine = *new ofxSweepLine(curPath.getOutline());
        ofPushStyle();
        ofFill();
         ofSetColor(comp5);
        for(EventPoint v: sweepLine.eq.pointSet)
            ofCircle(v.point.x, v.point.y,2);
        ofPopStyle();
    }
    if(intersectToggle.value){
        
    }
    if(swept){
        ofPushStyle();
        ofFill();
        ofSetColor(0,255,255);
        for(ofVec2f i : intersections){
            ofCircle(i.x, i.y, 2);
        }
        ofPopStyle();
    }
    if(snap && canvas.inside(mouseX, mouseY)){
        ofLine(lastPoint.x, lastPoint.y, mouseX, mouseY);
    }
    if(cleared)
        clear();
}

void testApp::clearDisplay(bool &pressed){
    cleared = true;
}
void testApp::clear(){
    ofPushStyle();
    ofSetColor(255, 255, 255);
    ofFill();
    ofRect(canvas);
    curPath.clear();
    ofPopStyle();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' ')
        cleared = true;
    if(key == 'p')
         sweepLine.eq.sanityTest();
    if(key == 's'){
        swept = true;
        intersections.clear();
        sweepLine = *new ofxSweepLine(curPath.getOutline());
        intersections = sweepLine.sweep(curPath.getOutline());
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    if(!snapToggle.value){
        if(canvas.inside(mouseX, mouseY))
            curPath.lineTo(mouseX, mouseY);
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if(canvas.inside(mouseX, mouseY)){
        lastPoint.set(mouseX, mouseY);
        cleared = false;
        swept = false;
        curPath.moveTo(mouseX, mouseY);
    }
    if(snapToggle.value && canvas.inside(mouseX, mouseY))
        snap = true;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    snap = false;
    if(snapToggle.value && canvas.inside(mouseX, mouseY))
        curPath.lineTo(mouseX, mouseY);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}