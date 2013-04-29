#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	
	grabber.open();
	grabber.setROI(ofRectangle(0,grabber.getSensorHeight() / 2,grabber.getSensorWidth(), 2));
	grabber.startCapture();

	cout << grabber.getDeviceSpecification().toString();

	fbo.allocate(grabber.getSensorWidth(), 2048, GL_LUMINANCE);
}

//--------------------------------------------------------------
void testApp::update(){
	grabber.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	fbo.begin();
	static int y = 0;
	grabber.draw(0,y);
	fbo.end();

	y += grabber.getHeight();
	if (y > fbo.getHeight()) {
		y = 0;
		ofPixels pixels;
		fbo.readToPixels(pixels);
		ofSaveImage(pixels, ofToString(ofGetFrameNum()) + ".png");
	}

	ofPushMatrix();
	ofRotateZ(90);
	fbo.draw(0, 0, ofGetHeight(), -ofGetWidth());
	ofPopMatrix();

	ofDrawBitmapStringHighlight(ofToString(grabber.getFps()), 20, 20);
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