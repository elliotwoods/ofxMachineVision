/**
 \file      Device.cpp
 \author    Elliot Woods
 */

#include "Device.h"

#ifndef __func__
#define __func__ __FUNCTION__
#endif

#define LOG_ERROR ofLogError("ofxMachineVision::" + string(__func__))
#define CHECK_OPEN if(!this->getIsOpen()) { LOG_ERROR << " Method cannot be called whilst device is not open"; return; }
#define REQUIRES(feature) if(!this->specification.supports(feature)) { LOG_ERROR << " Device requires " << Device::toString(feature) << " to use this function."; return; }

namespace ofxMachineVision {
#pragma mark Specification
    //---------
    Device::Specification::Specification() :
    valid(false),
    sensorWidth(0), sensorHeight(0),
    manufacturer(""), modelName("") {
    }

    //---------
    Device::Specification::Specification(const Specification & other) :
    valid(other.getValid()),
    sensorWidth(other.getSensorWidth()), sensorHeight(other.getSensorHeight()),
    manufacturer(other.getManufacturer()), modelName(other.getModelName()),
    features(other.getFeatures()),
    pixelModes(other.getPixelModes()),
    triggerModes(other.getTriggerModes()),
    triggerSignalTypes(other.getTriggerSignalTypes())
    {
    }

    //---------
    Device::Specification::Specification(int sensorWidth, int sensorHeight, string manufacturer, string modelName) :
    valid(true),
    sensorWidth(sensorWidth), sensorHeight(sensorHeight),
    manufacturer(manufacturer), modelName(modelName) {
    }

	//---------
    bool Device::Specification::supports(const Feature & feature) {
        return this->features.count(feature) > 0;
    }

    //---------
    bool Device::Specification::supports(const PixelMode & pixelMode) {
        return this->pixelModes.count(pixelMode) > 0;
    }

    //---------
    bool Device::Specification::supports(const TriggerMode & triggerMode) {
        return this->triggerModes.count(triggerMode) > 0;
    }

    //---------
    bool Device::Specification::supports(const TriggerSignalType & triggerSignalType) {
        return this->triggerSignalTypes.count(triggerSignalType) > 0;
    }

    //---------
    string Device::Specification::toString() const {
        stringstream ss;
        ss << "//--" << endl;
        ss << __func__ << endl;
        ss << endl;
        ss << "[Manufacturer]\t\t" << this->getManufacturer() << endl;
        ss << "[Model]\t\t\t" << this->getModelName() << endl;
        
        ss << "[Features]\t\t";
        const FeatureSet & features = this->getFeatures();
        for(FeatureSet::const_iterator it = features.begin(); it != features.end(); it++) {
            if (it != features.begin())
                ss << ", ";
            ss << Device::toString(*it);
        }
        ss << endl;
        
        ss << "[Pixel modes]\t\t";
        const PixelModeSet & pixelModes = this->getPixelModes();
        for(PixelModeSet::const_iterator it = pixelModes.begin(); it != pixelModes.end(); it++) {
            if (it != pixelModes.begin())
                ss << ", ";
            ss << Device::toString(*it);
        }
        ss << endl;
        
        ss << "[Trigger modes]\t\t";
        const TriggerModeSet & triggerModes = this->getTriggerModes();
        for(TriggerModeSet::const_iterator it = triggerModes.begin(); it != triggerModes.end(); it++) {
            if (it != triggerModes.begin())
                ss << ", ";
            ss << Device::toString(*it);
        }
        ss << endl;
        
        ss << "[Trigger signal types]:\t";
        const TriggerSignalTypeSet triggerSignalTypes = this->getTriggerSignalTypes();
        for(TriggerSignalTypeSet::const_iterator it = triggerSignalTypes.begin(); it != triggerSignalTypes.end(); it++) {
            if (it != triggerSignalTypes.begin())
                ss << ", ";
            ss << Device::toString(*it);
        }
        ss << endl;
        
        ss << "//--" << endl;
        
        return ss.str();
    }

    //---------
    void Device::Specification::addFeature(const Feature & feature) {
        this->features.insert(feature);
    }

    //---------
    void Device::Specification::addPixelMode(const PixelMode & pixelMode) {
        this->pixelModes.insert(pixelMode);
    }

    //---------
    void Device::Specification::addTriggerMode(const TriggerMode & triggerMode) {
        this->triggerModes.insert(triggerMode);
    }

    //---------
    void Device::Specification::addTriggerSignalType(const TriggerSignalType & triggerSignalType) {
        this->triggerSignalTypes.insert(triggerSignalType);
    }

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
    
#pragma mark toString helpers
    //----------
    string Device::toString(const Feature & deviceFeature) {
        switch (deviceFeature) {
            case Feature_ROI:
                return "ROI";
            case Feature_Binning:
                return "Binning";
            case Feature_PixelClock:
                return "Pixel clock";
            case Feature_Triggering:
                return "Triggering";
            case Feature_FreeRun:
                return "Free run capture";
            case Feature_OneShot:
                return "One shot capture";
            case Feature_Exposure:
                return "Exposure";
            case Feature_Gain:
                return "Gain";
        }
    }
    
    //----------
    string Device::toString(const PixelMode & pixelMode) {
        switch (pixelMode) {
            case Pixel_Unallocated:
                return "Unallocated";
            case Pixel_L8:
                return "L8";
            case Pixel_L12:
                return "L12";
            case Pixel_L16:
                return "L16";
            case Pixel_RGB8:
                return "RGB8";
            case Pixel_BAYER8:
                return "BAYER8";
        }
    }
    
    //----------
    string Device::toString(const TriggerMode & triggerMode) {
        switch (triggerMode) {
            case Trigger_Device:
                return "Device";
            case Trigger_GPIO1:
                return "GPIO1";
            case Trigger_GPIO2:
                return "GPIO2";
            case Trigger_Software:
                return "Software";
        }
    }
    
    //----------
    string Device::toString(const TriggerSignalType & triggerSignalType) {
        switch (triggerSignalType) {
            case TriggerSignal_Default:
                return "Default";
            case TriggerSignal_FallingEdge:
                return "Falling edge";
            case TriggerSignal_RisingEdge:
                return "Rising edge";
            case TriggerSignal_WhilstHigh:
                return "Whilst high";
            case TriggerSignal_WhilstLow:
                return "Whilst low";
        }
    }
    
    //----------
    bool Device::isColor(const ofxMachineVision::Device::PixelMode & pixelMode) {
        switch (pixelMode) {
            case Pixel_Unallocated:
            case Pixel_L8:
            case Pixel_L12:
            case Pixel_L16:
            case Pixel_BAYER8:
                return false;
            case Pixel_RGB8:
                return true;
        }
    }
}