#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	cout << "Listing devices..." << endl;
	auto devices = camera.getDevice()->listDevices(); // returns empty. is this normal?
	for (auto deviceDescription : devices) {
		cout << deviceDescription.manufacturer << ", " << deviceDescription.model << endl;
	}

	// get default initialisation settings and update as desired
	auto initSettings = camera.getDefaultInitialisationSettings();
	initSettings->getInt("Device ID").set(0);
	initSettings->getInt("Width").set(640);
	initSettings->getInt("Height").set(480);
	initSettings->getInt("Ideal frame rate").set(60);

    //open a connection to the device
    camera.open(initSettings);
	//camera.open();

    //start the camera capturing using default trigger
	camera.startCapture();

    //print device information
    cout << camera.getDeviceSpecification().toString();
}

//--------------------------------------------------------------
void testApp::update(){
    //update the texture in the Device class
    camera.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    //draw the texture in the Device class
    camera.draw(0, 0);
	ofDrawBitmapStringHighlight(ofToString(camera.getFps()), 10, 10);
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