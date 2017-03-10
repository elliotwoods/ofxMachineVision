#include "ofApp.h"

using namespace ofxCvGui;

//--------------------------------------------------------------
void ofApp::setup(){
	/*
	Notes:

	1. This project has a custom Post-Build step in VS2015 to copy the dll's from the
		Solution's bin dir to this Project's bin dir. This is useful when you are testing
		a plugin because the plugin's solution will be related to the location where the
		plugin is built, whilst this Project's location is where all the dll's want to be.
	*/
	this->deviceIndex = 0;

	this->gui.init();
	auto selectionPanel = this->gui.addWidgets();

	selectionPanel->addTitle("Open a device");

	selectionPanel->addEditableValue<int>(this->deviceIndex);
	for (auto deviceFactory : ofxMachineVision::Device::FactoryRegister::X()) {
		selectionPanel->addButton(deviceFactory.second->getModuleTypeName(), [this, deviceFactory]() {
			auto device = deviceFactory.second->makeUntyped();
			auto grabber = make_shared<ofxMachineVision::Grabber::Simple>();

			grabber->setDevice(device);
			if (grabber->open()) {
				grabber->startCapture();

				auto panel = ofxCvGui::Panels::makeBaseDraws(*grabber, deviceFactory.second->getModuleTypeName());
				panel->onUpdate += [grabber](ofxCvGui::UpdateArguments &) {
					grabber->update();
				};
				this->gui.add(panel);
			}
		});
	}
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}