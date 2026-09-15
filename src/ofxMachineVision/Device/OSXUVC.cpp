#include "OSXUVC.h"
#ifdef TARGET_OSX

namespace ofxMachineVision {
	namespace Device {
		//----------
		vector<Base::ListedDevice> OSXUVC::listDevices() const {
			ofVideoGrabber grabber;
			vector<ListedDevice> result;
			for (const auto & device : grabber.listDevices()) {
				if (device.bAvailable) {
					result.push_back({ make_shared<Base::InitialisationSettings>(device.id), "AVFoundation", device.deviceName });
				}
			}
			return result;
		}

		//----------
		shared_ptr<Base::InitialisationSettings> OSXUVC::getDefaultSettings() const {
			return make_shared<Base::InitialisationSettings>();
		}

		//----------
		Specification OSXUVC::open(shared_ptr<Base::InitialisationSettings> settings) {
			this->close();
			auto typedSettings = this->getTypedSettings<Base::InitialisationSettings>(settings);
			this->device = make_shared<ofVideoGrabber>();
			this->device->setDeviceID(typedSettings->deviceID);
			this->device->setDesiredFrameRate(30);
			this->device->setUseTexture(false);
			if (!this->device->setup(1920, 1080)) {
				this->device.reset();
				throw Exception("Cannot open AVFoundation camera");
			}
			this->timerStart = chrono::steady_clock::now();
			this->frameIndex = 0;
			Specification specification(CaptureSequenceType::Continuous, this->device->getWidth(), this->device->getHeight(), "Apple", "AVFoundation");
			specification.setDeviceID(typedSettings->deviceID);
			specification.addPixelMode(PixelMode::RGB8);
			return specification;
		}

		//----------
		bool OSXUVC::startCapture() {
			this->capturing = this->device && this->device->isInitialized();
			return this->capturing;
		}

		//----------
		void OSXUVC::stopCapture() { this->capturing = false; }

		//----------
		void OSXUVC::close() {
			this->stopCapture();
			if (this->device) { this->device->close(); }
			this->device.reset();
			this->frame.reset();
		}

		//----------
		void OSXUVC::updateIsFrameNew() {
			if (!this->capturing) { return; }
			this->device->update();
			if (this->device->isFrameNew()) {
				this->frame = FramePool::X().getAvailableFrameFilledWith(this->device->getPixels());
				this->frame->setTimestamp(chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now() - this->timerStart));
				this->frame->setFrameIndex(this->frameIndex++);
			}
		}

		//----------
		bool OSXUVC::isFrameNew() { return this->capturing && this->device->isFrameNew(); }

		//----------
		shared_ptr<Frame> OSXUVC::getFrame() { return this->frame; }
	}
}
#endif
