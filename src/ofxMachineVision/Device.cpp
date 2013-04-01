/**
 \file      Device.cpp
 \author    Elliot Woods
 */

#include "Device.h"

namespace ofxMachineVision {
#pragma mark Device members
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
    
#pragma mark toString helpers
    //--------
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