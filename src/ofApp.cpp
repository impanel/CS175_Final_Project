#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //ofLoadImage(texture, "test.png");
    //texture.loadImage("test.png");
    
    // square mesh setup
    columns = 50;
    rows = columns;
    width = 360;
    height = width;
    stepSize = width/columns;
    idx = width - 1;
    
    texture.allocate(width, height, OF_IMAGE_GRAYSCALE);
    dispFbo.allocate(width, height, OF_IMAGE_GRAYSCALE);
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh.enableColors();
    mesh.enableIndices();
    
    ofVec3f normalVec = ofVec3f(0,0,1); // normals pointing up
    
    diffuseShader.load("shaders/diffuse.vert", "shaders/diffuse.frag");
    
    // create the mesh
    for (int x = 0; x < columns; x++)
    {
        for (int y = 0; y < rows; y++)
        {
            mesh.addVertex(ofPoint(x * stepSize, y * stepSize, 0));
            mesh.addColor(ofFloatColor(1.0, 0, 0));
            mesh.addNormal(normalVec);
            
            //we are using non normalized texture coordinates (ARB texture)!!!
            mesh.addTexCoord(ofVec2f(x * stepSize, y * stepSize));
        }
    }
    
    // code for proper indexing from here: http://stackoverflow.com/questions/5915753/generate-a-plane-with-triangle-strips
    for(int y = 0; y < rows - 1; y++)
    {
        if((y & 1) == 0) //even rows
        {
            for(int x = 0; x < columns; x++)
            {
                mesh.addIndex(x + y * columns);
                mesh.addIndex(x + (y + 1) * columns);
            }
        }
        else //odd rows
        {
            for(int x = columns - 1; x > 0; x--)
            {
                mesh.addIndex(x + (y + 1) * columns);
                mesh.addIndex(x - 1 + y * columns);
            }
        }
    }
    
    
    //setup array to store all row pixels
    for (int i = 0 ; i < width; i++)
    {
        allPixels.push_back(new unsigned char[height]); //stores row pixels
        
        for (int j = 0 ; j < height; j++) {
            allPixels[i][j] = 0; // fill with black
        }
    }
    
    
    //Sound stuff
    sound.loadSound("Kuedo-Mtzpn.mp3");
    
    //create array for smoothing the fft values
    nBands = columns;
    
    for (int i = 0; i < nBands; i++)
        smoothFFT.push_back(0);
    
    //start the song
    //sound.setVolume(0);
    sound.play();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSoundUpdate(); // update the sound system
    
    // grab the fft and smooth values
    float * val = ofSoundGetSpectrum(nBands);
    for (int i = 0; i < nBands; i++){
        
        //let value and index slowly sink to 0 ...
        smoothFFT.at(i) = smoothFFT.at(i) * 0.96f;
        
        // ... or replace with higher incoming value
        if (smoothFFT.at(i) < val[i]) smoothFFT[i] = val[i];
    }
    
    unsigned char * charPixels = new unsigned char[width * height]; //pixels to fill
    unsigned char * rowPixels = new unsigned char[width]; //store a line of pixels
    
 
    
    for (int y = 0 ; y < height; y++)
    {
        rowPixels[y] = ofMap(smoothFFT.at(ofMap(y, 0, height, 0, nBands)), 0, 1, 0, 255); // fill one row each frame
    }
    allPixels.at(idx) = rowPixels;
    idx--;
    if (idx <= 0) {
        idx = width - 1;
    }
    
    for (int x = 0 ; x < width; x++)
    {
        //if(idx >= width)
            //{
                //allPixels.erase (allPixels.begin()); //removes first element
                //allPixels.push_back(rowPixels); // pushes back new row
            //}
            //else
            //{
                //allPixels.push_back(rowPixels);
                //cout<< allPixels.size() << endl;
                //cout<<idx<<endl;
                //idx++;
            //}
    }
    
  
//    for (int i = 0 ; i < width * height; i++)
//    {
//        if(i < (width * height)/2)
//            charPixels[i] = 255;
//        else
//            charPixels[i] = 0;
//    }
    
    for (int x = 0 ; x < width; x++)
    {
        for (int y = 0 ; y < height; y++)
        {
            charPixels[x * width + y] = allPixels[x][y];
        }
    }
    
    ofSetColor(255);
    
    ofPixels pixels;
    pixels.allocate(width, height, OF_IMAGE_GRAYSCALE);
    pixels.setFromExternalPixels(charPixels, width, height, 1);
    texture.setFromPixels(pixels);
    //texture.draw(0,0);
    
    //dispFbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofDisableAlphaBlending();
    glEnable(GL_DEPTH_TEST);
    cam.setFov(90);
    cam.begin();
    
    ofSetColor(255);
    
    ofPushMatrix();
    
    // translate mesh to world center
    ofTranslate((-1 * (width) / 2), (-1 * (height) / 2), 0);
    ofScale(1., 1., 1.);
    
    diffuseShader.begin();
    
    ofMatrixStack matrixStack(*ofGetWindowPtr());
    ofMatrix4x4 modelViewMatrix = matrixStack.getModelViewMatrix();
    
    ofMatrix3x3 normalMatrix = mat4ToMat3(modelViewMatrix);
    normalMatrix.invert();
    normalMatrix.transpose();
    
    ofMatrix4x4 projectionMatrix = matrixStack.getProjectionMatrix();
    
    diffuseShader.setUniform4f("uColor", 1.0, 0, 0, 1.0);
    diffuseShader.setUniformMatrix4f("ModelViewMatrix", modelViewMatrix);
    diffuseShader.setUniformMatrix3f("NormalMatrix", normalMatrix);
    diffuseShader.setUniformMatrix4f("ProjectionMatrix", projectionMatrix);
    diffuseShader.setUniformTexture("dispTex", texture.getTextureReference(), 0);
    //diffuseShader.setUniformTexture("dispTex", dispFbo.getTextureReference(), 0);
    
    int lX = 180;
    int lY = 180;
    int lZ = 180;
    
    float dispScale = 100;
    
    ofMatrix4x4 mVm = cam.getModelViewMatrix();
    
    ofVec4f lightCoord(lX, lY, lZ, 1);
    ofVec4f eyeLightCoord;
    eyeLightCoord = mVm * lightCoord; // eye coord vector = modelview matrix * object in world coordinates
    
    diffuseShader.setUniform4f("LightPosition", eyeLightCoord.x, eyeLightCoord.y, eyeLightCoord.z, 1.0);
    //ofSphere(eyeLightCoord.x, eyeLightCoord.y, eyeLightCoord.z, 20); // represents light position
    //ofSphere(lX, lY, lZ, 20); // test sphere
    
    diffuseShader.setUniform3f("Kd", 1.0, 1.0, 1.0); //Diffuse Reflectivity
    diffuseShader.setUniform3f("Ld", .9, .9, .9); //LightSource Intensity
    diffuseShader.setUniform1f("dispScale", dispScale);
    
    //mesh.draw(OF_MESH_WIREFRAME); // draw wire mesh
    mesh.draw(OF_MESH_FILL); // draw solid mesh
    
    diffuseShader.end();
    
    ofPopMatrix();
    
    //cout<< cam.getOrientationQuat()<<endl;
    //cout<< "------------"<<endl;
    
    cam.end();
    glDisable(GL_DEPTH_TEST);
    
    
    // draw interaction area
    ofRectangle vp = ofGetCurrentViewport();
    float r = MIN(vp.width, vp.height) * 0.5f;
    float x = vp.width * 0.5f;
    float y = vp.height * 0.5f;
    
    ofPushStyle();
    ofSetLineWidth(3);
    ofSetColor(255, 255, 255);
    ofNoFill();
    glDepthMask(false);
    ofCircle(x, y, r);
    glDepthMask(true);
    ofPopStyle();
    
    
    
    // draw FFT visuals
    ofSetColor(255,255,255,255);
    
    for (int i = 0; i < nBands; i++)
    {
        ofRect( i * 20, ofGetHeight(), 20, -1 * (smoothFFT.at(i) * 200));
    }
    
    texture.draw(0, 0);
    //dispFbo.draw(0, 0);
    
    // draw FrameRate
    ofDrawBitmapString(ofToString((int)ofGetFrameRate()), ofPoint(ofGetWidth() - 30, ofGetHeight() - 10));
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

//--------------------------------------------------------------

ofMatrix3x3 ofApp::mat4ToMat3(ofMatrix4x4 _mat4)
{
    return ofMatrix3x3(_mat4._mat[0][0],
                       _mat4._mat[0][1],
                       _mat4._mat[0][2],
                       _mat4._mat[1][0],
                       _mat4._mat[1][1],
                       _mat4._mat[1][2],
                       _mat4._mat[2][0],
                       _mat4._mat[2][1],
                       _mat4._mat[2][2]);
}

