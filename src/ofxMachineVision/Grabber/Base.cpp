#include "Base.h"

namespace ofxMachineVision {
	namespace Grabber {
		//----------
		Base::Base() {
			this->deviceState = State_Empty;
		}

		//----------
		Base::~Base() {
			this->deviceState = State_Deleting;
			switch(this->deviceType) {
				case Device::Type_Blocking:
					this->thread->stopThread();
					this->thread->waitForThread();
				case Device::Type_Updating:
					break;
				case Device::Type_NotImplemented:
					break;
			}
			this->baseDevice.reset();
			this->deviceState = State_Empty;
		}

		//----------
		void Base::setDevice(DevicePtr device) {
			this->close();
			this->baseDevice = device;

			this->deviceType = getType(device);

			//open a thread for the device if required
			switch (this->deviceType) {
			case Device::Type_Blocking: {
				this->thread = shared_ptr<Utils::ActionQueueThread>(new Utils::ActionQueueThread());
				break;
			}
			default:
				this->thread.reset();
				break; 
			}

			this->deviceState = (bool) this->baseDevice ? DeviceState::State_Closed : DeviceState::State_Empty;
		}

		//----------
		void Base::clearDevice() {
			this->setDevice(DevicePtr());
		}

		//----------
		DevicePtr Base::getDevice() const {
			return this->baseDevice;
		}

		//----------
		string Base::getDeviceTypeName() const {
			if (this->baseDevice) {
				return this->baseDevice->getTypeName();
			}
			else {
				return "uninitialised";
			}
		}
	}
}