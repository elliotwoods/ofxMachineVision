#include "VideoInput.h"

#include "ofConstants.h"
#ifdef TARGET_WIN32

namespace ofxMachineVision {
	namespace Device {
		//---------
		string VideoInput::getTypeName() const {
			return "VideoInput";
		}

		//----------
		std::vector<Device::Base::ListedDevice> VideoInput::listDevices() const {
			return vector<ListedDevice>();

			videoInput listDevicesDevice;
			auto deviceNames = listDevicesDevice.getDeviceList();

			vector<ListedDevice> deviceList(deviceNames.size());
			for (int i = 0; i < deviceNames.size(); i++) {
				deviceList[i] = ListedDevice{
					this->getDefaultSettings(),
					"DirectShow Device",
					deviceNames[i]
				};
				deviceList[i].initialisationSettings->deviceID = i;
			}
			return deviceList;
		}

		//---------
		Specification VideoInput::open(shared_ptr<Base::InitialisationSettings> initialisationSettings) {
			auto settings = this->getTypedSettings<InitialisationSettings>(initialisationSettings);

			this->device = make_shared<::videoInput>();
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

			return true;
		}

		//---------
		void VideoInput::stopCapture() {

		}

		//---------
		shared_ptr<Frame> VideoInput::getFrame() {
			auto frame = FramePool::X().getAvailableAllocatedFrame(this->device->getWidth(this->deviceIndex)
				, this->device->getHeight(this->deviceIndex)
				, ofPixelFormat::OF_PIXELS_RGB);

			bool success = this->device->getPixels(this->deviceIndex, frame->getPixels().getData(), true, true);

			if (success){
				frame->setTimestamp(chrono::high_resolution_clock::now() - this->timerStart);
				frame->setFrameIndex(this->frameIndex);
			}

			return frame;
		}

		//---------
		void VideoInput::setExposure(chrono::microseconds exposure) {
			this->device->setVideoSettingCameraPct(this->deviceIndex, this->device->propExposure, (float) exposure.count() / 1000000.0f);
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