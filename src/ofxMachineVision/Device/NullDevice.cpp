#include "NullDevice.h"

#include "ofAppRunner.h"

namespace ofxMachineVision {
	namespace Device {
		//----------
		string NullDevice::getTypeName() const {
			return "NullDevice";
		}

		//----------
		Specification NullDevice::open(shared_ptr<Base::InitialisationSettings> initialisationSettings) {
			auto settings = this->getTypedSettings<InitialisationSettings>(initialisationSettings);

			this->settings = * settings;
			Specification specification(CaptureSequenceType::Continuous
				, this->settings.width
				, this->settings.height
				, "NullDevice"
				, "NullDevice");
			specification.addPixelMode(PixelMode::L8);
			return specification;
		}
		
		//----------
		bool NullDevice::startCapture() {
			this->startTime = chrono::high_resolution_clock::now();
			return true;
		}
		
		//----------
		void NullDevice::stopCapture() {
			
		}
		
		//----------
		void NullDevice::close() {
			
		}
		
		//----------
		shared_ptr<Frame> NullDevice::getFrame() {
			ofSleepMillis(1e3 / this->settings.frameRate);

			auto frame = FramePool::X().getAvailableAllocatedFrame(this->settings.width, this->settings.height, OF_PIXELS_MONO);
			frame->getPixels().set(0, 0);
			frame->setFrameIndex(this->frameIndex++);
			frame->setTimestamp(chrono::high_resolution_clock::now() - this->startTime);

			return frame;
		}
	}
}