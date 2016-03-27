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
			Specification specification(this->settings.width, this->settings.height, "NullDevice", "NullDevice");
			specification.addFeature(Feature::Feature_FreeRun);
			specification.addPixelMode(PixelMode::Pixel_L8);
			return specification;
		}
		
		//----------
		bool NullDevice::startCapture() {
			return true;
		}
		
		//----------
		void NullDevice::stopCapture() {
			
		}
		
		//----------
		void NullDevice::close() {
			
		}
		
		//----------
		void NullDevice::getFrame(shared_ptr<Frame> frame) {
			auto & pixels = frame->getPixels();
			pixels.allocate(this->settings.width, this->settings.height, OF_PIXELS_MONO);
			pixels.set(0, 0);

			ofSleepMillis(1e3 / this->settings.frameRate);
			frame->setFrameIndex(this->frameIndex++);
			frame->setTimestamp(ofGetElapsedTimeMicros());
		}
	}
}