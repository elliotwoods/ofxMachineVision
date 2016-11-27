#include "Frame.h"

namespace ofxMachineVision {
#pragma mark Frame
	//----------
	Frame::Frame() {
		this->lock = new Poco::RWLock();
		this->frameIndex = 0;
		this->timestamp = 0;
		this->empty = true;
	}

	//----------
	Frame::Frame(const Frame & other) {
		if (other.isEmpty()) {
			this->empty = true;
			this->frameIndex = 0;
			this->timestamp = 0;
		} else {
			this->empty = false;
			this->frameIndex = other.getFrameIndex();
			this->timestamp = other.getTimestamp();
			this->pixels = other.getPixels();
		}
		this->lock = new Poco::RWLock();
	}

	//----------
	Frame::~Frame() {
		delete this->lock;
	}

	//----------
	shared_ptr<Frame> Frame::clone() {
		return make_shared<Frame>(*this);
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

	//----------
	void Frame::operator=(Frame & other) {
		this->lockForWriting();
		other.lockForReading();

		this->empty = other.isEmpty();
		this->pixels = other.getPixels();
		this->setTimestamp(other.getTimestamp());
		this->setFrameIndex(other.frameIndex);

		this->unlock();
		other.unlock();
	}
#pragma mark Frame
	//----------
	FrameEventArgs::FrameEventArgs (const shared_ptr<Frame> & frame) {
		this->frame = frame;
	}
}