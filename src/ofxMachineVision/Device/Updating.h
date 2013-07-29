#pragma once
#include "Base.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief A class interface to implement for threaded camera API's with isFrameNew style implementation.
		Implement this interface if your camera api follows isFrameNew design pattern.
		*/
		class Updating : public Base {
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

			virtual void updateIsFrameNew() = 0;
			virtual bool isFrameNew() = 0;
		};
	}
}