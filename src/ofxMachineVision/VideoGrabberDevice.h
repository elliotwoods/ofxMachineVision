/**
 \file      Device.h
 \author    Elliot Woods
 */

#pragma once

#include "Device.h"

namespace ofxMachineVision {
    /**
     \brief An ofxMachineVision::Device which wraps ofVideoGrabber. This only supports features available in ofVideoGrabber, and therefore lacks native support for most Device features such as shutter, roi, etc.
     */
    class VideoGrabberDevice : Device {
        
    };
}