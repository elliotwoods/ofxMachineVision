#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //let's instanstiate a test capture device
    camera = new ofxMachineVision::VideoGrabberDevice();
    
    //open a connection to the device
    camera->open();
    
    //start the camera capturing using default trigger
    camera->startFreeRunCapture();
    
    //print device information
    cout << camera->getSpecification().toString();
}

//--------------------------------------------------------------
void testApp::update(){
    
    //update the texture in the Device class
    camera->update();
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //draw the texture in the Device class
    camera->draw(0, 0);
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	camera->everyframe = ! camera->everyframe;
}

//--------------------------------------------------------------
void testApp::reset() {
}

//--------------------------------------------------------------
void testApp::advance() {
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

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