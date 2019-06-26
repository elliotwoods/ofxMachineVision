#pragma once

#include "ofMain.h"
#include "ofxMachineVision.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
    
	int captureWidth = 640;
	int captureHeight = 360;
	int captureFps = 30;
	vector<shared_ptr<ofxMachineVision::Grabber::Simple>> grabbers;
};
