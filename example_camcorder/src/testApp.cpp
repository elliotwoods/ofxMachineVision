#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
    ofEnableSmoothing();
	ofBackground(50);

	grabber.open();
	grabber.setROI(ofRectangle(0, 0, 2048, 256));
	grabber.startCapture();
	recorder.setGrabber(grabber);

	this->toggleRecord = false;
	this->bangClear = false;

	gui.setHeight(400);
	
	gui.addLabel("ofxMachineVision", OFX_UI_FONT_LARGE);
	gui.addLabel("Camcorder example", OFX_UI_FONT_MEDIUM);
	
	gui.addSpacer();

	gui.addLabel("Device");
	//gui.addButton("Open camera", &this->bangOpen);
	this->guiDeviceStateLabel = gui.addLabel("Device state", "...", OFX_UI_FONT_SMALL);
	
	gui.addSpacer();

	gui.addToggle("Record", &this->toggleRecord);
	this->guiRecordStateLabel = gui.addLabel("Recorder state", "...", OFX_UI_FONT_SMALL);
	this->guiRecordCountLabel = gui.addLabel("Frame count", "Empty", OFX_UI_FONT_SMALL);
	gui.addButton("Clear frames", &this->bangClear);
	
	gui.addSpacer();

	gui.addLabel("Frame details");
	this->guiFrameTimestamp = gui.addLabel("Timestamp", "", OFX_UI_FONT_SMALL);
	this->guiFrameDuration = gui.addLabel("Duration", "", OFX_UI_FONT_SMALL);
}

//--------------------------------------------------------------
void testApp::update(){
	this->guiRecordStateLabel->setLabel(Stream::Recorder::toString(this->recorder.getState()));
	this->guiDeviceStateLabel->setLabel(toString(grabber.getDeviceState()));
	this->guiRecordCountLabel->setLabel("Frame count: " + ofToString(this->recorder.size()));

	if (this->toggleRecord != this->recorder.getIsRecording()) {
		if (this->toggleRecord) {
			this->recorder.start();
		} else {
			this->recorder.stop(); 
		}
	}

	if (this->bangClear) {
		this->recorder.clear();
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	//-----
	// draw image
	if (this->selectionView.isAllocated()) {
		this->selectionView.draw(ofGetCurrentViewport());
	}
	//-----

	timeStart = this->recorder.getFirstTimestamp();
	timeWindow = MAX(this->recorder.getDuration(), 1);

	//----
	//draw frame ticks
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(ofColor(255, 100));
	for (auto &it : recorder) {
		float position = it.first;
		position -= timeStart;
		position /= timeWindow;
		position *= ofGetWidth();
		ofLine(position, ofGetHeight() - 150, position, ofGetHeight() - 50);
	}	
	ofPopStyle();
	//-----


	//-----
	//draw selection
	ofPushStyle();
	{
		float x = selectionTimestamp;
		x -= timeStart;
		x /= timeWindow;
		x *= ofGetWidth();
		
		float width = selectionDuration;
		width *= ofGetWidth() / (float) timeWindow;

		ofSetColor(ofColor(255, 100));
		ofSetLineWidth(0);
		ofRect(x, ofGetHeight() - 150, width, 200);

		ofSetLineWidth(1.0f);
		ofSetColor(ofColor(255, 200));
		ofLine(x, ofGetHeight() - 150, x, ofGetHeight());
	}
	ofPopStyle();
	//-----


	//-----
	//draw time ticks

	//find scale of time
	int timeScale = floor(log((float) timeWindow) / log(10.0f) + 0.5f);
	double timeScaleMinor = pow(10, timeScale - 2);

	double firstTick = floor(timeStart / timeScaleMinor) * timeScaleMinor;
	double time = firstTick;
	int i = 0;
	while (time < timeStart + timeWindow) {
		//convert to screen space
		float x = (time - timeStart) / timeWindow;
		x *= ofGetWidth();

		float height;
		float y2;
		if (i % 10 == 0) {
			height = 10.0f;
			string label;
			if (timeScale >= 6) {
				label = ofToString((time - timeStart) / 1e6, 2) + "s";
			} else if (timeScale >= 3) {
				label = ofToString((time - timeStart) / 1e3, 2) + "ms";
			} else {
				label = ofToString((time - timeStart) , 2) + "us";
			}
			ofDrawBitmapString(label, x, ofGetHeight() - 3);
			y2 = ofGetHeight() - 3.0f;
		} else if (i % 5 == 0) {
			height = 5.0f;
			y2 = ofGetHeight() - 15.0f;
		} else {
			height = 2.0f;
			y2 = ofGetHeight() - 18.0f;
		}
		ofLine(x, ofGetHeight() - 20.0f, x, y2);

		time += timeScaleMinor;
		i++;
	}
	//
	//-----


	//-----
	// draw pipet selections
	ofPushStyle();
	for (auto pipet : pipets) {
		auto drawLocation = pipet * ofVec2f(ofGetWidth(), ofGetHeight());

		//background
		ofSetColor(0, 100);
		ofRect(drawLocation.x, drawLocation.y, 24, 20);

		//number
		ofSetColor(255);
		if (recorder.count(this->selectionTimestamp) > 0) {
			auto value = this->getValue(this->selectionTimestamp, pipet);
			ofDrawBitmapString(ofToString((int) value), drawLocation.x + 2, drawLocation.y + 18);
		}

		//line
		ofLine(drawLocation.x, drawLocation.y, drawLocation.x, drawLocation.y + 20);
	}
	ofPopStyle();
	//-----

}

//--------------------------------------------------------------
unsigned char testApp::getValue(Frame::Timestamp & timestamp, ofVec2f position) { 
	const auto &pixels = this->recorder[timestamp].getPixelsRef();
	position *= ofVec2f(pixels.getWidth(), pixels.getHeight());
	return pixels[pixels.getPixelIndex(position.x, position.y)];
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key)
	{
	case OF_KEY_BACKSPACE:
		if (pipets.size() > 0)
			pipets.pop_back();
		break;
	case 'p':
		float x = (float) ofGetMouseX() / (float) ofGetWidth();
		float y = (float) ofGetMouseY() / (float) ofGetHeight();
		pipets.push_back(ofVec2f(x, y));
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if (key == ' ') {
		toggleRecord = ! toggleRecord;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	float time = (float) x / ofGetWidth();
	time *= timeWindow;
	time += timeStart;

	Stream::Recorder::iterator frame = recorder.lower_bound( (Frame::Timestamp) time);
	if (frame == recorder.end()) {
		return;
	}

	this->selectionTimestamp = frame->first;

	auto next = recorder.upper_bound(frame->first);
	if (next == recorder.end()) {
		this->selectionDuration = 0;
	} else {
		this->selectionDuration = next->first - frame->first;
	}

	this->selectionView.allocate(frame->second.getPixelsRef());
	this->selectionView.loadData(frame->second.getPixelsRef());

	this->guiFrameTimestamp->setLabel("Timestamp : " + ofToString(selectionTimestamp) + "us");
	this->guiFrameDuration->setLabel(ofToString(selectionDuration) + "us / " + ofToString(float(1e6) / float(selectionDuration)) + "fps");
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	mouseDragged(x, y, button);
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
	