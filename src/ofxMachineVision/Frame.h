#pragma once

#include "Poco/RWLock.h"
#include "ofPixels.h"
#include "ofThread.h"

namespace ofxMachineVision {
	/**
	\brief An instance of ofPixels with some other metadata and a thread lock
	*/
	class Frame {
	public:
		bool lockForReading() { return this->lock.tryReadLock(); }
		bool lockForWriting() { return this->lock.tryWriteLock(); }
		void unlock() { this->lock.unlock(); }

		const ofPixels & getPixelsRef() const { return this->pixels; }
		const unsigned char * getPixels() const { return this->getPixelsRef().getPixels(); }

	protected:
		Poco::RWLock lock;
		ofPixels pixels;
	};
}