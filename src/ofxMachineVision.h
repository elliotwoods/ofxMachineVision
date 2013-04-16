#pragma once

/**
 \file      ofxMachineVision.h
 \brief     Include this file in your project to have access to the namespace ofxMachineVision.
 \author    Elliot Woods

 Only Device::Blocking and Grabber::Simple have been implemented so far.
 Awaiting:
 - Device::Polling
 - Device::Callback
 - Grabber::Callback
 - Grabber::Blocking
*/

#include "ofxMachineVision/Device/Blocking.h"

#include "ofxMachineVision/Grabber/Simple.h"

namespace ofxMachineVision {
	template<class DeviceClass>
	class SimpleGrabber : public Grabber::Simple {
	public:
		SimpleGrabber() : Grabber::Simple(DevicePtr(new DeviceClass())) {
		}
	};
}