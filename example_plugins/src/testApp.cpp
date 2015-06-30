#include "testApp.h"

using namespace ofxCvGui;

//--------------------------------------------------------------
void testApp::setup(){
	/*
	Notes:

	1. This project has a custom Post-Build step in VS2015 to copy the dll's from the
		Solution's bin dir to this Project's bin dir. This is useful when you are testing
		a plugin because the plugin's solution will be related to the location where the
		plugin is built, whilst this Project's location is where all the dll's want to be.
	*/
	this->deviceIndex = 0;

	this->gui.init();
	auto selectionPanel = this->gui.addScroll();

	selectionPanel->add(Widgets::Title::make("Open a device"));

	selectionPanel->add(Widgets::EditableValue<int>::make("Device index", this->deviceIndex));
	for (auto deviceFactory : ofxMachineVision::Device::FactoryRegister::X()) {
		selectionPanel->add(Widgets::Button::make(deviceFactory.second->getModuleTypeName(), [this, deviceFactory]() {
			auto device = deviceFactory.second->makeUntyped();
			auto grabber = make_shared<ofxMachineVision::Grabber::Simple>();
			
			grabber->setDevice(device);
			grabber->open();
			grabber->startCapture();

			auto panel = this->gui.makePanel(*grabber, deviceFactory.second->getModuleTypeName());
			panel->onUpdate += [grabber](ofxCvGui::UpdateArguments &) {
				grabber->update();
			};
			this->gui.add(panel);
		}));
	}
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

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