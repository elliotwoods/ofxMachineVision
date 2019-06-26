#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	// Get the register of device types (note for this you'll need ofxSingleton and ofxPlugin referenced from your project)
	auto & drivers = ofxMachineVision::Device::FactoryRegister::X();

	// Iterate through the drivers
	for (auto & driverIterator : drivers) {
		auto driverName = driverIterator.first;
		auto driver = driverIterator.second;

		// We need a blank device to call listDevices
		auto blankDevice = driver->makeUntyped();
		
		cout << "Opening " << driverName << endl;

		// Not all drivers return a list of devices. By default here we will only get webcams
		auto deviceDescriptions = blankDevice->listDevices();
		for (auto deviceDescription : deviceDescriptions) {

			// When we list the devices, the special properties (e.g. deviceID, filename, etc) are stored as initialisationSettings
			auto initSettings = deviceDescription.initialisationSettings;

			// We need a new grabber. We make an empty grabber without a device
			auto grabber = make_shared<ofxMachineVision::Grabber::Simple>();
			
			// We make a new device using the driver and pass it to the grabber
			auto newDevice = driver->makeUntyped();
			grabber->setDevice(newDevice);
			
			// Open using the settings from the listDevices, and start the capture
			if (grabber->open(initSettings)) {
				grabber->startCapture();

				// If the capture is running, add it to the list
				if (grabber->getIsDeviceRunning()) {
					this->grabbers.push_back(grabber);
				}
			}

			
		}
	}
}

//--------------------------------------------------------------
void testApp::update() {
	for (auto grabber : this->grabbers) {
		grabber->update();
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	// draw all grabbers in a grid
	int grabberCount = this->grabbers.size();
	int countWidth = ceil(sqrt((float) grabberCount));
	int countHeight = ceil((float)grabberCount / (float)countWidth);
	
	auto drawWidth = ofGetWidth() / countWidth;
	auto drawHeight = ofGetHeight() / countHeight;

	for (int i = 0; i < grabbers.size(); i++) {
		auto grabber = grabbers[i];
		if (grabber) {
			auto x = drawWidth * (i % countWidth);
			auto y = drawHeight * (i / countWidth);

			grabber->draw(x, y, drawWidth, drawHeight);
			stringstream info;
			info << " Device:" << i << " " << grabber->getDeviceTypeName() << " " << grabber->getWidth() << "x" << grabber->getHeight() << "@" << grabber->getFps() << "Hz";
			ofDrawBitmapStringHighlight(info.str(), x + 10, y + 20);
		}
	}
	ofDrawBitmapStringHighlight("App : " + ofToString(ofGetFrameRate()) + "Hz", ofGetWidth()-100, ofGetHeight()-20);
}
