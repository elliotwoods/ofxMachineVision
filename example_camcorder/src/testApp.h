#pragma once

#include "ofxXimea.h"
#include "ofMain.h"
#include "ofxMachineVision.h"

using namespace ofxMachineVision;

class testApp : public ofBaseApp{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	Stream::Recorder recorder;
	ofxMachineVision::SimpleGrabber<ofxXimea::Device> grabber;
};
