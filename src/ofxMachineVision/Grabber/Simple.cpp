#include "Simple.h"
#include "ofAppRunner.h"

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
					{
						auto device = dynamic_pointer_cast<Device::Blocking>(this->getDevice());
						if (!device) {
							OFXMV_FATAL << "Type mismatch with Device::Blocking";
							throw(std::exception());
						}
					
						this->frame = shared_ptr<Frame>(new Frame());
						this->thread->startThread();

						this->thread->performInThread([=] () {
							this->setSpecification(device->open(deviceID));
						});

						this->thread->setIdleFunction(
							[=] () {
							if (this->getIsDeviceRunning()) {
								device->getFrame(this->frame);
								if (!this->frame->isEmpty()) {
									this->callbackNewFrame(this->frame);
								}
							} else {
								ofSleepMillis(2);
							}
						});
						this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;
						break;
					}
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
					{
						this->thread->stopThread();
						this->thread->waitForThread(true);
					}
					break;
				}
				this->deviceState = State_Closed;
			}
		}

		//----------
		void Simple::startCapture(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN

			this->setTriggerMode(triggerMode, triggerSignalType);
		
			this->callInRightThread([=] () {
				this->getDevice()->startCapture();
				this->deviceState = State_Running;
			});
		}

		//----------
		void Simple::stopCapture() {
			CHECK_OPEN

			this->callInRightThread([=] () {
				this->getDevice()->stopCapture();
				this->deviceState = State_Waiting;
			});
		}

		//----------
		void Simple::update() {
			CHECK_OPEN
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
			
			this->callInRightThread([=] () {
				this->getDevice()->setExposure(exposure);
			});
		}
		
		//----------
		void Simple::setGain(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Gain)

			this->callInRightThread([=] () {
				this->getDevice()->setGain(percent);
			});
		}

		//----------
		void Simple::setFocus(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Focus)
			
			this->callInRightThread([=] () {
				this->getDevice()->setFocus(percent);
			});
		}

		//----------
		void Simple::setBinning(int binningX, int binningY) {
			CHECK_OPEN
			REQUIRES(Feature_Binning)
			
			this->callInRightThread([=] () {
				this->getDevice()->setBinning(binningX, binningY);
			});
		}

		//----------
		void Simple::setROI(const ofRectangle & roi) {
			CHECK_OPEN
			REQUIRES(Feature_ROI);

			this->callInRightThread([=] () {
				this->getDevice()->setROI(roi);
			});
		}

		//----------
		void Simple::setTriggerMode(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN
			REQUIRES(Feature_Triggering)
			REQUIRES(triggerMode);
			REQUIRES(triggerSignalType);

			this->callInRightThread([=] () {
				this->getDevice()->setTriggerMode(triggerMode, triggerSignalType);
			});
		}

		//----------
		void Simple::setGPOMode(const GPOMode & gpoMode) {
			CHECK_OPEN
			REQUIRES(Feature_GPO);
			REQUIRES(gpoMode);

			this->callInRightThread([=] () {
				this->getDevice()->setGPOMode(gpoMode);
			});
		}

		//----------
		void Simple::callInRightThread(std::function<void()> function) {
			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->thread->performInThread(function);
				break;
			}
		}

		//----------
		void Simple::callbackNewFrame(shared_ptr<Frame> frame) {
			if (this->getDeviceState() == State_Deleting) {
				return;
			}


			frame->lockForReading();
			this->waitingPixelsLock.lock();
			this->waitingPixels = frame->getPixelsRef();
			this->waitingPixelsLock.unlock();
			frame->unlock();

			float interval = (frame->getTimestamp() - this->lastTimestamp) / 1e6;

			float newfps = (1.0f / interval);
			if (this->fps == this->fps && abs(log(this->fps) - log(newfps)) < 10) {
				this->fps = 0.9 * fps + 0.1f * newfps;
			} else {
				this->fps = newfps;
			}

			this->newFrameWaiting = true;
			this->lastTimestamp = frame->getTimestamp();
			this->lastFrameIndex = frame->getFrameIndex();

			FrameEventArgs frameEvent(frame);
			this->onNewFrameReceived(frameEvent);
		}
	}
}