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
			Blocking::Blocking(Device::Blocking * device, Grabber::Base * grabber) {
				this->device = device;
				this->grabber = grabber;
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
			void Blocking::setBinning(int binningX, int binningY) {
				Binning binning = {binningX, binningY};
				this->addAction(Action(Action::Type_SetBinning, binning), true);
			}

			//----------
			void Blocking::setROI(const ofRectangle& roi) {
				this->addAction(Action(Action::Type_SetROI, roi), true);
			}
			
			//----------
			void Blocking::setTriggerMode(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
				TriggerSettings triggerSettings(triggerMode, triggerSignalType);
				this->addAction(Action(Action::Type_SetTriggerSettings, triggerSettings), true);
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
					while (!this->actionQueue.empty()) {
						Action & action(this->actionQueue.front());
						switch (action.getType()) {
						case Action::Type_Open:
							this->grabber->setSpecification(this->device->open(action.getArgument<int>()));
							break;
						case Action::Type_Close:
							this->device->close();
							while (!this->actionQueue.empty())
								this->actionQueue.pop();
							this->actionQueueLock.unlock();
						case Action::Type_StartFreeRun:
							this->device->startCapture();
							break;
						case Action::Type_StopFreeRun:
							this->device->stopCapture();
							break;
						case Action::Type_SetBinning:
							{
								array<int, 2> binning = action.getArrayArgument<int, 2>();
								this->device->setBinning(binning[0], binning[1]);
							}
							break;
						case Action::Type_SetROI:
							{
								ofRectangle roi = action.getArgument<ofRectangle>();
								this->device->setROI(roi);
							}
							break;
						case Action::Type_SetTriggerSettings:
							{
								TriggerSettings triggerSettings = action.getArgument<TriggerSettings>();
								this->device->setTriggerMode(triggerSettings.first, triggerSettings.second);
							}
							break;
						}
						actionQueue.pop();
					}
					actionQueueLock.unlock();
		
					if (this->grabber->getIsDeviceRunning()) {
						//pull a frame from the device
						this->device->getFrame(this->frame);
						ofNotifyEvent(this->evtNewFrame, this->frame, this); 
					} else {
						//the device is open but waiting for instructions
						ofThread::sleep(1);
					}
				}

				device->close();
			}
		}
	}
}