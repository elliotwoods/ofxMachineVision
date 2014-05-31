#include "OSXUVCDevice.h"

namespace ofxMachineVision {
	namespace Device {
		//-----------
		OSXUVCDevice::OSXUVCDevice(int width, int height, float desiredFramerate) {
			this->width = width;
			this->height = height;
			this->desiredFramerate = desiredFramerate;
			QueryPerformanceFrequency(&this->timerFrequency);
		}
		
		//-----------
		Specification OSXUVCDevice::open(int deviceID) {
			this->deviceID = deviceID;
			this->device.initGrabber(this->width, this->height);
			this->device.setDesiredFrameRate(this->desiredFramerate);
			this->setSharpness(0.0f);
			this->resetTimestamp();
			
			const auto deviceList = this->device.listVideoDevices();
			const string deviceName = deviceList.empty() ? "" : deviceList[deviceList.size() % deviceID];
			Specification specification(width, height, "videoInput", deviceName);
			
			specification.addFeature(Feature::Feature_DeviceID);
			specification.addFeature(Feature::Feature_Exposure);
			specification.addFeature(Feature::Feature_FreeRun);
			specification.addFeature(Feature::Feature_Gain);
			specification.addFeature(Feature::Feature_Focus);
			specification.addFeature(Feature::Feature_Sharpness);
			
			specification.addPixelMode(PixelMode::Pixel_RGB8);
			
			this->frameIndex = 0;
			
			OFXMV_WARNING << "VideoInputDevice implements Exposure as a normalised range 0...1000. I.e. ignoring the unit us.";
			return specification;
		}
	}
}
