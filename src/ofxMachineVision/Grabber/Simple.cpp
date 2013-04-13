#include "Simple.h"

namespace ofxMachineVision {
	namespace Grabber {
		//----------
		void Simple::open(int deviceID) {
			this->close();

			try {
				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					this->threadBlocking->open(deviceID);
					this->deviceState = this->getSpecification().getValid() ? State_Running : State_Closed;
					break;
				default:
					throw std::exception("Device not implemented");
				}
			} catch (std::exception e) {
				OFXMV_ERROR << e.what();
				this->deviceState = State_Closed;
			}
		}

		//----------
		void Simple::close() {
			if (this->getIsDeviceRunning()) {
				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					this->threadBlocking->close();
					this->deviceState = State_Closed;
					break;
				}
			}
		}

		//----------
		void Simple::startCapture(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN

			this->setTriggerMode(triggerMode, triggerSignalType);
		
			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->startFreeRun();
				this->deviceState = State_Running;
				break;
			}
		}

		//----------
		void Simple::stopCapture() {
			CHECK_OPEN
			switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					this->threadBlocking->stopFreeRun();
					break;
			}
		}

		//----------
		void Simple::setBinning(int binningX, int binningY) {
			CHECK_OPEN
			REQUIRES(Feature_Binning)

			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setBinning(binningX, binningY);
				break;
			}
		}

		//----------
		void Simple::setROI(const ofRectangle & roi) {
			CHECK_OPEN
			REQUIRES(Feature_ROI);

			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setROI(roi);
				break;
			}
		}

		//----------
		void Simple::setTriggerMode(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN
			REQUIRES(Feature_Triggering)
			REQUIRES(triggerMode);
			REQUIRES(triggerSignalType);

			
			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setTriggerMode(triggerMode, triggerSignalType);
			}
		}
	}
}