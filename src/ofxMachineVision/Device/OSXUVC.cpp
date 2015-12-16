#include "OSXUVC.h"

#ifdef TARGET_OSX

#include <mach/mach.h>
#include <mach/mach_time.h>

namespace ofxMachineVision {
	namespace Device {
		//-----------
		OSXUVC::OSXUVC(int width, int height, float desiredFramerate) {
			this->width = width;
			this->height = height;
			this->desiredFramerate = desiredFramerate;
			this->frame = shared_ptr<Frame>(new Frame());
		}
		
		//-----------
		Specification OSXUVC::open(int deviceID) {
			this->deviceID = deviceID;
			this->device.setDeviceID(this->deviceID);
			this->device.setup(this->width, this->height);
			this->device.setDesiredFrameRate(this->desiredFramerate);
			
			this->controller.useCamera(0x046d, 0x082d, deviceID);
			this->controller.setAutoExposure(false);
			this->controller.setAutoFocus(false);
			
			this->setSharpness(0.0f);
			this->resetTimestamp();
			
			const auto deviceList = this->device.listVideoDevices();
			if (deviceList.size() == 0) {
				OFXMV_FATAL << "No video devices found";
			}
			const string deviceName = deviceList.empty() ? "" : deviceList[deviceID % deviceList.size()];
			
			Specification specification(width, height, "UVC", deviceName);
			
			specification.addFeature(Feature::Feature_DeviceID);
			specification.addFeature(Feature::Feature_Exposure);
			specification.addFeature(Feature::Feature_FreeRun);
			specification.addFeature(Feature::Feature_Gain);
			specification.addFeature(Feature::Feature_Focus);
			specification.addFeature(Feature::Feature_Sharpness);
			
			specification.addPixelMode(PixelMode::Pixel_RGB8);
			
			this->resetTimestamp();
			
			OFXMV_WARNING << "OSXUVCDevice implements Exposure as a normalised range 0...1000. I.e. ignoring the unit us.";
			return specification;
		}
		
		//-----------
		bool OSXUVC::startCapture() {
			OFXMV_WARNING << "startCapture is not used with OSXUVCDevice";
			return true;
		}
		
		//-----------
		void OSXUVC::stopCapture() {
			OFXMV_WARNING << "stopCapture is not supported by VideoInputDevice";
		}
		
		//----------
		void OSXUVC::close() {
			this->device.close();
		}
		
		//---------
		void OSXUVC::setExposure(Microseconds exposure) {
			this->controller.setExposure((float) exposure / 1000.0f);
		}
		
		//---------
		void OSXUVC::setGain(float percent) {
			this->controller.setGain(percent);
		}
		
		//---------
		void OSXUVC::setFocus(float percent) {
			this->controller.setAbsoluteFocus(percent);
		}
		
		//---------
		void OSXUVC::setSharpness(float percent) {
			this->controller.setSharpness(percent);
		}
		
		//---------
		void OSXUVC::updateIsFrameNew() {
			this->device.update();
			if (this->device.isFrameNew()) {
				this->frame->lockForWriting();
				const auto elapsedTime = mach_absolute_time() - this->timerStart;
				
				auto timestamp = AbsoluteToNanoseconds(* (const AbsoluteTime *) & elapsedTime);
				this->frame->setTimestamp(* (uint64_t *) & timestamp / 1000);
				this->frame->setFrameIndex(this->frameIndex);
				this->frame->getPixels() = this->device.getPixels();
				this->frame->unlock();
			}
		}
		
		//---------
		bool OSXUVC::isFrameNew() {
			return this->device.isFrameNew();
		}
		
		//---------
		shared_ptr<Frame> OSXUVC::getFrame() {
			return this->frame;
		}
		
		//---------
		void OSXUVC::resetTimestamp() {
			this->timerStart = mach_absolute_time();
			this->frameIndex = 0;
		}
	}
}

#endif