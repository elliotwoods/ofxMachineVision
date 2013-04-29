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
		typedef long long Timestamp;

		/** Use the empty flag if the capture of this frame failed **/
		Frame(bool empty=false);
		Frame(Frame &);
		~Frame();

		bool lockForReading();
		bool lockForWriting();
		void unlock();

		ofPixels & getPixelsRef() { return this->pixels; }
		unsigned char * getPixels() { return this->getPixelsRef().getPixels(); }

		/** Set the frame timestamp
		\param timestamp Timestamp of frames in microseconds
		*/
		void setTimestamp(Timestamp timestamp) { this->timestamp = timestamp; }
		Timestamp getTimestamp() const { return this->timestamp; }

		/** Set the frame index
		\param index The index of the frame (can be reset in some camera API's on any parameter change).
		*/
		void setFrameIndex(int frameIndex) { this->frameIndex = frameIndex; }
		long getFrameIndex() const { return this->frameIndex; }

		bool isEmpty() const { return this->empty; }

		bool operator<(const Frame&) const;

	protected:
		bool empty;
		Poco::RWLock * lock;
		ofPixels pixels;
		Timestamp timestamp;
		long frameIndex;
	};

	class FrameEventArgs {
	public:
		FrameEventArgs(Frame &);
		Frame * frame;
	};
}