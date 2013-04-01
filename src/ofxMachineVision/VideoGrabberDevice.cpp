/**
 \file      Device.h
 \author    Elliot Woods
 */

#include "VideoGrabberDevice.h"

namespace ofxMachineVision {
    //---------
    VideoGrabberDevice::VideoGrabberDevice() : Device(Device::FreeRunMode_PollEveryFrame) {
    }
    
    //---------
    bool VideoGrabberDevice::customOpen(int deviceID) {
        this->video.setDeviceID(deviceID);
        this->devicePixelModes.insert(Pixel_RGB8);
        this->deviceTriggerModes.insert(Trigger_Device);
        this->manufacturer = "openFrameworks";
        this->modelName = "ofVideoGrabber";
        return true;
    }
    
    //---------
    void VideoGrabberDevice::customClose() {
        
    }
    
    //---------
    bool VideoGrabberDevice::customStart(TriggerMode triggerMode) {
        //we can't request native sensor size using video grabber
        this->video.initGrabber(640, 480, false);
        return this->video.isInitialized();
    }
    
    //---------
    void VideoGrabberDevice::customStop() {
        this->video.close();
    }
}