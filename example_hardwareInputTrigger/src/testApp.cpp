#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	grabber.open();

	grabber.setExposure(100);
	grabber.setROI(ofRectangle(0,0,2048,1024));

	grabber.startCapture(Trigger_GPIO0, TriggerSignal_RisingEdge);
}

//--------------------------------------------------------------
void testApp::update(){
	grabber.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	grabber.draw(ofGetCurrentViewport());

	stringstream status;
	status << "Last timestamp : \t" << grabber.getLastTimestamp() << endl;
	status << "Last frame index : \t" << grabber.getLastFrameIndex() << endl;
	status << "Framerate : \t\t" << grabber.getFps() << endl;
	status << endl;
	status << grabber.getDeviceSpecification();

	ofEnableAlphaBlending();
	ofDrawBitmapStringHighlight(status.str(), 10, 20, ofColor(100,0,0,100));
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