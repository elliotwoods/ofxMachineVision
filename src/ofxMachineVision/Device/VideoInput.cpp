#include "VideoInput.h"

#include "ofConstants.h"
#ifdef TARGET_WIN32

namespace ofxMachineVision {
	namespace Device {
		//---------
		string VideoInput::getTypeName() const {
			return "VideoInput";
		}

		//---------
		Specification VideoInput::open(shared_ptr<Base::InitialisationSettings> initialisationSettings) {
			auto settings = this->getTypedSettings<InitialisationSettings>(initialisationSettings);

			if (!this->device) {
				this->device = make_shared<::videoInput>();
			}
			this->deviceIndex = settings->deviceID;

			this->device->setComMultiThreaded(true);
			this->device->setIdealFramerate(this->deviceIndex, (int) settings->idealFrameRate);
			this->device->setRequestedMediaSubType(VI_MEDIASUBTYPE_MJPG);
			this->device->setupDevice(this->deviceIndex, settings->width, settings->height);
			this->device->setVideoSettingFilter(this->deviceIndex, this->device->propSharpness, 0);
			this->resetTimestamp();

			Specification specification(settings->width, settings->height, "videoInput", this->device->getDeviceName(this->deviceIndex));
			
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
			this->device->stopDevice(this->deviceIndex);
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

			ofPixels & pixels(frame->getPixels());
			if (pixels.getWidth() != this->device->getWidth(this->deviceIndex) || pixels.getHeight() != this->device->getHeight(this->deviceIndex)) {
				pixels.allocate(this->device->getWidth(this->deviceIndex), this->device->getHeight(this->deviceIndex), OF_IMAGE_COLOR);
			}

			this->device->getPixels(this->deviceIndex, frame->getPixels().getData(), true, true);
			QueryPerformanceCounter(&timestampLong);

			this->frameIndex++;

			frame->setTimestamp((Microseconds)chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - this->timerStart).count());
			frame->setFrameIndex(this->frameIndex);
		}

		//---------
		void VideoInput::setExposure(Microseconds exposure) {
			this->device->setVideoSettingCameraPct(this->deviceIndex, this->device->propExposure, (float) exposure / 1000000.0f);
		}
	
		//---------
		void VideoInput::setGain(float percent) {
			this->device->setVideoSettingCameraPct(this->deviceIndex, this->device->propGain, percent);
		}

		//---------
		void VideoInput::setFocus(float percent) {
			this->device->setVideoSettingCameraPct(this->deviceIndex, this->device->propFocus, percent);
		}

		//---------
		void VideoInput::setSharpness(float percent) {
			OFXMV_ERROR << "Error with setting sharpness, videoInput seems to be incompatible with this property right now";
			//this->device->setVideoSettingCameraPct(this->deviceID, this->device->propSharpness, percent);
		}

		//---------
		void VideoInput::showSettings() {
			this->device->showSettingsWindow(this->deviceIndex);
		}

		//---------
		void VideoInput::resetTimestamp() {
			this->timerStart = chrono::high_resolution_clock::now();
		}
	}
}

#endif