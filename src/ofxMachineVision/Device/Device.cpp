/**
 \file      Device.cpp
 \author    Elliot Woods
 */

#include "Device.h"

#define CHECK_OPEN if(!this->getIsOpen()) { LOG_ERROR << " Method cannot be called whilst device is not open"; return; }
#define REQUIRES(feature) if(!this->specification.supports(feature)) { LOG_ERROR << " Device requires " << Device::toString(feature) << " to use this function."; return; }

namespace ofxMachineVision {
#pragma mark PollDeviceThread
    //---------
    void Device::PollDeviceThread::start(Device * device, float updateLoopPeriod) {
        stop();
        
        this->device = device;
        this->updateLoopPeriod = updateLoopPeriod;
        this->startThread(true, false);
    }

    //---------
    void Device::PollDeviceThread::stop() {
        if (this->isThreadRunning()) {
            this->stopThread();
        }
    }

    //---------
    void Device::PollDeviceThread::setUpdateLoopPeriod(float updateLoopPeriod) {
        this->updateLoopPeriod = updateLoopPeriod;
    }

    //---------
    void Device::PollDeviceThread::threadedFunction() {
        while (this->isThreadRunning()) {
            this->device->callbackPollFrame();
            this->sleep(this->updateLoopPeriod);
        }
    }
   
#pragma mark RunDeviceThread
    //----------
    void Device::RunDeviceThread::addAction(const Action & action) {
        this->actionQueueLock.lock();
        this->actionQueue.push(action);
        this->actionQueueLock.unlock();
    }
	
	//----------
	void Device::RunDeviceThread::blockUntilActionQueueEmpty() {
		bool queueEmpty = false;
		
		while (!queueEmpty) {
			this->actionQueueLock.lock();
			queueEmpty = this->actionQueue.empty();
			this->actionQueueLock.unlock();
		}
	}

    //----------
    void Device::RunDeviceThread::threadedFunction() {
		//process any requested actions
		actionQueueLock.lock();
		while (!actionQueue.empty()) {
			device->callbackAction(actionQueue.front());
			actionQueue.pop();
		}
		actionQueueLock.unlock();
		
		if (device->getDeviceState() == State_Running) {
			device->callbackPollFrame();
		}
    }
    
#pragma mark Device public members
    //---------
    void Device::open(int deviceID) {
        this->close();
        
        this->isFrameNew = false;
        this->hasNewFrameWaiting = false;
        
		if (this->driverFreeRunMode == FreeRunMode_Blocking) {
			this->specification = Specification(deviceID);
			this->runDeviceThread.open();
		} else {
			this->specification = this->customOpen(deviceID);
		}
        
        
        if (this->specification.getValid()) {
            this->deviceState = State_Waiting;
            this->roi = ofRectangle(0, 0, this->getSensorWidth(), this->getSensorHeight());
            allocatePixels();
        } else {
            this->deviceState = State_Closed;
        }
    }

    //---------
    void Device::close() {
        if (this->getIsOpen()) {
            stopFreeRunCapture();
            
            this->customClose();
            this->deviceState = State_Closed;
            this->pixelMode = Pixel_Unallocated;
        }
    }
    
    //----------
    void Device::startFreeRunCapture(const TriggerMode & triggerMode, const TriggerSignalType & triggerSignalType) {
        CHECK_OPEN
        REQUIRES(Feature_FreeRun);
        stopFreeRunCapture();
        
        REQUIRES(triggerMode);
        REQUIRES(triggerSignalType);
        
        this->isFrameNew = false;
        this->hasNewFrameWaiting = false;
        
        if (this->customStart(triggerMode, triggerSignalType)) {
            
            switch (this->driverFreeRunMode) {
                case FreeRunMode_Blocking:
                    pollThread.start(this);
                    break;
                case FreeRunMode_OwnThread:
                    break;
                case FreeRunMode_PollEveryFrame:
                    pollThread.start(this);
                    break;
            }
            
            this->deviceState = State_Running;
        }
    }
    
    //----------
    void Device::stopFreeRunCapture() {
        CHECK_OPEN
        if (this->deviceState == State_Running) {
            this->customStop();
            this->deviceState = State_Waiting;
        }
    }
    
    //----------
    bool Device::isFreeRunCaptureRunning() const {
        return this->deviceState == State_Running;
    }
    //----------
    void Device::draw(float x, float y, float w, float h) {
        this->texture.draw(x, y, w, h);
    }

    //----------
    void Device::update() {
        if (this->hasNewFrameWaiting) {
            this->isFrameNew = true;
            this->hasNewFrameWaiting = false;
        }
        
        //update texture if frame is new
        if (this->isFrameNew && this->useTexture) {
            if (!ofThread::isMainThread()) {
                LOG_ERROR << "Cannot update camera's texture from a thread other than the main thread. Please call this function from the main application thread.";
                return;
            }

			const ofPixels & pixels = this->frame.getPixelsRef();

            //if we haven't allocated yet or frame dimensions have changed
            if (this->texture.getWidth() != pixels.getWidth() || this->texture.getHeight() != pixels.getHeight()) {
                this->texture.allocate(pixels);
            }
            
            this->texture.loadData(pixels);
        }
    }
    
    //----------
    bool Device::getIsFrameNew() const {
        return this->isFrameNew;
    }
    
    //----------
    void Device::setUseTexture(bool useTexture) {
        this->useTexture = useTexture;
        if (useTexture) {
            this->allocateTexture();
        } else {
            this->texture.clear();
        }
    }
    
#pragma mark Device protected members
	//----------
	Device::Device(const FreeRunMode & freeRunMode) :
		frame(this)
	{
		this->driverFreeRunMode = freeRunMode;
		this->deviceState = State_Closed;
		this->setUseTexture(true);
		this->isFrameNew = false;
		this->hasNewFrameWaiting = false;
		
		if (freeRunMode == FreeRunMode_Blocking) {
			this->runDeviceThread.setDevice(this);
			this->runDeviceThread.startThread();
		}
	}
	
    //----------
    void Device::callbackAction(const RunDeviceThread::Action & action) {
		switch (action) {
			case RunDeviceThread::Action_Open:
				this->specification = this->customOpen(this->getDeviceID());
				break;
			case RunDeviceThread::Action_Close:
				this->customClose();
				break;
			case RunDeviceThread::Action_StartFreeRun:
				break;
			case RunDeviceThread::Action_StopFreeRun:
				break;
			default:
				break;
		}
	}
	
	//----------
    void Device::callbackPollFrame() {
        if (this->customPollFrame()) {
            this->onNewFrame();
        }
    }
    
    //----------
    void Device::onNewFrame() {
        ofNotifyEvent(this->newFrame, this->frame, this);
        this->hasNewFrameWaiting = true;
    }
    
    //----------
    void Device::allocatePixels() {
        
    }
    
    //----------
    void Device::allocateTexture() {
        this->texture.allocate(this->frame.getPixelsRef());
    }
}