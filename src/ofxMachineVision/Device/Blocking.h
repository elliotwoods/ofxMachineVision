#pragma once

#include "Base.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief A class interface to implement for blocking camera API's
		Implement this interface if your camera api is blocking, i.e. thread waits whilst new frame arrives.
		
		Please implement in your derived class :
		* Virtual functions below
		* Virtual functions in Device::Base

		Single shot
		-----------

		If your camera is a one-shot camera. Then please ignore the singleShot function and just write
		the capture code into getFrame. We will call this whenever we want a single shot.

		Examples of single shot blocking devices:
		* StilImages

		*/
		class Blocking : public Base {
		public:
			/**
			OVerride this function to return new frames. Feel free to throw ofxMachineVision::Exception if something goes wrong
			*/
			virtual shared_ptr<Frame> getFrame() = 0;
		};
	}
}