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

		*/
		class Blocking : public Base {
		public:
			/**
			Note : Make sure to lock the Frame for writing whilst you write to it.
			*/
			virtual void getFrame(shared_ptr<Frame>) = 0;
		};
	}
}