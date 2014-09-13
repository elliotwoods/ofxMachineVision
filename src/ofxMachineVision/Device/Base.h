#pragma once

#include "../../../addons/ofxMachineVision/src/ofxMachineVision/Specification.h"
#include "../../../addons/ofxMachineVision/src/ofxMachineVision/Frame.h"
#include "../../../addons/ofxMachineVision/src/ofxMachineVision/Constants.h"

#include "ofRectangle.h"

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

			virtual bool startCapture() {
				return false;
			};
			virtual void stopCapture() {

			}
			virtual void singleShot() { }
				
			/**
			\name Optional interfaces
			You should declare which actions your camara supports in the Specification's Features
			*/
			//@{
			virtual void setExposure(Microseconds exposure) { };
			virtual void setGain(float percent) { };
			virtual void setFocus(float percent) { };
			virtual void setSharpness(float percent) { };
			virtual void setBinning(int binningX = 1, int binningY = 1) { };
			virtual void setROI(const ofRectangle &) { };
			virtual void setTriggerMode(const TriggerMode &, const TriggerSignalType &) { };
			virtual void setGPOMode(const GPOMode &) { };
			//@}
		};
	}

	typedef shared_ptr<Device::Base> DevicePtr;
}