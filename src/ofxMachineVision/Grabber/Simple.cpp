#include "Simple.h"

namespace ofxMachineVision {
	namespace Grabber {
		//----------
		Simple::Simple(DevicePtr device) : Base(device) {
			this->useTexture = true;
			this->newFrameWaiting = false;
			this->currentFrameNew = false;

			this->fps = 0.0f;
			this->lastTimestamp = 0;
			this->lastFrameIndex = 0;
		}

		//----------
		void Simple::open(int deviceID) {
			this->close();

			try {
				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					this->threadBlocking->open(deviceID);
					this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;
					ofAddListener(this->threadBlocking->evtNewFrame, this, &Simple::callbackNewFrame);
					break;
				default:
					throw std::exception("Device not implemented");
				}
			} catch (std::exception e) {
				OFXMV_ERROR << e.what();
				this->deviceState = State_Closed;
			}
		}

		//----------
		void Simple::close() {
			if (this->getIsDeviceRunning()) {
				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					this->threadBlocking->close();
					this->deviceState = State_Closed;
					ofRemoveListener(this->threadBlocking->evtNewFrame, this, &Simple::callbackNewFrame);
					break;
				}
			}
		}

		//----------
		void Simple::startCapture(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN

			this->setTriggerMode(triggerMode, triggerSignalType);
		
			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->startFreeRun();
				this->deviceState = State_Running;
				break;
			}
		}

		//----------
		void Simple::stopCapture() {
			CHECK_OPEN
			switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					this->threadBlocking->stopFreeRun();
					break;
			}
		}

		//----------
		void Simple::update() {
			currentFrameNew = this->newFrameWaiting;
			this->newFrameWaiting = false;

			if (this->isFrameNew()) {
				this->waitingPixelsLock.lock();
				std::swap(this->pixels, this->waitingPixels);
				this->waitingPixelsLock.unlock();

				if (!this->pixels.isAllocated()) {
					//we have no pixels allocated
					this->texture.clear();
					return;
				}
				if (this->useTexture) {
					if ((int) this->texture.getWidth() != this->pixels.getWidth() || (int) this->texture.getHeight() != this->pixels.getHeight()) {
						this->texture.allocate(this->pixels);
					}
					this->texture.loadData(this->pixels);
				}
			}
		}

		//----------
		void Simple::draw(float x, float y, float w, float h) {
			this->texture.draw(x, y, w, h);
		}

		//----------
		float Simple::getWidth() {
			return this->pixels.getWidth();
		}

		//----------
		float Simple::getHeight() {
			return this->pixels.getHeight();
		}

		//----------
		void Simple::setUseTexture(bool useTexture) {
			this->useTexture = useTexture;

			if (!useTexture) {
				this->texture.clear();
			}
		}

		//----------
		unsigned char * Simple::getPixels() {
			return this->getPixelsRef().getPixels();
		}

		//----------
		ofPixels & Simple::getPixelsRef() {
			return this->pixels;
		}

		//----------
		void Simple::setExposure(Microseconds exposure) {
			CHECK_OPEN
			REQUIRES(Feature_Exposure)
			
			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setExposure(exposure);
				break;
			}
		}
		
		//----------
		void Simple::setGain(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Gain)

			switch(this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setGain(percent);
			}
		}

		//----------
		void Simple::setFocus(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Focus)

			switch(this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setFocus(percent);
			}
		}

		//----------
		void Simple::setBinning(int binningX, int binningY) {
			CHECK_OPEN
			REQUIRES(Feature_Binning)

			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setBinning(binningX, binningY);
				break;
			}
		}

		//----------
		void Simple::setROI(const ofRectangle & roi) {
			CHECK_OPEN
			REQUIRES(Feature_ROI);

			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setROI(roi);
				break;
			}
		}

		//----------
		void Simple::setTriggerMode(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN
			REQUIRES(Feature_Triggering)
			REQUIRES(triggerMode);
			REQUIRES(triggerSignalType);

			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setTriggerMode(triggerMode, triggerSignalType);
			}
		}

		//----------
		void Simple::setGPOMode(const GPOMode & gpoMode) {
			CHECK_OPEN
			REQUIRES(Feature_GPO);
			REQUIRES(gpoMode);

			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->threadBlocking->setGPOMode(gpoMode);
			}
		}

		//----------
		void Simple::callbackNewFrame(FrameEventArgs & frameEventArgs) {
			if (this->getDeviceState() == State_Deleting) {
				return;
			}

			Frame & frame(*frameEventArgs.frame);

			frame.lockForReading();
			this->waitingPixelsLock.lock();
			this->waitingPixels = frame.getPixelsRef();
			this->waitingPixelsLock.unlock();
			frame.unlock();

			float interval = (frame.getTimestamp() - this->lastTimestamp) / 1e6;

			float newfps = (1.0f / interval);
			if (this->fps == this->fps && abs(log(this->fps) - log(newfps)) < 10) {
				this->fps = 0.9 * fps + 0.1f * newfps;
			} else {
				this->fps = newfps;
			}

			this->newFrameWaiting = true;
			this->lastTimestamp = frame.getTimestamp();
			this->lastFrameIndex = frame.getFrameIndex();

			if (!this->newFrameReceived.empty()) {
				ofNotifyEvent(this->newFrameReceived, frameEventArgs, this);
			}
		}
	}
}