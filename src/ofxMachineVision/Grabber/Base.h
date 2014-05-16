#pragma once

#include <string>

#include "ofxMachineVision/Device/Blocking.h"
#include "ofxMachineVision/Device/Types.h"
#include "ofxMachineVision/Specification.h"
#include "ofxMachineVision/Constants.h"
#include "Thread/Blocking.h"

#include "../../../../ofxLiquidEvent/src/ofxLiquidEvent.h"

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
					shared_ptr<Device::Blocking> blockingDevice = static_pointer_cast<Device::Blocking>(device);
					this->threadBlocking = new Thread::Blocking(blockingDevice, this);
					break;
				}
				case Device::Type_NotImplemented:
					OFXMV_FATAL << "Device not implemented";
					break;
				}
				this->baseDevice = device;
				this->deviceState = State_Closed;
			}

			virtual ~Base() {
				this->deviceState = State_Deleting;
				switch(this->deviceType) {
				case Device::Type_Blocking:
					delete this->threadBlocking;
					break;
				}
			}

			virtual void open(int deviceID = 0) = 0;
			virtual void close() = 0;

			virtual void startCapture(const TriggerMode & = Trigger_Device, const TriggerSignalType & = TriggerSignal_Default) = 0;
			virtual void stopCapture() = 0;

			/**
			\name Capture properties
			*/
			//@{
			virtual void setExposure(Microseconds exposure) = 0;
			virtual void setGain(float percent) = 0;
			virtual void setFocus(float percent) = 0;
			virtual void setBinning(int binningX = 1, int binningY = 1) = 0;
			virtual void setROI(const ofRectangle &) = 0;
			virtual void setTriggerMode(const TriggerMode &, const TriggerSignalType &) = 0;
			virtual void setGPOMode(const GPOMode &) = 0;
			//@}

			/**
			\name Product specification
			Some of the members of Device::Specification are exposed.
			*/
			//@{
			const Specification & getDeviceSpecification() const { return this->specification; }
			int getDeviceID() const { return this->getDeviceSpecification().getDeviceID(); }
			int getSensorWidth() const { return this->getDeviceSpecification().getSensorWidth(); }
			int getSensorHeight() const { return this->getDeviceSpecification().getSensorHeight(); }
			const string & getManufacturer() const { return this->getDeviceSpecification().getManufacturer(); };
			const string & getModelName() const { return this->getDeviceSpecification().getModelName(); };
			//@}

			DevicePtr getDevice() { return this->baseDevice; }
			const Device::Type & getDeviceType() const { return this->deviceType; }
			const DeviceState & getDeviceState() const { return this->deviceState; }
			bool getIsDeviceOpen() const { return this->getDeviceState() != State_Closed; }
			bool getIsDeviceRunning() const { return this->getDeviceState() == State_Running; }

			ofxLiquidEvent<FrameEventArgs> onNewFrameReceived;
		protected:
			void setSpecification(const Specification & specification) { this->specification = specification; }

			Specification specification;
			DeviceState deviceState;

			ofxMachineVision::Grabber::Thread::Blocking * threadBlocking;
		private:
			DevicePtr baseDevice; // why is this private and not protected?
			Device::Type deviceType;

			friend Thread::Blocking;
		};
	}

	typedef shared_ptr<Grabber::Base> GrabberPtr;
}