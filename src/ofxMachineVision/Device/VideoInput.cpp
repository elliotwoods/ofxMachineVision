#include "VideoInput.h"

#include "ofConstants.h"
#ifdef TARGET_WIN32

namespace ofxMachineVision {
	namespace Device {
		//---------
		VideoInput::VideoInput(int width, int height, float desiredFramerate) {
			this->width = width;
			this->height = height;
			this->desiredFramerate = desiredFramerate;
			QueryPerformanceFrequency(&this->timerFrequency);
		}

		//---------
		string VideoInput::getTypeName() const {
			return "VideoInput";
		}

		//---------
		Specification VideoInput::open(int deviceID) {
			this->deviceID = deviceID;
			if (!this->device) {
				this->device = make_shared<::videoInput>();
			}
			this->device->setComMultiThreaded(true);
			this->device->setIdealFramerate(deviceID, (int) this->desiredFramerate);
			this->device->setRequestedMediaSubType(VI_MEDIASUBTYPE_MJPG);
			this->device->setupDevice(deviceID, width, height);
			this->device->setVideoSettingFilter(this->deviceID, this->device->propSharpness, 0);
			this->resetTimestamp();

			Specification specification(width, height, "videoInput", this->device->getDeviceName(this->deviceID));
			
			specification.addFeature(Feature::Feature_DeviceID);
			specification.addFeature(Feature::Feature_Exposure);
			specification.addFeature(Feature::Feature_FreeRun);
			specification.addFeature(Feature::Feature_Gain);
			specification.addFeature(Feature::Feature_Focus);
			specification.addFeature(Feature::Feature_Sharpness);
			
			specification.addPixelMode(PixelMode::Pixel_RGB8);

			this->frameIndex = 0;

			OFXMV_WARNING << "VideoInput. implements Exposure as a normalised range 0...1000. I.e. ignoring the unit us.";
			return specification;
		}

		//---------
		void VideoInput::close() {
			this->device->stopDevice(this->deviceID);
		}
		
		//---------
		bool VideoInput::startCapture() {
			OFXMV_WARNING << "startCapture is not used with VideoInput.";
			return true;
		}

		//---------
		void VideoInput::stopCapture() {
			OFXMV_WARNING << "stopCapture is not supported by VideoInput.";
		}

		//---------
		void VideoInput::getFrame(shared_ptr<Frame> frame) {
			LARGE_INTEGER timestampLong;

			ofPixels & pixels(frame->getPixelsRef());
			if (pixels.getWidth() != this->device->getWidth(this->deviceID) || pixels.getHeight() != this->device->getHeight(this->deviceID)) {
				pixels.allocate(this->device->getWidth(this->deviceID), this->device->getHeight(this->deviceID), OF_IMAGE_COLOR);
			}

			this->device->getPixels(this->deviceID, frame->getPixels(), true, true);
			QueryPerformanceCounter(&timestampLong);

			this->frameIndex++;

			frame->setTimestamp((Microseconds)((timestampLong.QuadPart - timerStart.QuadPart)* 1e6 / this->timerFrequency.QuadPart));
			frame->setFrameIndex(this->frameIndex);
		}

		//---------
		void VideoInput::setExposure(Microseconds exposure) {
			this->device->setVideoSettingCameraPct(this->deviceID, this->device->propExposure, (float) exposure / 1000000.0f);
		}
	
		//---------
		void VideoInput::setGain(float percent) {
			this->device->setVideoSettingCameraPct(this->deviceID, this->device->propGain, percent);
		}

		//---------
		void VideoInput::setFocus(float percent) {
			this->device->setVideoSettingCameraPct(this->deviceID, this->device->propFocus, percent);
		}

		//---------
		void VideoInput::setSharpness(float percent) {
			OFXMV_ERROR << "Error with setting sharpness, videoInput seems to be incompatible with this property right now";
			//this->device->setVideoSettingCameraPct(this->deviceID, this->device->propSharpness, percent);
		}

		//---------
		void VideoInput::showSettings() {
			this->device->showSettingsWindow(this->deviceID);
		}

		//---------
		void VideoInput::resetTimestamp() {
			QueryPerformanceCounter(&this->timerStart);
		}
	}
}

#endif