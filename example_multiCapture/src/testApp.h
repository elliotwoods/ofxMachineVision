#pragma once

// you can also define these in project settings
#define USE_OFXMACHINEVISION

#ifdef USE_OFXMACHINEVISION
#define GRABBER ofxMachineVision::SimpleGrabber<ofxMachineVision::Device::Webcam>
#define GRABBER_STR "ofxMachineVision"
#else
#define GRABBER ofVideoGrabber
#define GRABBER_STR "ofVideoGrabber"
#endif

#include "ofMain.h"
#include "ofxMachineVision.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
    
	int numGrabbers = 2; // we could read this from the number of devices, but ofxMachineVision::Device::Webcam always returns 0 currently
	int captureWidth = 640;
	int captureHeight = 360;
	int captureFps = 30;
	vector<shared_ptr<GRABBER>> grabbers;
	
	// for calculating framerates
	vector<float> frameRates; // keep all capture framerates in here
	vector<float> lastCaptureTimes;
};
