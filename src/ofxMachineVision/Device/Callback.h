#pragma once

#include "Base.h"
#include "ofxLiquidEvent.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief A class interface to implement for callback based camera API's
		Implement this interface if your camera api is based on callbacks, i.e. callback function called on new frame.

		Please implement in your derived class :
		* Virtual functions below
		* Virtual functions in Device::Base

		*/
		class Callback : public Base {
		public:
			Callback() { }

			/**
			Please lock, allocate, fill, unlock, deliver to onNewFrame each frame you receive from driver callback
			*/
			ofxLiquidEvent<shared_ptr<Frame>> onNewFrame;
		};
	}
}