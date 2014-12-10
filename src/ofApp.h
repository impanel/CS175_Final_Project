#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofMatrix3x3 mat4ToMat3(ofMatrix4x4 _mat4);
    
        ofEasyCam cam; // add mouse controls for camera movement
        ofMesh mesh;
        ofMesh mPlane;
        ofShader diffuseShader;
        ofSoundPlayer sound;
        //float * smoothFFT;
        vector<float> smoothFFT;
        vector<unsigned char*> allPixels;
    
        ofImage texture;
        ofFbo dispFbo;
    
        int nBands;
        int rows;
        int columns;
        int width;
        int height;
        int stepSize;
    
        int idx;
};
