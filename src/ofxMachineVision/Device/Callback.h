#pragma once

#include "Base.h"
#include "../../../../ofxLiquidEvent/src/ofxLiquidEvent.h"

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
			Callback() {
				this->incomingFrame = make_shared<Frame>();
			}

			/**
			Note : Make sure to lock the Frame for writing whilst you write to it.
			*/
			ofxLiquidEvent<shared_ptr<Frame>> onNewFrame;

			///Frame allocated, locked and filled by the callback function
			shared_ptr<Frame> incomingFrame;
		};
	}
}