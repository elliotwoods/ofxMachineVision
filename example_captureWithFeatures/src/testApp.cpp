#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	camera.open();
	if (!camera.getIsDeviceOpen()) {
		ofSystemAlertDialog("Device did not open");
	}
	camera.startCapture(Trigger_GPIO1, TriggerSignal_RisingEdge);
	camera.setExposure(1291);
	camera.setROI(ofRectangle(0, 0, 2048, 1024));
}

//--------------------------------------------------------------
void testApp::update(){
	camera.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	camera.draw(ofGetCurrentViewport());
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == ' ') {
		static int index = 0;
		string filename = ofToString(index++) + ".bmp";
		ofSaveImage(camera.getPixelsRef(), filename);
		ofLogNotice("captureWithFeatures") << "Camera image saved to " << filename;
	}
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