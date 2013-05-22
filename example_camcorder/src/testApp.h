#pragma once

#include "ofxXimea.h"
#include "ofMain.h"
#include "ofxMachineVision.h"
#include "ofxUI.h"

using namespace ofxMachineVision;

class testApp : public ofBaseApp{

  public:
	void setup();
	void update();
	void loadSelection();

	void draw();

	unsigned char getValue(Microseconds, ofVec2f position);

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

	ofxUICanvas gui;
	ofxUILabel * guiDeviceStateLabel;
	ofxUILabel * guiRecordStateLabel;
	ofxUILabel * guiRecordCountLabel;
	ofxUILabel * guiFrameTimestamp;
	ofxUILabel * guiFrameDuration;

	bool bangOpen;
	bool bangClear;
	bool bangClearBefore;
	bool bangClearAfter;

	bool bangSavePipets;
	bool toggleRecord;
	bool toggleSave;
	bool toggleProgress;

	Microseconds timeStart;
	Microseconds timeWindow;
	
	ofTexture selectionView;
	Microseconds selectionTimestamp;
	Microseconds selectionDuration;

	vector<ofVec2f> pipets;
};
