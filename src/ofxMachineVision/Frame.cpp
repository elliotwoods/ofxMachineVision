#include "Frame.h"

namespace ofxMachineVision {
#pragma mark Frame::Storage
	//----------
	Frame::Storage::Storage() {

	}

#pragma mark Frame
	//----------
	Frame::Frame(Storage & storage) 
	: storage(storage) {

	}

	//----------
	Frame::~Frame() {

	}

	//----------
	bool Frame::operator<(const Frame& rhs) const {
		return this->getTimestamp() < rhs.getTimestamp();
	}
}