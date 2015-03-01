#pragma once
#include "Base.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief A class interface to implement for threaded camera API's with isFrameNew style implementation.
		Implement this interface if your camera api follows isFrameNew design pattern.

		Please implement in your derived class :
		* Virtual functions below
		* Virtual functions in Device::Base

		*/
		class Updating : public Base {
		public:
			virtual void updateIsFrameNew() = 0;
			virtual bool isFrameNew() = 0;
			virtual shared_ptr<Frame> getFrame() = 0;  //needs a comment - what does this mean?
		};
	}
}