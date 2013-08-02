#include "VideoInputDevice.h"

namespace ofxMachineVision {
	namespace Device {
		//---------
		VideoInputDevice::VideoInputDevice(int width, int height) {
			this->width = width;
			this->height = height;
			QueryPerformanceFrequency(&this->timerFrequency);
			this->device.setComMultiThreaded(true);
		}

		//---------
		Specification VideoInputDevice::open(int deviceID) {
			this->deviceID = deviceID;
			this->device.setIdealFramerate(deviceID, 20);
			this->device.setupDevice(deviceID, width, height);
			this->device.setVideoSettingFilter(this->deviceID, this->device.propSharpness, 0);
			QueryPerformanceCounter(&this->timerStart);

			Specification specification(width, height, "videoInput", this->device.getDeviceName(this->deviceID));
			
			specification.addFeature(Feature::Feature_DeviceID);
			specification.addFeature(Feature::Feature_Exposure);
			specification.addFeature(Feature::Feature_FreeRun);
			specification.addFeature(Feature::Feature_Gain);
			specification.addFeature(Feature::Feature_Focus);
			
			specification.addPixelMode(PixelMode::Pixel_RGB8);

			this->frameIndex = 0;

			OFXMV_WARNING << "VideoInputDevice implements Exposure as a normalised range 0...1000. I.e. ignoring the unit us.";
			return specification;
		}

		//---------
		void VideoInputDevice::close() {
			this->device.stopDevice(this->deviceID);
		}

		//---------
		bool VideoInputDevice::startCapture() {
			OFXMV_WARNING << "startCapture is not used with VideoInputDevice";
			return true;
		}

		//---------
		void VideoInputDevice::stopCapture() {
			OFXMV_WARNING << "stopCapture is not supported by VideoInputDevice";
		}

		//---------
		void VideoInputDevice::setExposure(Microseconds exposure) {
			this->device.setVideoSettingCameraPct(this->deviceID, this->device.propExposure, (float) exposure / 1000.0f);
		}
	
		//---------
		void VideoInputDevice::setGain(float percent) {
			this->device.setVideoSettingCameraPct(this->deviceID, this->device.propGain, percent);
		}

		//---------
		void VideoInputDevice::setFocus(float percent) {
			this->device.setVideoSettingCameraPct(this->deviceID, this->device.propFocus, percent);
		}

		//---------
		void VideoInputDevice::getFrame(Frame & frame) {
			forSleeping.sleep(1);
			LARGE_INTEGER timestampLong;
			
			ofPixels & pixels(frame.getPixelsRef());
			if (pixels.getWidth() != this->device.getWidth(this->deviceID) || pixels.getHeight() != this->device.getHeight(this->deviceID)) {
				pixels.allocate(this->device.getWidth(this->deviceID), this->device.getHeight(this->deviceID), OF_IMAGE_COLOR);
			}

			this->device.getPixels(this->deviceID, frame.getPixels(), true, true);
			QueryPerformanceCounter(&timestampLong);
			
			this->frameIndex++;

			frame.setTimestamp((timestampLong.QuadPart - timerStart.QuadPart)* 1e6 / this->timerFrequency.QuadPart);
			frame.setFrameIndex(this->frameIndex);
		}

		//---------
		void VideoInputDevice::showSettings() {
			this->device.showSettingsWindow(this->deviceID);
		}
	}
}