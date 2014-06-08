#include "OSXUVCDevice.h"

#ifdef TARGET_OSX

#include <mach/mach.h>
#include <mach/mach_time.h>

namespace ofxMachineVision {
	namespace Device {
		//-----------
		OSXUVCDevice::OSXUVCDevice(int width, int height, float desiredFramerate) {
			this->width = width;
			this->height = height;
			this->desiredFramerate = desiredFramerate;
			this->frame = shared_ptr<Frame>(new Frame());
		}
		
		//-----------
		Specification OSXUVCDevice::open(int deviceID) {
			this->deviceID = deviceID;
			this->device.setDeviceID(this->deviceID);
			this->device.initGrabber(this->width, this->height);
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
		bool OSXUVCDevice::startCapture() {
			OFXMV_WARNING << "startCapture is not used with OSXUVCDevice";
			return true;
		}
		
		//-----------
		void OSXUVCDevice::stopCapture() {
			OFXMV_WARNING << "stopCapture is not supported by VideoInputDevice";
		}
		
		//----------
		void OSXUVCDevice::close() {
			this->device.close();
		}
		
		//---------
		void OSXUVCDevice::setExposure(Microseconds exposure) {
			this->controller.setExposure((float) exposure / 1000.0f);
		}
		
		//---------
		void OSXUVCDevice::setGain(float percent) {
			this->controller.setGain(percent);
		}
		
		//---------
		void OSXUVCDevice::setFocus(float percent) {
			this->controller.setAbsoluteFocus(percent);
		}
		
		//---------
		void OSXUVCDevice::setSharpness(float percent) {
			this->controller.setSharpness(percent);
		}
		
		//---------
		void OSXUVCDevice::updateIsFrameNew() {
			this->device.update();
			if (this->device.isFrameNew()) {
				this->frame->lockForWriting();
				const auto elapsedTime = mach_absolute_time() - this->timerStart;
				
				auto timestamp = AbsoluteToNanoseconds(* (const AbsoluteTime *) & elapsedTime);
				this->frame->setTimestamp(* (uint64_t *) & timestamp / 1000);
				this->frame->setFrameIndex(this->frameIndex);
				this->frame->getPixelsRef() = this->device.getPixelsRef();
				this->frame->unlock();
			}
		}
		
		//---------
		bool OSXUVCDevice::isFrameNew() {
			return this->device.isFrameNew();
		}
		
		//---------
		shared_ptr<Frame> OSXUVCDevice::getFrame() {
			return this->frame;
		}
		
		//---------
		void OSXUVCDevice::resetTimestamp() {
			this->timerStart = mach_absolute_time();
			this->frameIndex = 0;
		}
	}
}

#endif