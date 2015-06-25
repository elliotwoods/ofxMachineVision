#pragma once

#include "Poco/RWLock.h"
#include "ofPixels.h"
#include "ofThread.h"

#include "../../../addons/ofxMachineVision/src/ofxMachineVision/Constants.h"

namespace ofxMachineVision {
	/**
	\brief An instance of ofPixels with some other metadata and a thread lock.
	*/
	class Frame {
	public:
		Frame();
		Frame(const Frame &);
		~Frame();

		bool lockForReading();
		bool lockForWriting();
		void unlock();

		ofPixels & getPixels() { return this->pixels; }
		const ofPixels & getPixels() const { return this->pixels; }

		/** Set the frame timestamp
		\param timestamp Timestamp of frames in microseconds
		*/
		void setTimestamp(Microseconds timestamp) { this->timestamp = timestamp; }
		Microseconds getTimestamp() const { return this->timestamp; }

		/** Set the frame index
		\param index The index of the frame (can be reset in some camera API's on any parameter change).
		*/
		void setFrameIndex(int frameIndex) { this->frameIndex = frameIndex; }
		long getFrameIndex() const { return this->frameIndex; }

		bool isEmpty() const { return this->empty; }
		void setEmpty(bool empty) { this->empty = empty; }

		bool operator<(const Frame&) const;

		///Copy operator
		void operator=(Frame &);

	protected:
		bool empty;
		Poco::RWLock * lock;
		ofPixels pixels;
		Microseconds timestamp;
		long frameIndex;
	};

	class FrameEventArgs {
	public:
		FrameEventArgs(const shared_ptr<Frame> &);
		shared_ptr<Frame> frame;
	};
}