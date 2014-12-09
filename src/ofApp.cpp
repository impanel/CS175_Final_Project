#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    height = 10;
    width = 10;
    stepSize = 40;
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh.enableColors();
    mesh.enableIndices();
    
    ofVec3f normalVec = ofVec3f(0,0,1); // normals pointing up
    
    diffuseShader.load("shaders/diffuse.vert", "shaders/diffuse.frag");
    
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            mesh.addVertex(ofPoint(x * stepSize, y * stepSize, 0));
            mesh.addColor(ofFloatColor(1.0, 0, 0));
            mesh.addNormal(normalVec);
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
    
    cout<< mesh.hasNormals() << endl;
    
//   vector <ofIndexType> index = mesh.getIndices();
//    ofVec3f normals = ofVec3f(0, 0 , 1);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    glEnable(GL_DEPTH_TEST);
    cam.begin();
    
    ofSetColor(255);
    
    ofPushMatrix();
    // translate mesh to world center
    ofTranslate((-1 * (stepSize * width - stepSize) / 2), (-1 * (stepSize * height - stepSize) / 2), 0);
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
    
    int lX = 180;
    int lY = 180;
    int lZ = 180;
    
    ofMatrix4x4 mVm = cam.getModelViewMatrix();
    
    ofVec4f lightCoord(lX, lY, lZ, 1);
    ofVec4f eyeLightCoord;
    eyeLightCoord = mVm * lightCoord; // eye coord vector = modelview matrix * object in world coordinates
    
    diffuseShader.setUniform4f("LightPosition", eyeLightCoord.x, eyeLightCoord.y, eyeLightCoord.z, 1.0); // TODO: should be eye coordinates
    //ofSphere(eyeLightCoord.x, eyeLightCoord.y, eyeLightCoord.z, 20); // represents light position
    //ofSphere(lX, lY, lZ, 20); // test sphere
    
    diffuseShader.setUniform3f("Kd", 1.0, 1.0, 1.0); //Diffuse Reflectivity
    diffuseShader.setUniform3f("Ld", .9, .9, .9); //LightSource Intensity

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
    ofSetColor(255, 255, 0);
    ofNoFill();
    glDepthMask(false);
    ofCircle(x, y, r);
    glDepthMask(true);
    ofPopStyle();
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

