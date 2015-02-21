#pragma once

/**
 \file      ofxMachineVision.h
 \brief     Include this file in your project to have access to the namespace ofxMachineVision.
 \author    Elliot Woods

Implemented:
 - Device::Blocking
 - Device::Updating
 - Grabber::Simple (Acts like Updating + Callback + draws)
 
Awaiting:
 - Device::Callback
 - Grabber::Callback
 - Grabber::Blocking
*/

#include "ofxMachineVision/Constants.h"
#include "ofxMachineVision/Device/Blocking.h"
#include "ofxMachineVision/Device/Updating.h"
#include "ofxMachineVision/Grabber/Simple.h"
#include "ofxMachineVision/Stream/Recorder.h"
#include "ofxMachineVision/Stream/DiskStreamer.h"

#include "ofxMachineVision/Device/VideoInputDevice.h"
#include "ofxMachineVision/Device/OSXUVCDevice.h"
#include "ofxMachineVision/Device/NullDevice.h"

#include "ofConstants.h"

namespace ofxMachineVision {
	/**
	\brief A simple class for wrapping camera drivers
	e.g. usage : ofxMachineVision::SimpleGrabber<ofxXimea::Device> grabber
	this will give you an instance of ofxMachineVision::Grabber::Simple which
	holds an instance of ofxXimea::Device
	*/
	template<class DeviceClass>
	class SimpleGrabber : public Grabber::Simple {
	public:
		SimpleGrabber() : Grabber::Simple(DevicePtr(new DeviceClass())) {
		}

		shared_ptr<DeviceClass> getDeviceTyped() const {
			return static_pointer_cast<DeviceClass>(this->getDevice());
		}
	};
	
	typedef SimpleGrabber<Device::Webcam> Webcam;
}
