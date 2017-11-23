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
				case Device::Type::Blocking:
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
						this->deviceState = this->getDeviceSpecification().getValid() ? DeviceState::Waiting : DeviceState::Closed;
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

							//note : we don't do anything here for one-shot cameras
						});

						break;
					}
						
				case Device::Type::Updating:
					{
						auto device = dynamic_pointer_cast<Device::Updating>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Updating"));
						}
						this->setSpecification(device->open(initialisationSettings));
						this->deviceState = this->getDeviceSpecification().getValid() ? DeviceState::Waiting : DeviceState::Closed;
						break;
					}
				case Device::Type::Callback:
					{
						auto device = dynamic_pointer_cast<Device::Callback>(this->getDevice());
						if (!device) {
							throw(ofxMachineVision::Exception("Type mismatch with Device::Callback"));
						}
						this->setSpecification(device->open(initialisationSettings));
						this->deviceState = this->getDeviceSpecification().getValid() ? DeviceState::Waiting : DeviceState::Closed;

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

				this->lastInitialisationSettingsUsed = initialisationSettings;
			}
			catch (ofxMachineVision::Exception e) {
				OFXMV_ERROR << e.what();
				this->deviceState = DeviceState::Closed;
			}
			catch (std::exception e) {
				OFXMV_ERROR << e.what();
				this->deviceState = DeviceState::Closed;
			}

			return this->getIsDeviceOpen();
		}

		//----------
		void Simple::close() {
			this->stopCapture();

			if (this->getIsDeviceOpen()) {
				this->callInRightThread([this]() {
					this->getDevice()->close();
				}, true);

				switch (this->getDeviceType()) {
				case Device::Type::Blocking:
					this->thread.reset();
					break;
				case Device::Type::Updating:
				case Device::Type::Callback:
				case Device::Type::NotImplemented:
				default:
					break;
				}
				
				this->deviceState = DeviceState::Closed;
			}
		}

		//----------
		void Simple::startCapture() {
			CHECK_OPEN;
			REQUIRES(CaptureSequenceType::Continuous);

			this->callInRightThread([=] () {
				this->getDevice()->startCapture();
				this->deviceState = DeviceState::Running;
			}, true);
		}

		//----------
		void Simple::stopCapture() {
			CHECK_OPEN;
			REQUIRES(CaptureSequenceType::Continuous);

			if (this->getIsDeviceRunning()) {
				this->callInRightThread([=]() {
					this->getDevice()->stopCapture();
					this->deviceState = DeviceState::Waiting;
				}, true);
			}
		}

		//----------
		void Simple::singleShot() {
			CHECK_OPEN;
			REQUIRES(CaptureSequenceType::OneShot);
			switch (this->getDeviceType()) {
				case Device::Type::Blocking:
				{
					auto device = dynamic_pointer_cast<Device::Blocking>(this->getDevice());

					shared_ptr<ofxMachineVision::Frame> frame;

					this->thread->performInThread([device, & frame]() {
						frame = device->getFrame();
					}, true);

					if (frame) {
						this->setFrame(frame);
						this->notifyNewFrame(frame);
					}

					break;
				}
				case Device::Type::Updating:
				case Device::Type::Callback:
				{
					this->getDevice()->singleShot();
					break;
				}
				case Device::Type::NotImplemented:
				{
					break;
				}
			}
		}

		//----------
		void Simple::reopen() {
			auto wasDeviceRunning = this->getIsDeviceRunning();
			this->close();
			this->open(lastInitialisationSettingsUsed); // open handles the case where this is empty by using default settings
		}

		//----------
		bool Simple::isSingleShot() const {
			return this->specification.getCaptureSequenceType() == CaptureSequenceType::OneShot;
		}

		//----------
		shared_ptr<Frame> Simple::getFreshFrame(chrono::microseconds timeout) {
			shared_ptr<Frame> frame;

			auto startTime = chrono::system_clock::now();

			if (this->specification.getCaptureSequenceType() == CaptureSequenceType::OneShot) {
				
				// --
				// One Shot
				// --
				// Trigger a capture, use our local 'update / isFrameNew' pattern
				
				this->singleShot();
				this->update(); //clear any existing new frame marked this frame
				while (!this->isFrameNew()) {
					if (chrono::system_clock::now() - startTime > timeout) {
						throw(ofxMachineVision::Exception("Timeout on getFreshFrame"));
					}
					this->update();
					ofSleepMillis(1);
				}
				frame = this->getFrame();

			} else if (this->specification.getCaptureSequenceType() == CaptureSequenceType::Continuous) {
				
				// --
				// Free Run
				// --
				// Varies by device type

				switch (this->getDeviceType()) {
				case Device::Type::Blocking:
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
				case Device::Type::Updating:
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
				case Device::Type::Callback:
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
				case Device::Type::NotImplemented:
					throw(ofxMachineVision::Exception("getFreshFrame not implemented for this device type"));
					break;
				}
			} else {
				throw(ofxMachineVision::Exception("Your camera Device does not support capture (FreeRun or OneShot)"));
			}

			if (frame) {
				this->notifyNewFrame(frame);
			}
			return frame;
		}

		//----------
		void Simple::update() {
			CHECK_OPEN_SILENT
			
			switch (this->getDeviceType()) {
				case Device::Type::Blocking:
				case Device::Type::Callback:
					currentFrameNew = this->newFrameWaiting;
					this->newFrameWaiting = false;
					break;
				case Device::Type::Updating:
				{
					auto device = dynamic_pointer_cast<Device::Updating>(this->getDevice());
					if (device) {
						device->updateIsFrameNew();
						this->currentFrameNew = device->isFrameNew();
						if (this->currentFrameNew) {
							auto frame = device->getFrame();
							if (frame) {
								this->setFrame(frame);
								this->notifyNewFrame(frame);
							}
						}
					}
					break;
				}
				case Device::Type::NotImplemented:
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
		const vector<shared_ptr<AbstractParameter>> Simple::getDeviceParameters() const {
			auto device = this->getDevice();
			if (device) {
				return device->getParameters();
			}
			else {
				return vector<shared_ptr<AbstractParameter>>();
			}
		}

		//----------
		void Simple::syncToDevice(AbstractParameter & parameter) {
			this->callInRightThread([&parameter]() {
				parameter.syncToDevice();
				parameter.syncFromDevice();
			}, true);
		}

		//----------
		void Simple::syncFromDevice(AbstractParameter & parameter) {
			this->callInRightThread([&parameter]() {
				parameter.syncFromDevice();
			}, true);
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
			case Device::Type::Blocking:
				this->thread->performInThread(move(function), blocking);
				break;
			case Device::Type::Updating:
				try {
					function();
				}
				OFXMV_CATCH_ALL_TO_ERROR
				break;
			case Device::Type::Callback:
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
			if (this->getDeviceState() == DeviceState::Deleting) {
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