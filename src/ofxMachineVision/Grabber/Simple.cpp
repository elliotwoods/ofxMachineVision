#include "Simple.h"
#include "ofAppRunner.h"
#include "ofSystemUtils.h"

namespace ofxMachineVision {
	namespace Grabber {
		//----------
		Simple::Simple() {
			this->useTexture = true;
			this->newFrameWaiting = false;
			this->currentFrameNew = false;

			this->fps = 0.0f;
			this->lastTimestamp = 0;
			this->lastFrameIndex = 0;
		}

		//----------
		Simple::~Simple() {
			this->close();
		}

		//----------
		void Simple::open(int deviceID) {
			this->close();

			if (!this->getIsDeviceExists()) {
				OFXMV_ERROR << "Cannot open device. Grabber has no Device";
				return;
			}

			try {
				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					{
						auto device = dynamic_pointer_cast<Device::Blocking>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Blocking"));
						}
						this->setFrame(shared_ptr<Frame>(new Frame()));
						this->thread->startThread();

						this->thread->performInThread([=] () {
							this->setSpecification(device->open(deviceID));
						});

						this->thread->setIdleFunction(
							[=] () {
							if (this->getIsDeviceRunning()) {
								shared_ptr<Frame> frame(new Frame());
								device->getFrame(frame);
								if (!frame->isEmpty()) {
									this->setFrame(frame);
									this->notifyNewFrame(frame);
								}
							} else {
								ofSleepMillis(2);
							}
						});
						this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;
						break;
					}
						
				case Device::Type_Updating:
					{
						auto device = dynamic_pointer_cast<Device::Updating>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Updating"));
						}
						this->setSpecification(device->open(deviceID));
						this->setFrame(shared_ptr<Frame>(new Frame()));
						this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;
						break;
					}
				case Device::Type_Callback:
					{
						auto device = dynamic_pointer_cast<Device::Callback>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Callback"));
						}
						this->setSpecification(device->open(deviceID));
						this->setFrame(shared_ptr<Frame>(new Frame()));
						this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;

						//callback on frames received
						device->onNewFrame += [this](shared_ptr<Frame> & incomingFrame) {
							//copy the incoming frame
							auto frameCopy = make_shared<Frame>();
							*frameCopy = *incomingFrame; //<- copy operator

							//fill our local copy
							this->setFrame(frameCopy);

							//alert the grabber's listeners
							this->notifyNewFrame(frameCopy);
						};
						break;
					}
				default:
					throw Exception("Device not implemented");
				}
			}
			catch (ofxMachineVision::Exception e) {
				OFXMV_ERROR << e.what();
				this->deviceState = State_Closed;
			}
			catch (std::exception e) {
				OFXMV_ERROR << e.what();
				this->deviceState = State_Closed;
			}
		}

		//----------
		void Simple::close() {
			if (this->getIsDeviceOpen()) {
				this->stopCapture();

				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					{
						this->thread->stopThread();
						this->thread->waitForThread(true);
					}
					break;
				case Device::Type_Updating:
				case Device::Type_Callback:
					this->getDevice()->close();
					break;
				case Device::Type_NotImplemented:
					break;
				}
				
				this->deviceState = State_Closed;
			}
		}

		//----------
		void Simple::startCapture(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN
			REQUIRES(Feature::Feature_FreeRun)

			this->setTriggerMode(triggerMode, triggerSignalType);
		
			this->callInRightThread([=] () {
				this->getDevice()->startCapture();
				this->deviceState = State_Running;
			});
		}

		//----------
		void Simple::stopCapture() {
			CHECK_OPEN
			REQUIRES(Feature::Feature_FreeRun)

			if (this->getIsDeviceRunning()) {
				this->callInRightThread([=]() {
					this->getDevice()->stopCapture();
					this->deviceState = State_Waiting;
				});
			}
		}

		//----------
		void Simple::singleShot() {
			CHECK_OPEN
			REQUIRES(Feature::Feature_OneShot);
			switch (this->getDeviceType()) {
				case Device::Type_Blocking:
				{
					auto device = dynamic_pointer_cast<Device::Blocking>(this->getDevice());
					this->thread->performInThread([device]() {
						device->singleShot();
					}, true);
					break;
				}
				case Device::Type_Updating:
				case Device::Type_Callback:
				{
					this->getDevice()->singleShot();
					break;
				}
				case Device::Type_NotImplemented:
				{
					break;
				}
			}
		}

		//----------
		shared_ptr<Frame> Simple::getFreshFrame(float timeout) {
			if (this->specification.supports(Feature::Feature_OneShot)) {
				this->singleShot();
				auto startTime = ofGetElapsedTimef();
				while (!this->isFrameNew()) {
					if (ofGetElapsedTimef() - startTime > timeout) {
						throw(ofxMachineVision::Exception("Timeout on getFreshFrame"));
					}
					this->update();
					ofSleepMillis(1);
				}
				return this->getFrame();
			} else if (this->specification.supports(Feature::Feature_FreeRun)) {
				shared_ptr<Frame> freshFrame(new Frame());
				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
				{
					auto device = dynamic_pointer_cast<Device::Blocking>(this->getDevice());
					this->thread->performInThread([device, freshFrame]() {
						device->getFrame(freshFrame);
					});
					break;
				}
				case Device::Type_Updating:
				{
					auto device = dynamic_pointer_cast<Device::Updating>(this->getDevice());
					device->updateIsFrameNew();
					while (true) {
						device->updateIsFrameNew();
						if (device->isFrameNew()) {
							freshFrame = device->getFrame();
							break;
						}
						ofSleepMillis(1);
					}
					break;
				}
				case Device::Type_Callback:
				{
					// Method : Wait for a new callback to come through from the Device
					this->newFrameWaiting = false;
					while (true) {
						if (this->newFrameWaiting) {
							freshFrame = this->frame;
							break;
						}
						ofSleepMillis(1);
					}
					break;
				}
				case Device::Type_NotImplemented:
					throw(ofxMachineVision::Exception("Single Shot not implemented for this device type"));
					break;
				}

				return freshFrame;
			}
			else {
				throw(ofxMachineVision::Exception("Your camera Device does not support capture (FreeRun or OneShot)"));
			}
		}

		//----------
		void Simple::update() {
			CHECK_OPEN_SILENT
			
			switch (this->getDeviceType()) {
				case Device::Type_Blocking:
				case Device::Type_Callback:
					currentFrameNew = this->newFrameWaiting;
					this->newFrameWaiting = false;
					break;
				case Device::Type_Updating:
				{
					auto device = dynamic_pointer_cast<Device::Updating>(this->getDevice());
					if (device) {
						device->updateIsFrameNew();
						this->currentFrameNew = device->isFrameNew();
						if (this->currentFrameNew) {
							auto frame = device->getFrame();
							this->setFrame(frame);
							this->notifyNewFrame(frame);
						}
					}
					break;
				}
				case Device::Type_NotImplemented:
					throw(ofxMachineVision::Exception("Simple::update is not implemented for your camera type"));
					break;
			}

			if (this->isFrameNew()) {
				auto frame = this->getFrame();
				auto & pixels = frame->getPixelsRef();
				if (!pixels.isAllocated()) {
					//we have no pixels allocated
					this->texture.clear();
					return;
				}
				if (this->useTexture) {
					if ((int) this->texture.getWidth() != pixels.getWidth() || (int) this->texture.getHeight() != pixels.getHeight()) {
						this->texture.allocate(pixels);
					}
					this->texture.loadData(pixels);
				}
			}
		}

		//----------
		void Simple::draw(float x, float y, float w, float h) {
			this->texture.draw(x, y, w, h);
		}

		//----------
		float Simple::getWidth() {
			auto frame = this->getFrame();
			if (frame) {
				return frame->getPixelsRef().getWidth();
			}
			else {
				return 0.0f;
			}
		}

		//----------
		float Simple::getHeight() {
			auto frame = this->getFrame();
			if (frame) {
				return frame->getPixelsRef().getHeight();
			}
			else {
				return 0.0f;
			}
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
			auto frame = this->getFrame();
			return frame->getPixelsRef();
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
		void Simple::setSharpness(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Sharpness)
			
			this->callInRightThread([=] () {
				this->getDevice()->setSharpness(percent);
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
		shared_ptr<Frame> Simple::getFrame() {
			this->framePointerLock.lock();
			auto frame = this->frame;
			this->framePointerLock.unlock();
			return frame;
		}

		//----------
		void Simple::setFrame(shared_ptr<Frame> frame) {
			this->framePointerLock.lock();
			this->frame = frame;
			this->framePointerLock.unlock();
		}

		//----------
		void Simple::callInRightThread(std::function<void()> function) {
			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->thread->performInThread(function);
				break;
			case Device::Type_Updating:
				function();
				break;
			case Device::Type_Callback:
				function();
				break;
			case Device::Type_NotImplemented:
				break;
			}
		}

		//----------
		void Simple::notifyNewFrame(shared_ptr<Frame> frame) {
			if (this->getDeviceState() == State_Deleting) {
				return;
			}

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

			FrameEventArgs frameEvent(this->getFrame());
			this->onNewFrameReceived(frameEvent);
		}
	}
}