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

			Specification specification(CaptureSequenceType::Continuous
				, settings->width
				, settings->height
				, "videoInput"
				, this->device->getDeviceName(this->deviceIndex));
			
			specification.addPixelMode(PixelMode::RGB8);

			this->setupFloatParameter("Exposure", this->device->propExposure);
			this->setupFloatParameter("Gain", this->device->propGain);
			this->setupFloatParameter("Focus", this->device->propFocus);
			this->setupFloatParameter("Sharpness", this->device->propSharpness);

			this->frameIndex = 0;

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

			this->device->getPixels(this->deviceIndex, frame->getPixels().getData(), true, true);

			frame->setTimestamp(chrono::high_resolution_clock::now() - this->timerStart);
			frame->setFrameIndex(this->frameIndex);
			return frame;
		}

		//---------
		void VideoInput::showSettings() {
			this->device->showSettingsWindow(this->deviceIndex);
		}

		//---------
		void VideoInput::resetTimestamp() {
			this->timerStart = chrono::high_resolution_clock::now();
		}

		//----------
		void VideoInput::setupFloatParameter(string name, long propertyCode) {
			//get the initial settings
			long min, max, step, value, flags, defaultValue;
			this->device->getVideoSettingCamera(this->deviceIndex
				, propertyCode
				, min
				, max
				, step
				, value
				, flags
				, defaultValue);

			//construct the parameter
			auto parameter = make_shared<Parameter<float>>(ofParameter<float>{name
				, (float)value
				, (float)min
				, (float)max});

			parameter->getDeviceValueFunction = [this, propertyCode]() {
				long min, max, step, value, flags, defaultValue;

				this->device->getVideoSettingCamera(this->deviceIndex
					, propertyCode
					, min
					, max
					, step
					, value
					, flags
					, defaultValue);

				return (float)value;
			};

			parameter->getDeviceValueRangeFunction = [this, propertyCode](float & min, float & max) {
				long longMin, longMax, step, value, flags, defaultValue;

				this->device->getVideoSettingCamera(this->deviceIndex
					, propertyCode
					, longMin
					, longMax
					, step
					, value
					, flags
					, defaultValue);

				min = (float)longMin;
				max = (float)longMax;
			};

			parameter->setDeviceValueFunction = [this, propertyCode](const float & value) {
				this->device->setVideoSettingCamera(this->deviceIndex
					, propertyCode
					, value);
			};

			this->parameters.emplace_back(move(parameter));
		}
	}
}

#endif