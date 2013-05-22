#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	grabber.open();
	//grabber.setExposure(1000);
	grabber.startCapture();
}

//--------------------------------------------------------------
void testApp::update(){
	grabber.update();
	static bool high = false;
	high = !high;
	grabber.setGPOMode(high ? ofxMachineVision::GPOMode_On : ofxMachineVision::GPOMode_Off);
}

//--------------------------------------------------------------
void testApp::draw(){
	grabber.draw(0, 0);

	stringstream status;
	status << "Last timestamp : \t" << grabber.getLastTimestamp() << endl;
	status << "Last frame index : \t" << grabber.getLastFrameIndex() << endl;
	status << endl;
	status << grabber.getDeviceSpecification();

	ofDrawBitmapStringHighlight(status.str(), 10, 10);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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