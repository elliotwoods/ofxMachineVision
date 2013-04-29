#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
    ofEnableSmoothing();
	ofBackground(50);

	grabber.open();
	grabber.startCapture();
	recorder.setGrabber(grabber);

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	ofPushMatrix();
	auto timeStart = this->recorder.getFirstTimestamp();
	auto timeWindow = this->recorder.getDuration();

	ofScale(ofGetWidth() / timeWindow, ofGetHeight());

	for (auto &it : recorder) {
		float position = it.getTimestamp();
		position = position - timeStart;
		position /= timeWindow;

		ofLine(position, 0, position, 1);
	}
	ofPopMatrix();

	stringstream ss;
	ss << "Recorder:\t" << Stream::Recorder::toString(this->recorder.getState()) << endl;

	ofDrawBitmapString(ss.str(), 10, 10);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if (key == ' ') {
		if (recorder.isRecording()) {
			recorder.stop();
		} else {
			recorder.start();
		}
	}
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
	