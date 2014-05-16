#include "Blocking.h"
#include "ofxMachineVision/Grabber/Base.h"

namespace ofxMachineVision {
	namespace Grabber {
		namespace Thread {
			//----------
			Blocking::Blocking(shared_ptr<Device::Blocking> device, Grabber::Base * grabber) {
				this->device = device;
				this->grabber = grabber;
				this->frame = shared_ptr<Frame>(new Frame());
			}

			//----------
			Blocking::~Blocking() {
				this->close();
			}

			//----------
			void Blocking::open(int deviceID) {
				this->startThread();
				this->perform([this, deviceID] () {
					const Specification specification = this->device->open(deviceID);
					this->grabber->setSpecification(specification);
				}, true);
			}

			//----------
			void Blocking::close() {
				this->perform([this] () {
					this->device->close();
				}, true);
				this->stopThread();
			}

			//----------
			void Blocking::startFreeRun() {
				this->perform([this] () {
					this->device->startCapture();
				}, true);
			}

			//----------
			void Blocking::stopFreeRun() {
				this->perform([this] () {
					this->device->stopCapture();
				}, true);
			}

			//----------
			void Blocking::setExposure(Microseconds exposure) {
				this->perform([=] () {
					this->device->setExposure(exposure);
				}, false);
			}

			//----------
			void Blocking::setGain(float percent) {
				this->perform([=] () {
					this->device->setGain(percent);
				}, false);
			}

			//----------
			void Blocking::setFocus(float percent) {
				this->perform([=] () {
					this->device->setFocus(percent);
				}, false);
			}

			//----------
			void Blocking::setBinning(int binningX, int binningY) {
				this->perform([=] () {
					this->device->setBinning(binningX, binningY);
				}, false);
			}

			//----------
			void Blocking::setROI(const ofRectangle& roi) {
				this->perform([=] () {
					this->device->setROI(roi);
				}, false);
			}
			
			//----------
			void Blocking::setTriggerMode(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
				this->perform([=] () {
					this->device->setTriggerMode(triggerMode, triggerSignalType);
				}, false);
			}

			//----------
			void Blocking::setGPOMode(const GPOMode & gpoMode) {
				this->perform([=] () {
					this->device->setGPOMode(gpoMode);
				}, false);
			}

			//----------
			void Blocking::idleFunction() {
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
	}
}