#pragma once

#include "ofPixels.h"
#include "ofThread.h"

#include "ofxMachineVision/Constants.h"

#include <shared_mutex>

namespace ofxMachineVision {
	/**
	The Frame class is a managed shipment of pixels. Notes:
	* When you own a Frame, you 
	*/
	class Frame {
	public:
		class Storage {
		public:
			Storage();
		protected:
			ofPixels pixels;

			friend class FramePool;
			friend class Frame;
		};

		Frame(Storage &);
		virtual ~Frame();

		const ofPixels & getPixels() const { return this->storage.pixels; }
		ofPixels & getPixels() { return this->storage.pixels; }

		chrono::nanoseconds getTimestamp() const { return this->timestamp; }
		void setTimestamp(chrono::nanoseconds value) { this->timestamp = value; }

		uint64_t getFrameIndex() const { return this->frameIndex; }
		void setFrameIndex(uint64_t value) { this->frameIndex = value;  }

		bool operator<(const Frame&) const;
	protected:
		chrono::nanoseconds timestamp;
		uint64_t frameIndex = 0;
		Storage & storage;

		friend class FramePool;
	};
}