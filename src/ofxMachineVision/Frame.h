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
		Frame() {
			this->lock = new Poco::RWLock();
			this->frameIndex = 0;
			this->timestamp = 0;
		}
		~Frame() { delete this->lock; }
		bool lockForReading() { return this->lock->tryReadLock(); }
		bool lockForWriting() { return this->lock->tryWriteLock(); }
		void unlock() { this->lock->unlock(); }

		ofPixels & getPixelsRef() { return this->pixels; }
		unsigned char * getPixels() { return this->getPixelsRef().getPixels(); }

		/** Set the frame timestamp
		\param timestamp Timestamp of frames in microseconds
		*/
		void setTimestamp(long timestamp) { this->timestamp = timestamp; }
		long getTimestamp() const { return this->timestamp; }

		/** Set the frame index
		\param index The index of the frame (can be reset in some camera API's on any parameter change).
		*/
		void setFrameIndex(int frameIndex) { this->frameIndex = frameIndex; }
		long getFrameIndex() { this->frameIndex; }
	protected:
		Poco::RWLock * lock;
		ofPixels pixels;
		long timestamp;
		long frameIndex;
	};
}