/**
 \file      Device.cpp
 \author    Elliot Woods
 */

#include "Device.h"

#define LOG_ERROR ofLogError("ofxMachineVision::" + string(__func__))
#define CHECK_OPEN if(!this->getIsOpen()) { LOG_ERROR << " Method cannot be called whilst device is not open"; return; }

namespace ofxMachineVision {
#pragma mark Device public members
    //---------
    void Device::open(int deviceID) {
        this->close();
        
        this->deviceFeatures.clear();
        this->devicePixelModes.clear();
        this->deviceTriggerModes.clear();
        this->deviceTriggerSignalTypes.clear();
        
        if (this->customOpen(deviceID)) {
            this->deviceState = State_Waiting;
        } else {
            this->deviceState = State_Closed;
        }
    }

    //---------
    void Device::close() {
        if (this->getIsOpen()) {
            stopTriggeredCapture();
            
            this->customClose();
            this->deviceState = State_Closed;
        }
    }
    
    //---------
    string Device::getDetailedInfo() const {
        stringstream ss;
        ss << "//--" << endl;
        ss << "Device info:\t" << endl;
        ss << "Manufacturer:\t" << this->getManufacturer() << endl;
        ss << "Model:\t" << this->getModelName() << endl;

        ss << "Capabilities: ";
        const DeviceFeatureSet & features = this->getDeviceFeatures();
        for(DeviceFeatureSet::const_iterator it = features.begin(); it != features.end(); it++) {
            if (it != features.begin())
                ss << ", ";
            ss << toString(*it);
        }
        
        ss << "Pixel modes: ";
        const PixelModeSet & pixelModes = this->getPixelModes();
        for(PixelModeSet::const_iterator it = pixelModes.begin(); it != pixelModes.end(); it++) {
            if (it != pixelModes.begin())
                ss << ", ";
            ss << toString(*it);
        }
        
        ss << "Trigger modes: ";
        const TriggerModeSet & triggerModes = this->getDeviceTriggerModes();
        for(TriggerModeSet::const_iterator it = triggerModes.begin(); it != triggerModes.end(); it++) {
            if (it != triggerModes.begin())
                ss << ", ";
            ss << toString(*it);
        }
        
        ss << "Trigger signal types";
        const TriggerSignalTypeSet triggerSignalTypes = this->getDeviceTriggerSignalTypes();
        for(TriggerSignalTypeSet::const_iterator it = triggerSignalTypes.begin(); it != triggerSignalTypes.end(); it++) {
            if (it != triggerSignalTypes.begin())
                ss << ", ";
            ss << toString(*it);
        }
        
        ss << "//--" << endl;
    }
    
    //----------
    void Device::startTriggeredCapture(TriggerMode triggerMode) {
        CHECK_OPEN
        stopTriggeredCapture();
        
        if (this->deviceTriggerModes.count(triggerMode) == 0) {
            LOG_ERROR << "This device does not support trigger mode " << toString(triggerMode) << ". Choosing device clock trigger instead.";
            triggerMode = Trigger_Device;
        }
        
        if (this->customStart(triggerMode)) {
            this->deviceState = State_Running;
        }
    }
    
    //----------
    void Device::stopTriggeredCapture() {
        CHECK_OPEN
        if (this->deviceState == State_Running) {
            this->customStop();
            this->deviceState = State_Waiting;
        }
    }
    
    //----------
    void Device::draw(float x, float y, float w, float h) {
        this->texture.draw(x, y, w, h);
    }

    //----------
    void Device::update() {
        ofLogWarning() << "Update not implemented";
        
        // update texture
    }
    
#pragma mark Device protected members
    //----------
    void Device::allocateTexture() {
        //should check if we're running in main thread
        
    }
    
#pragma mark toString helpers
    //----------
    string Device::toString(const DeviceFeature & deviceFeature) {
        switch (deviceFeature) {
            case Feature_ROI:
                return "ROI";
            case Feature_Binning:
                return "Binning";
            case Feature_PixelClock:
                return "Pixel clock";
            case Feature_Triggering:
                return "Triggering";
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
}