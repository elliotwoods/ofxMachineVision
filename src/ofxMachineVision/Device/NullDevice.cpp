#include "NullDevice.h"

#include "ofAppRunner.h"

namespace ofxMachineVision {
	namespace Device {
		//----------
		NullDevice::NullDevice(int width, int height, float desiredFramerate) {
			this->specification = Specification(width, height, "ofxMachineVision", "NullDevice");
			this->specification.addFeature(Feature::Feature_FreeRun);
			this->specification.addFeature(Feature::Feature_DeviceID);
			this->specification.addPixelMode(PixelMode::Pixel_L8);
			this->frameInterval = (int) (1000.0f / desiredFramerate);
			this->frameIndex = 0;
		}
		
		//----------
		string NullDevice::getTypeName() const {
			return "NullDevice";
		}

		//----------
		Specification NullDevice::open(int deviceID) {
			this->specification.setDeviceID(0);
			return this->specification;
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
			if (pixels.getWidth() != this->specification.getSensorWidth() || pixels.getHeight() != this->specification.getSensorHeight()) {
				pixels.allocate(this->specification.getSensorWidth(), this->specification.getSensorHeight(), OF_PIXELS_MONO);
				pixels.set(0,0);
			}
			ofSleepMillis(this->frameInterval);
			frame->setFrameIndex(this->frameIndex++);
			frame->setTimestamp(ofGetElapsedTimeMicros());
		}
	}
}