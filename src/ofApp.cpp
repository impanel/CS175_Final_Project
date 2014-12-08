#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    height = 10;
    width = 10;
    size = 40;
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh.enableColors();
    mesh.enableIndices();
    
    
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            mesh.addVertex(ofPoint(x * size, y * size, 0));
            mesh.addColor(ofFloatColor(1.0, 0, 0));
        }
    }
    
    // code for proper indexing from here: http://stackoverflow.com/questions/5915753/generate-a-plane-with-triangle-strips
    for(int y = 0; y < height - 1; y++)
    {
        if((y & 1) == 0) //even rows
        {
            for(int x = 0; x < width; x++)
            {
                mesh.addIndex(x + y * width);
                mesh.addIndex(x + (y + 1) * width);
            }
        }
        else //odd rows
        {
            for(int x = width - 1; x > 0; x--)
            {
                mesh.addIndex(x + (y + 1) * width);
                mesh.addIndex(x - 1 + y * width);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    ofPushMatrix();
    ofTranslate(300, 300);
    mesh.draw(OF_MESH_WIREFRAME);
    ofPopMatrix();
    
    
    vector<ofIndexType> indices = mesh.getIndices();
    
    for (int i = 0; i < height - 1; i++)
    {
        for (int j = 0; j < width - 1; j++)
        {
            //ofDrawBitmapString(ofToString(indices.at(i * height + j)), mesh.ge);
        }
    }
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
