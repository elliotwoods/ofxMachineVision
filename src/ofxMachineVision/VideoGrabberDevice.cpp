/**
 \file      Device.h
 \author    Elliot Woods
 */

#include "VideoGrabberDevice.h"

namespace ofxMachineVision {
    //---------
    Device::Specification VideoGrabberDevice::customOpen(int deviceID) {
        this->video.setDeviceID(deviceID);
        this->video.setUseTexture(false);
        
        Specification specification(640, 480, "openFrameworks", "ofVideoGrabber");
        specification.addPixelMode(Pixel_RGB8);
        specification.addFeature(Feature_FreeRun);
        specification.addTriggerMode(Trigger_Device);
        
        return specification;
    }
    
    //---------
    void VideoGrabberDevice::customClose() {
        
    }
    
    //---------
    bool VideoGrabberDevice::customStart(const TriggerMode &, const TriggerSignalType &) {
        //we can't request native sensor size using video grabber
        this->video.initGrabber(this->getSensorWidth(), this->getSensorHeight(), false);
        return this->video.isInitialized();
    }
    
    //---------
    void VideoGrabberDevice::customStop() {
        this->video.close();
    }
    
    //---------
    bool VideoGrabberDevice::customPollFrame() {
        this->video.update();
        bool newFrame = this->video.isFrameNew();
        if (newFrame) {
            this->pixels.setFromExternalPixels(this->video.getPixels(), this->getSensorWidth(), this->getSensorHeight(), 3);
        }
        return newFrame;
    }
}