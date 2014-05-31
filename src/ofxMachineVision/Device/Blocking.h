#pragma once

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
			virtual Specification open(int deviceID = 0) override = 0;
			virtual void close() override = 0;
			virtual bool startCapture() override = 0;
			virtual void stopCapture() override = 0;
			virtual void setExposure(Microseconds exposure) override { };
			virtual void setGain(float percent) override { };
			virtual void setFocus(float percent) override { };
			virtual void setSharpness(float percent) = 0;
			virtual void setBinning(int binningX = 1, int binningY = 1) override { };
			virtual void setROI(const ofRectangle &) override { };
			virtual void setTriggerMode(const TriggerMode &, const TriggerSignalType &) override { };
			virtual void setGPOMode(const GPOMode &) override { };
			//@}


			/**
			Note : Make sure to lock the Frame for writing whilst you write to it.
			*/
			virtual void getFrame(shared_ptr<Frame>) = 0;
		};
	}
}