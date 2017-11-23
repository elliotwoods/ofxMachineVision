#include "Base.h"

namespace ofxMachineVision {
	namespace Grabber {
		//----------
		Base::Base() {
			this->deviceState = DeviceState::Empty;
		}

		//----------
		Base::~Base() {
			this->deviceState = DeviceState::Deleting;
			this->baseDevice.reset();
			this->deviceState = DeviceState::Empty;
		}

		//----------
		void Base::setDevice(DevicePtr device) {
			this->close();
			if (device) {
				this->baseDevice = device;
				this->deviceType = getType(device);

				//open a thread for the device if required
				switch (this->deviceType) {
				case Device::Type::Blocking: {
					this->thread = make_unique<Utils::ActionQueueThread>();
					break;
				}
				default:
					if (this->thread) {
						this->thread.reset();
					}
					break;
				}

				this->deviceState = DeviceState::Closed;
			}
			else {
				this->deviceState = DeviceState::Empty;
			}
			
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