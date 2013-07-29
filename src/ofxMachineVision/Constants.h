#pragma once

#include <string>
#include <array>

#ifndef __func__
#define __func__ __FUNCTION__
#endif
#define OFXMV_WARNING ofLogWarning(string(__func__))
#define OFXMV_ERROR ofLogError(string(__func__))
#define OFXMV_FATAL ofLogFatalError(string(__func__))

using namespace std;

namespace ofxMachineVision {
	typedef array<int, 2> Binning;
	typedef unsigned long long Microseconds;
    
	/**
	\brief Features which a device may support. These are known after the device is opened
	*/
	enum Feature {
        Feature_ROI,
        Feature_Binning,
        Feature_PixelClock,
        Feature_Triggering,
		Feature_GPO,
        Feature_FreeRun,
        Feature_OneShot,
        Feature_Exposure,
        Feature_Gain,
		Feature_Focus,
		Feature_DeviceID
    };
        
	/**
	\brief Data mode of camera pixels
	*/
    enum PixelMode {
        Pixel_Unallocated,
        Pixel_L8,
        Pixel_L12,
        Pixel_L16,
        Pixel_RGB8,
        Pixel_BAYER8
    };
        
	/**
	\brief Trigger mode of device. Device is the default (i.e. as fast as possible). GPIO1,2 denote external hardware triggers.
	*/
    enum TriggerMode {
        Trigger_Device,
        Trigger_Software,
        Trigger_GPIO0,
        Trigger_GPIO1
    };
        
	/**
	\brief Signal type which is classed as a trigger when working with an external hardware trigger signal.
	*/
    enum TriggerSignalType {
        TriggerSignal_Default,
        TriggerSignal_RisingEdge,
        TriggerSignal_FallingEdge,
        TriggerSignal_WhilstHigh,
        TriggerSignal_WhilstLow
    };

	/**
	\brief GPO mode of device.
	*/
    enum GPOMode {
        GPOMode_On,
        GPOMode_Off,
        GPOMode_HighWhilstExposure,
        GPOMode_HighWhilstFrameActive,
        GPOMode_LowWhilstExposure,
        GPOMode_LowWhilstFrameActive
    };
    
	typedef std::pair<TriggerMode, TriggerSignalType> TriggerSettings;

	/**
	\brief The current state of the capture device
	*/
    enum DeviceState {
        State_Closed,
        State_Waiting,
        State_Running,
		State_Deleting
    };

	/**
	@name Static helpers
	*/
	//@{
	static string toString(const Feature & deviceFeature) {
		switch (deviceFeature) {
			case Feature_ROI:
				return "ROI";
			case Feature_Binning:
				return "Binning";
			case Feature_PixelClock:
				return "Pixel clock";
			case Feature_Triggering:
				return "Triggering";
			case Feature_GPO:
				return "GPO";
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

	static string toString(const PixelMode & pixelMode) {
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

	static string toString(const TriggerMode & triggerMode) {
		switch (triggerMode) {
			case Trigger_Device:
				return "Device";
			case Trigger_GPIO0:
				return "GPIO0";
			case Trigger_GPIO1:
				return "GPIO1";
			case Trigger_Software:
				return "Software";
		}
	}

	static string toString(const TriggerSignalType & triggerSignalType) {
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

	static string toString(const GPOMode & gpoMode) {
		switch (gpoMode) {
		case GPOMode_On:
			return "On";
		case GPOMode_Off:
			return "Off";
		case GPOMode_HighWhilstExposure:
			return "High whilst exposure";
		case GPOMode_HighWhilstFrameActive:
			return "High whilst frame active";
		case GPOMode_LowWhilstExposure:
			return "Low whilst exposure";
		case GPOMode_LowWhilstFrameActive:
			return "Low whilst frame active";
		}
	}

	static string toString(const DeviceState & deviceState) {
		switch (deviceState) {
		case State_Closed:
			return "Closed";
		case State_Deleting:
			return "Deleting";
		case State_Running:
			return "Running";
		case State_Waiting:
			return "Waiting";
		}
	}
    
	static bool isColor(const PixelMode & pixelMode) {
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
	//@}
}