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
			this->lastTimestamp = chrono::nanoseconds(0);
			this->lastFrameIndex = 0;
			this->clearCachedFrame();
		}

		//----------
		Simple::~Simple() {
			this->close();
		}

		//----------
		shared_ptr<Device::Base::InitialisationSettings> Simple::getDefaultInitialisationSettings() {
			if (!this->getIsDeviceExists()) {
				return make_shared<Device::Base::InitialisationSettings>();
			}
			else {
				return this->getDevice()->getDefaultSettings();
			}
		}

		//----------
		bool Simple::open(shared_ptr<Device::Base::InitialisationSettings> initialisationSettings) {
			this->close();

			if (!this->getIsDeviceExists()) {
				OFXMV_ERROR << "Cannot open device. Grabber has no Device";
				return false;
			}

			//if no settings are set, then use defaults
			if (initialisationSettings == nullptr) {
				initialisationSettings = this->getDevice()->getDefaultSettings();
			}

			this->getDevice()->initOnMainThread();

			this->clearCachedFrame();

			try {
				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					{
						auto device = dynamic_pointer_cast<Device::Blocking>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Blocking"));
						}

						//start up the thread
						this->thread = make_unique<Utils::ActionQueueThread>();

						//open the device in the thread
						this->thread->performInThread([this, device, initialisationSettings] () {
							try {
								this->setSpecification(device->open(initialisationSettings));
							}
							catch (ofxMachineVision::Exception e) {
								ofLogError("ofxMachineVision") << "Couldn't open device : " << e.what();
								this->clearDevice();
							}
							catch (const std::exception & e) {
								ofLogError("ofxMachineVision") << "Couldn't open device : " << e.what();
								this->clearDevice();
							}
						}, true);

						//set our device state if the device was opened
						this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;
						if (!this->getIsDeviceOpen()) {
							throw(ofxMachineVision::Exception("Failed to open device"));
						}

						//set the idle function to capture frames
						this->thread->setIdleFunction(
							[=]() {
							//--
							//Continuous capture function
							//--
							//
							if (this->getIsDeviceRunning()) {
								try {
									auto frame = device->getFrame();
									if (frame) {
										this->setFrame(frame);
										this->notifyNewFrame(frame);
									}
								}
								OFXMV_CATCH_ALL_TO_ERROR;
							}
							else {
								ofSleepMillis(2);
							}
							//
							//--
						});
						
						break;
					}
						
				case Device::Type_Updating:
					{
						auto device = dynamic_pointer_cast<Device::Updating>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Updating"));
						}
						this->setSpecification(device->open(initialisationSettings));
						this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;
						break;
					}
				case Device::Type_Callback:
					{
						auto device = dynamic_pointer_cast<Device::Callback>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Callback"));
						}
						this->setSpecification(device->open(initialisationSettings));
						this->deviceState = this->getDeviceSpecification().getValid() ? State_Waiting : State_Closed;

						//callback on frames received
						device->onNewFrame += [this](shared_ptr<Frame> & frame) {
							//fill our local pointer
							this->setFrame(frame);

							//alert the grabber's listeners
							this->notifyNewFrame(frame);
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

			return this->getIsDeviceOpen();
		}

		//----------
		void Simple::close() {
			if (this->getIsDeviceOpen()) {
				this->stopCapture();
				
				this->callInRightThread([this]() {
					this->getDevice()->close();
				}, true);

				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
					this->thread.reset();
					break;
				case Device::Type_Updating:
				case Device::Type_Callback:
				case Device::Type_NotImplemented:
				default:
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
			}, true);
		}

		//----------
		void Simple::stopCapture() {
			CHECK_OPEN
			REQUIRES(Feature::Feature_FreeRun)

			if (this->getIsDeviceRunning()) {
				this->callInRightThread([=]() {
					this->getDevice()->stopCapture();
					this->deviceState = State_Waiting;
				}, true);
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
		shared_ptr<Frame> Simple::getFreshFrame(chrono::microseconds timeout) {
			shared_ptr<Frame> frame;

			auto startTime = chrono::system_clock::now();

			if (this->specification.supports(Feature::Feature_OneShot)) {
				
				// --
				// One Shot
				// --
				// Trigger a capture, use our local 'update / isFrameNew' pattern
				
				this->singleShot();
				while (!this->isFrameNew()) {
					if (chrono::system_clock::now() - startTime > timeout) {
						throw(ofxMachineVision::Exception("Timeout on getFreshFrame"));
					}
					this->update();
					ofSleepMillis(1);
				}
				frame = this->getFrame();

			} else if (this->specification.supports(Feature::Feature_FreeRun)) {
				
				// --
				// Free Run
				// --
				// Varies by device type

				switch (this->getDeviceType()) {
				case Device::Type_Blocking:
				{
					// --
					// Blocking
					// --
					//
					//perform a blocking capture in the device thread
					auto device = dynamic_pointer_cast<Device::Blocking>(this->getDevice());
					this->thread->performInThread([device, &frame]() {
						frame = device->getFrame();
					}, true);
					//
					// --
					break;
				}
				case Device::Type_Updating:
				{
					// --
					// Updating
					// --
					//
					//keep updating until we get another frame
					auto device = dynamic_pointer_cast<Device::Updating>(this->getDevice());
					device->updateIsFrameNew();
					while (true) {
						device->updateIsFrameNew();
						if (device->isFrameNew()) {
							frame = device->getFrame();
							break;
						}
						if (chrono::system_clock::now() - startTime > timeout) {
							throw(ofxMachineVision::Exception("Timeout on getFreshFrame"));
						}
						ofSleepMillis(1);
					}
					//
					// --
					break;
				}
				case Device::Type_Callback:
				{
					// --
					// Callback
					// --
					//
					// Method : Wait for a new callback to come through from the Device
					this->newFrameWaiting = false;
					while (true) {
						if (this->newFrameWaiting) {
							frame = this->frame;
							break;
						}
						if (chrono::system_clock::now() - startTime > timeout) {
							throw(ofxMachineVision::Exception("Timeout on getFreshFrame"));
						}
						ofSleepMillis(1);
					}
					//
					// --
					break;
				}
				case Device::Type_NotImplemented:
					throw(ofxMachineVision::Exception("getFreshFrame not implemented for this device type"));
					break;
				}
			} else {
				throw(ofxMachineVision::Exception("Your camera Device does not support capture (FreeRun or OneShot)"));
			}

			return frame;
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
				//update our local pixels cache
				auto frame = this->getFrame();
				this->pixels = frame->getPixels();

				if (this->useTexture) {
					//if we have no pixels, clear the texture
					if (!this->pixels.isAllocated()) {
						this->texture.clear();
						return;
					}
					else {
						//check if we have to reallocate texture
						if ((int) this->texture.getWidth() != this->pixels.getWidth() || (int) this->texture.getHeight() != this->pixels.getHeight()) {
							this->texture.allocate(this->pixels);
						}
						//load the pixels into the texture
						this->texture.loadData(this->pixels);
					}
				}
			}
		}

		//----------
		void Simple::draw(float x, float y, float w, float h) const {
			this->texture.draw(x, y, w, h);
		}

		//----------
		float Simple::getWidth() const {
			auto frame = this->getFrame();
			if (frame) {
				return frame->getPixels().getWidth();
			}
			else {
				return 0.0f;
			}
		}

		//----------
		float Simple::getHeight() const {
			auto frame = this->getFrame();
			if (frame) {
				return frame->getPixels().getHeight();
			}
			else {
				return 0.0f;
			}
		}

		//----------
		ofTexture & Simple::getTexture() {
			return this->texture;
		}

		//----------
		const ofTexture & Simple::getTexture() const {
			return this->texture;
		}

		//----------
		void Simple::setUseTexture(bool useTexture) {
			this->useTexture = useTexture;

			if (!useTexture) {
				this->texture.clear();
			}
		}

		//----------
		bool Simple::isUsingTexture() const {
			return this->useTexture;
		}

		//----------
		const ofPixels & Simple::getPixels() const {
			return this->pixels;
		}

		//----------
		ofPixels & Simple::getPixels() {
			return this->pixels;
		}

		//----------
		void Simple::setExposure(chrono::microseconds exposure) {
			CHECK_OPEN
			REQUIRES(Feature_Exposure)
			
			this->callInRightThread([=] () {
				this->getDevice()->setExposure(exposure);
			}, false);
		}
		
		//----------
		void Simple::setGain(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Gain)

			this->callInRightThread([=] () {
				this->getDevice()->setGain(percent);
			}, false);
		}

		//----------
		void Simple::setFocus(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Focus)
			
			this->callInRightThread([=] () {
				this->getDevice()->setFocus(percent);
			}, false);
		}

		//----------
		void Simple::setSharpness(float percent) {
			CHECK_OPEN
			REQUIRES(Feature_Sharpness)
			
			this->callInRightThread([=] () {
				this->getDevice()->setSharpness(percent);
			}, false);
		}

		//----------
		void Simple::setBinning(int binningX, int binningY) {
			CHECK_OPEN
			REQUIRES(Feature_Binning)
			
			this->callInRightThread([=] () {
				this->getDevice()->setBinning(binningX, binningY);
			}, false);
		}

		//----------
		void Simple::setROI(const ofRectangle & roi) {
			CHECK_OPEN
			REQUIRES(Feature_ROI);

			this->callInRightThread([=] () {
				this->getDevice()->setROI(roi);
			}, false);
		}

		//----------
		void Simple::setTriggerMode(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
			CHECK_OPEN
			REQUIRES(Feature_Triggering)
			REQUIRES(triggerMode);
			REQUIRES(triggerSignalType);

			this->callInRightThread([=] () {
				this->getDevice()->setTriggerMode(triggerMode, triggerSignalType);
			}, false);
		}

		//----------
		void Simple::setGPOMode(const GPOMode & gpoMode) {
			CHECK_OPEN
			REQUIRES(Feature_GPO);
			REQUIRES(gpoMode);

			this->callInRightThread([=] () {
				this->getDevice()->setGPOMode(gpoMode);
			}, false);
		}

		//----------
		shared_ptr<Frame> Simple::getFrame() const {
			this->framePointerMutex.lock();
			{
				auto frame = this->frame;
			}
			this->framePointerMutex.unlock();
			return frame;
		}

		//----------
		void Simple::callInRightThread(std::function<void()> function, bool blocking) {
			switch (this->getDeviceType()) {
			case Device::Type_Blocking:
				this->thread->performInThread(move(function), blocking);
				break;
			case Device::Type_Updating:
				try {
					function();
				}
				OFXMV_CATCH_ALL_TO_ERROR
				break;
			case Device::Type_Callback:
				try {
					function();
				}
				OFXMV_CATCH_ALL_TO_ERROR
				break;
			default:
				break;
			}
		}

		//----------
		void Simple::notifyNewFrame(shared_ptr<Frame> frame) {
			if (this->getDeviceState() == State_Deleting) {
				return;
			}

			chrono::duration<float> interval = frame->getTimestamp() - this->lastTimestamp;

			float newfps = (1.0f / interval.count());

			//damp the fps to make it more readable
			if (this->fps == this->fps && abs(log(this->fps) - log(newfps)) < 10) {
				this->fps = 0.9 * fps + 0.1f * newfps;
			} else {
				this->fps = newfps;
			}

			this->newFrameWaiting = true;
			this->lastTimestamp = frame->getTimestamp();
			this->lastFrameIndex = frame->getFrameIndex();

			this->onNewFrameReceived(this->getFrame());
		}


		//----------
		void Simple::setFrame(shared_ptr<Frame> frame) {
			this->framePointerMutex.lock();
			{
				this->frame = frame;
			}
			this->framePointerMutex.unlock();
		}

		//----------
		void Simple::clearCachedFrame() {
			this->setFrame(FramePool::X().getCleanFrame());
		}
	}
}