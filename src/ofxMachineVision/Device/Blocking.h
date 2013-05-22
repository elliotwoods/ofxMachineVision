#pragma once

#include "ofPixels.h"
#include "ofRectangle.h"

#include "ofxMachineVision/Specification.h"
#include "ofxMachineVision/Frame.h"
#include "Base.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief A class interface to implement for blocking camera API's
		Implement this interface if your camera api is blocking, i.e. thread waits whilst new frame arrives.
		*/
		class Blocking : public Base {
		public:
			/**
			\name Interface::Base
			*/
			//@{
			virtual Specification open(int deviceID = 0) = 0;
			virtual void close() = 0;
			virtual void setExposure(Microseconds exposure) { };
			virtual void setBinning(int binningX = 1, int binningY = 1) { };
			virtual void setROI(const ofRectangle &) { };
			virtual void setTriggerMode(const TriggerMode &, const TriggerSignalType &) { };
			virtual void setGPOMode(const GPOMode &) { };
			//@}

			virtual bool startCapture() = 0;
			virtual void stopCapture() = 0;
			/**
			Note : Make sure to lock the Frame for writing whilst you write to it.
			*/
			virtual void getFrame(Frame &) = 0;
		};
	}
}