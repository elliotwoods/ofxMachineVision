#pragma once

#include <string>

#include "ofxMachineVision/Device/Blocking.h"
#include "ofxMachineVision/Device/Types.h"
#include "ofxMachineVision/Specification.h"
#include "ofxMachineVision/Constants.h"
#include "Thread/Blocking.h"

#define CHECK_OPEN if(!this->getIsDeviceOpen()) { OFXMV_ERROR << " Method cannot be called whilst device is not open"; return; }
#define REQUIRES(feature) if(!this->specification.supports(feature)) { OFXMV_ERROR << " Device requires " << ofxMachineVision::toString(feature) << " to use this function."; return; }

namespace ofxMachineVision {
	namespace Grabber {
		/**
		\brief Base class for grabbers
		Grabbers are fully implemented classes used to operate abstract Device classes
		*/
		class Base {
		public:
			Base(DevicePtr device) {
				this->deviceType = getType(device);
				switch(this->deviceType) {
				case Device::Type_Blocking: {
					Device::Blocking * blockingDevice = static_cast<Device::Blocking*>(device.get());
					this->threadBlocking = new Thread::Blocking(blockingDevice, this);
					break;
				}
				case Device::Type_NotImplemented:
					OFXMV_FATAL << "Device not implemented";
					break;
				}

				this->deviceState = State_Closed;
			}

			virtual ~Base() {
				this->deviceState = State_Deleting;
				switch(this->deviceType) {
				case Device::Type_Blocking:
					if (this->getIsDeviceOpen()) {
						this->threadBlocking->close();
					}
					delete this->threadBlocking;
					break;
				}
			}

			virtual void open(int deviceID = 0) = 0;
			virtual void close() = 0;

			/**
			\name Capture properties
			*/
			//@{
			virtual void setBinning(int binningX = 1, int binningY = 1) = 0;
			virtual void setROI(const ofRectangle &) = 0;
			virtual void setTriggerMode(const TriggerMode &, const TriggerSignalType &) = 0;
			//@}

			/**
			\name Product specification
			Some of the members of Device::Specification are exposed.
			*/
			//@{
			const Specification & getSpecification() const { return this->specification; }
			int getDeviceID() const { return this->getSpecification().getDeviceID(); }
			int getSensorWidth() const { return this->getSpecification().getSensorWidth(); }
			int getSensorHeight() const { return this->getSpecification().getSensorHeight(); }
			const string & getManufacturer() const { return this->getSpecification().getManufacturer(); };
			const string & getModelName() const { return this->getSpecification().getModelName(); };
			//@}

			const Device::Type & getDeviceType() const { return this->deviceType; }
			const DeviceState & getDeviceState() const { return this->deviceState; }
			bool getIsDeviceOpen() const { return this->getDeviceState() != State_Closed; }
			bool getIsDeviceRunning() const { return this->getDeviceState() == State_Running; }
		protected:
			void setSpecification(const Specification & specification) { this->specification = specification; }

			Specification specification;
			DeviceState deviceState;

			ofxMachineVision::Grabber::Thread::Blocking * threadBlocking;
		private:
			DevicePtr device;
			Device::Type deviceType;

			friend Thread::Blocking;
		};
	}
}