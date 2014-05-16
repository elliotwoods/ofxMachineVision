#include "Blocking.h"
#include "ofxMachineVision/Grabber/Base.h"

namespace ofxMachineVision {
	namespace Grabber {
		namespace Thread {
#pragma mark Action
			//----------
			Blocking::Action::Action(const Type & type) {
				this->type = type;
			}
			
			//----------
			Blocking::Action::Action(const Type & type, Poco::Any arguments) {
				this->type = type;
				this->arguments = arguments;
			}

#pragma mark Blocking
			//----------
			Blocking::Blocking(shared_ptr<Device::Blocking> device, Grabber::Base * grabber) {
				this->device = device;
				this->grabber = grabber;
				this->frame = shared_ptr<Frame>(new Frame());
			}

			//----------
			Blocking::~Blocking() {
				this->close();
				bool waitForActionQueue = true;
				while (true) {
					this->actionQueueLock.lock();
					if (this->actionQueue.empty()) {
						waitForActionQueue = false;
					}
					this->actionQueueLock.unlock();
					if (waitForActionQueue) {
						sleep(10);
					} else {
						break;
					}
				}
				actionQueueLock.unlock();
			}

			//----------
			void Blocking::open(int deviceID) {
				this->startThread();
				this->addAction(Action(Action::Type_Open, deviceID), true);
			}

			//----------
			void Blocking::close() {
				this->addAction(Action::Type_Close, true);
				this->stopThread();
			}

			//----------
			void Blocking::startFreeRun() {
				this->addAction(Action::Type_StartFreeRun, true);
			}

			//----------
			void Blocking::stopFreeRun() {
				this->addAction(Action::Type_StopFreeRun, true);
			}

			//----------
			void Blocking::setExposure(Microseconds exposure) {
				this->addAction(Action(Action::Type_SetExposure, exposure), false);
			}

			//----------
			void Blocking::setGain(float percent) {
				this->addAction(Action(Action::Type_SetGain, percent), false);
			}

			//----------
			void Blocking::setFocus(float percent) {
				this->addAction(Action(Action::Type_SetFocus, percent), false);
			}

			//----------
			void Blocking::setBinning(int binningX, int binningY) {
				Binning binning = {binningX, binningY};
				this->addAction(Action(Action::Type_SetBinning, binning), false);
			}

			//----------
			void Blocking::setROI(const ofRectangle& roi) {
				this->addAction(Action(Action::Type_SetROI, roi), false);
			}
			
			//----------
			void Blocking::setTriggerMode(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
				TriggerSettings triggerSettings(triggerMode, triggerSignalType);
				this->addAction(Action(Action::Type_SetTriggerSettings, triggerSettings), false);
			}

			//----------
			void Blocking::setGPOMode(const GPOMode & gpoMode) {
				this->addAction(Action(Action::Type_SetGPOMode, gpoMode), false);
			}

			//----------
			void Blocking::addAction(const Action & action, bool blockUntilComplete) {
				this->actionQueueLock.lock();
				this->actionQueue.push(action);
				this->actionQueueLock.unlock();
				
				if (blockUntilComplete) {
					this->blockUntilActionQueueEmpty();
				}
			}
	
			//----------
			void Blocking::blockUntilActionQueueEmpty() {
				bool queueEmpty = false;
		
				while (!queueEmpty) {
					this->actionQueueLock.lock();
					queueEmpty = this->actionQueue.empty();
					this->actionQueueLock.unlock();
				}
			}

			//----------
			void Blocking::threadedFunction() {
				while (this->isThreadRunning()) {
					//process any requested actions
					this->actionQueueLock.lock();
					if (!this->actionQueue.empty()) {
						while (!this->actionQueue.empty()) {
							bool clearedQueue = false;
							Action & action(this->actionQueue.front());
							switch (action.getType()) {
							case Action::Type_Open:
							{
								int deviceID = action.getArgument<int>();
								const Specification specification = this->device->open(deviceID);
								this->grabber->setSpecification(specification);
								break;
							}
							case Action::Type_Close:
								this->device->close();
								while (!this->actionQueue.empty())
									this->actionQueue.pop();
								clearedQueue = true;
								break;
							case Action::Type_StartFreeRun:
								this->device->startCapture();
								break;
							case Action::Type_StopFreeRun:
								this->device->stopCapture();
								break;
							case Action::Type_SetExposure:
								{
									const Microseconds & exposure = action.getArgument<Microseconds>();
									this->device->setExposure(exposure);
								}
								break;
							case Action::Type_SetGain:
								{
									const float & gain = action.getArgument<float>();
									this->device->setGain(gain);
								}
								break;
							case Action::Type_SetFocus:
								{
									const float & focus = action.getArgument<float>();
									this->device->setFocus(focus);
								}
								break;
							case Action::Type_SetBinning:
								{
									const array<int, 2> & binning = action.getArrayArgument<int, 2>();
									this->device->setBinning(binning[0], binning[1]);
								}
								break;
							case Action::Type_SetROI:
								{
									const ofRectangle & roi = action.getArgument<ofRectangle>();
									this->device->setROI(roi);
								}
								break;
							case Action::Type_SetTriggerSettings:
								{
									const TriggerSettings & triggerSettings = action.getArgument<TriggerSettings>();
									this->device->setTriggerMode(triggerSettings.first, triggerSettings.second);
								}
								break;
							case Action::Type_SetGPOMode:
								{
									const GPOMode & gpoMode = action.getArgument<GPOMode>();
									this->device->setGPOMode(gpoMode);
								}
							}
							if (!clearedQueue) {
								//we clear the queue when closing
								actionQueue.pop();
							}
						}
						actionQueueLock.unlock();
					} else {
						actionQueueLock.unlock();
						if (this->grabber->getIsDeviceRunning()) {
							//pull a frame from the device
							this->device->getFrame(*this->frame);
							if (!this->frame->isEmpty()) {
								FrameEventArgs eventArgs = FrameEventArgs(frame);
								ofNotifyEvent(this->evtNewFrame, eventArgs, this); 
							}
						} else {
							//the device is open but waiting for instructions
							ofThread::sleep(1);
						}
					}
				}

				device->close();
			}
		}
	}
}