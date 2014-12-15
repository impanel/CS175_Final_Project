#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
// square mesh setup
    columns = 50;
    rows = columns;
    width = 360;
    height = width;
    stepSize = width/columns;
    
    //set flags
    isVerbose = false;
    isWire = false;
    isPaused = false;
    
    //allocate memory for the displacement map
    texture.allocate(width, height, OF_IMAGE_GRAYSCALE);
    
    // set up the mesh
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh.enableColors();
    mesh.enableIndices();
    
    ofVec3f normalVec = ofVec3f(0,0,1); // normals pointing up
    
    shader.load("shaders/phong.vert", "shaders/phong.frag");
    
    // create the mesh and set vertices, color and normals
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
    
    // index the vertices for proper drawing
    // code for proper indexing found here: http://stackoverflow.com/questions/5915753/generate-a-plane-with-triangle-strips
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
    
    
    //setup array to store all row pixels from FFT values
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
    bands = columns;
    
    for (int i = 0; i < bands; i++)
        smoothFFT.push_back(0);
    
    //start the song playback
    //sound.setVolume(0);
    sound.play();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSoundUpdate(); // update the sound system
    
    // grab the fft and smooth values
    float * val = ofSoundGetSpectrum(bands);
    for (int i = 1; i < bands; i++){
        
        //let value and index slowly sink to 0 ...
        smoothFFT.at(i) = smoothFFT.at(i) * 0.96f;
        
        // ... or replace with higher incoming value
        if (smoothFFT.at(i) < val[i]) smoothFFT[i] = val[i];
    }
    
    unsigned char * charPixels = new unsigned char[width * height]; //1D pixel array
    unsigned char * rowPixels = new unsigned char[width]; //store a line of pixels
    
    // create a new row off FFT pixels
    for (int i = 0 ; i < height; i++)
    {
        rowPixels[i] = ofMap(smoothFFT.at(ofMap(i, 0, height, 1, bands)), 0, 1, 0, 255); // fill one row each frame
    }
    
    // advance all rows indexes
    for (int i = 0 ; i < allPixels.size() -1; i++)
    {
        allPixels.at(i) = allPixels.at(i + 1);
    }
    
    // add new row to array
    allPixels.at(allPixels.size() - 1) = rowPixels;
    
    
    //transform 2D array to 1D array
    for (int x = 0 ; x < width; x++)
    {
        for (int y = 0 ; y < height; y++)
        {
            charPixels[x * width + y] = allPixels[x][y];
        }
    }
    
    //store pixels from unsigned char arrays in depth map texture object so we can send it to the vertex shader
    ofPixels pixels;
    pixels.allocate(width, height, OF_IMAGE_GRAYSCALE);
    pixels.setFromExternalPixels(charPixels, width, height, 1);
    texture.setFromPixels(pixels);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofClear(0, 0, 0); //clear the background each frame
    
    ofDisableAlphaBlending();
    glEnable(GL_DEPTH_TEST);
    cam.setFov(90);
    cam.begin();
    
    // transform the object to have a nice view on start up
    ofSetColor(255);
    ofRotate(285, 1, 0, 0);
    ofRotate(215, 0, 0, 1);
    ofRotate(0, 0, 1, 0);
    ofScale(2., 2., 2.);
    
    ofPushMatrix();
    
    // translate mesh to world center
    ofTranslate((-1 * (width) / 2), (-1 * (height) / 2), 0);
    
    //everything that happens after shader.begin is going to the GPU
    shader.begin();
    
    ofMatrixStack matrixStack(*ofGetWindowPtr());
    ofMatrix4x4 modelViewMatrix = matrixStack.getModelViewMatrix();
    
    //create the world normal matrix
    ofMatrix3x3 normalMatrix = mat4ToMat3(modelViewMatrix);
    normalMatrix.invert();
    normalMatrix.transpose();
    
    ofMatrix4x4 projectionMatrix = matrixStack.getProjectionMatrix();
    
    shader.setUniformMatrix4f("ModelViewMatrix", modelViewMatrix);
    shader.setUniformMatrix3f("NormalMatrix", normalMatrix);
    shader.setUniformMatrix4f("ProjectionMatrix", projectionMatrix);
    shader.setUniformTexture("DispTex", texture.getTextureReference(), 0);
    
    // light position in world coordinates
    int lX = 360;
    int lY = 360;
    int lZ = 180;
    
    float dispScale = 100;
    
    float test = smoothFFT.at(smoothFFT.size() - 1);
    
    ofMatrix4x4 mVm = cam.getModelViewMatrix();
    
    ofVec4f lightCoord(lX, lY, lZ, 1);
    ofVec4f eyeLightCoord;
    eyeLightCoord = mVm * lightCoord; // eye coord vector = modelview matrix * object in world coordinates
    
    shader.setUniform4f("LightPosition", eyeLightCoord.x, eyeLightCoord.y, eyeLightCoord.z, 1.0);
    
    // Material properties
    shader.setUniform3f("Ka", .0, .9, .9); //Ambient Reflectivity
    shader.setUniform3f("Kd", .0, .8, .8); //Diffuse Reflectivity
    shader.setUniform3f("Ks", .0, .9, .9); //Specular Reflectivity
    shader.setUniform1f("Shininess", .9);
    
    // Light properties
    shader.setUniform3f("La", .9, .0, .8); //Ambient Light Intensity
    shader.setUniform3f("Ld", .9, .0, .8); //Diffuse Light Intensity
    shader.setUniform3f("Ls", .1, .1, .9); //Specular Light Intensity
    
    //Dislpacement scalar
    shader.setUniform1f("DispScale", dispScale);
    
    if (isWire)
        mesh.draw(OF_MESH_WIREFRAME); // draw wire mesh
    else
        mesh.draw(OF_MESH_FILL); // draw solid mesh
    
    shader.end();
   
    ofPopMatrix();

    cam.end();
    glDisable(GL_DEPTH_TEST);
    
    
    if (isVerbose)
    {
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
        
        for (int i = 0; i < bands; i++)
        {
            ofRect( i * 20 - 20, ofGetHeight(), 20, -1 * (smoothFFT.at(i) * 200));
        }
        
        //draw displacement map in lower right corner
        texture.draw(ofGetWidth() - 100, ofGetHeight() - 100, 100, 100);
        ofDrawBitmapString("depth map", ofGetWidth() - 100, ofGetHeight() - 110);
    }
    
    ofDrawBitmapString("press [space] for verbose mode \npress [w] to toggle wireframe render \npress [p] to toggle pause the song", ofPoint(10, 10));
    
    // display FrameRate
    ofDrawBitmapString(ofToString((int)ofGetFrameRate()), ofPoint(ofGetWidth() - 30, 10));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == ' ')
    {
        isVerbose = !isVerbose;
    }
    if (key == 'w')
    {
        isWire = !isWire;
    }
    if (key == 'p') {
        if (!isPaused) {
            sound.setPaused(true);
            isPaused = true;
        }
        else
        {
            sound.setPaused(false);
            isPaused = false;
        }
    }
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

// function to convert 4x4 to 3x3 matrix
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

