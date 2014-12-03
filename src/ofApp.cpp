#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    sh.load("shaders/basic-gl3.vshader", "shaders/diffuse-gl3.fshader");
    pl.plane(100, 100, 10, 10);
    pl.enableColors();
    pl.addColor(ofFloatColor(255, 0, 0));
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(130, 130, 130);
    //sh.begin();
    //
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    pl.draw();
    //
    ofPopMatrix();
    //sh.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
