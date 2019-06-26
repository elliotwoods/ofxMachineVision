#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofLogNotice() << "Using " << GRABBER_STR;
	
	grabbers.resize(numGrabbers);
	frameRates.resize(numGrabbers);
	lastCaptureTimes.resize(numGrabbers);

	for (int i = 0; i < grabbers.size(); i++) {
		grabbers[i] = make_shared<GRABBER>();
		frameRates[i] = 0;
		lastCaptureTimes[i] = 0;
		auto grabber = grabbers[i];
#ifdef USE_OFXMACHINEVISION
		// get default initialisation settings and update as desired
		if(i==0) grabber->getDevice()->listDevices(); // returns empty. is this normal?
		auto initSettings = grabber->getDefaultInitialisationSettings();
		initSettings->getInt("Device ID").set(i);
		initSettings->getInt("Width").set(captureWidth);
		initSettings->getInt("Height").set(captureHeight);
		initSettings->getInt("Ideal frame rate").set(captureFps);
		grabber->open(initSettings);
		grabber->startCapture();
#else
		if (i == 0) grabber->listDevices();
		grabber->setDeviceID(i);
		grabber->setDesiredFrameRate(captureFps);
		grabber->setup(captureWidth, captureHeight);
#endif
	}
}

//--------------------------------------------------------------
void testApp::update() {
	// update all of the grabbers
	for (int i = 0; i < grabbers.size(); i++) {
		auto grabber = grabbers[i];
		if (grabber) {
			grabber->update();
			if (grabber->isFrameNew()) {
				frameRates[i] = ofLerp(frameRates[i], 1.0f / (ofGetElapsedTimef() - lastCaptureTimes[i]), 0.1); // apply smoothing
				lastCaptureTimes[i] = ofGetElapsedTimef();
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	// draw all grabbers in a grid
	int x = 0;
	int y = 0;
	for (int i = 0; i < grabbers.size(); i++) {
		auto grabber = grabbers[i];
		if (grabber) {
			if (x + grabber->getWidth() > ofGetWidth()) {
				x = 0;
				y += grabber->getHeight();
			}
			grabber->draw(x, y);
			stringstream info;
			info << "Using " << GRABBER_STR << ". Device:" << i << " " << grabber->getWidth() << "x" << grabber->getHeight() << " @ " << frameRates[i] << " fps";
			ofDrawBitmapStringHighlight(info.str(), x + 10, y + 20);
			x += grabber->getWidth();
		}
	}
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), ofGetWidth()-100, ofGetHeight()-20);
}
