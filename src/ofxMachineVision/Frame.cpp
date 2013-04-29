#include "Frame.h"

namespace ofxMachineVision {
#pragma mark Frame
	//----------
	Frame::Frame(bool empty) {
		this->lock = new Poco::RWLock();
		this->frameIndex = 0;
		this->timestamp = 0;
		this->empty = empty;
	}

	//----------
	Frame::Frame(Frame & other) {
		other.lockForReading();
		if (other.isEmpty()) {
			this->empty = true;
			this->frameIndex = 0;
			this->timestamp = 0;
		} else {
			this->empty = false;
			this->frameIndex = other.getFrameIndex();
			this->timestamp = other.getTimestamp();
			this->pixels = other.getPixelsRef();
		}
		other.unlock();
	}

	//----------
	Frame::~Frame() {
		delete this->lock;
	}

	//----------
	bool Frame::lockForReading() {
		return this->lock->tryReadLock();
	}

	//----------
	bool Frame::lockForWriting() {
		return this->lock->tryWriteLock();
	}

	//----------
	void Frame::unlock() {
		this->lock->unlock();
	}

	//----------
	bool Frame::operator<(const Frame& rhs) const {
		return this->getTimestamp() < rhs.getTimestamp();
	}
#pragma mark Frame
	//----------
	FrameEventArgs::FrameEventArgs (Frame & frame) {
		this->frame = & frame;
	}
}