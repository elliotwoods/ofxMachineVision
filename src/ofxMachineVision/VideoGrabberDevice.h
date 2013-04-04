/**
 \file      Device.h
 \author    Elliot Woods
 */

#pragma once

#include "Device.h"
#include "ofVideoGrabber.h"

namespace ofxMachineVision {
    /**
     \brief An ofxMachineVision::Device which wraps ofVideoGrabber. This only supports features available in ofVideoGrabber, and therefore lacks native support for most Device features such as shutter, roi, etc.
     */
    class VideoGrabberDevice : public Device {
    public:
        VideoGrabberDevice() : Device(Device::FreeRunMode_PollEveryFrame) { }
    protected:
        Specification customOpen(int deviceID);
        void customClose();
        bool customStart(TriggerMode);
        void customStop();
        bool customPollFrame();
        
        ofVideoGrabber video;
    };
}