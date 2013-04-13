#pragma once

#include "ofxMachineVision/Constants.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief Root base class for Device interfaces
		This interface specifies the functions that every Device::Interface should have.

		Some notes on Device::Interface :
		* No state should be stored in the interface
		* Each interface should offer a set of functions from the perspective of the camera API

		*/
		class Base {
		public:
			virtual ~Base() { }
			virtual Specification open(int deviceID = 0) = 0;
			virtual void close() = 0;
				
			/**
			\name Optional interfaces
			You should declare which actions your camara supports in the Specification's Features
			*/
			//@{
			virtual void setBinning(int binningX = 1, int binningY = 1) = 0;
			virtual void setROI(const ofRectangle &) = 0;
			virtual void setTriggerMode(const TriggerMode &, const TriggerSignalType &) = 0;
			//@}
		};
	}
}