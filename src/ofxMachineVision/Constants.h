#pragma once

#include <string>
#include <array>

#ifndef __func__
#define __func__ __FUNCTION__
#endif
#define OFXMV_NOTICE ofLogNotice(string(__func__))
#define OFXMV_WARNING ofLogWarning(string(__func__))
#define OFXMV_ERROR ofLogError(string(__func__))
#define OFXMV_FATAL ofLogFatalError(string(__func__))
#define OFXMV_CATCH_ALL_TO_ERROR catch(const ofxMachineVision::Exception & e) { OFXMV_ERROR << e.what(); } catch(std::exception e) { OFXMV_ERROR << e.what(); }
using namespace std;

namespace ofxMachineVision {
	typedef array<int, 2> Binning;

	/**
	\brief Data mode of camera pixels
	*/
	enum class CaptureSequenceType {
		Continuous,
		OneShot
	};

	/**
	\brief Data mode of camera pixels
	*/
	enum class PixelMode {
        Unallocated,
        L8,
        L12,
        L16,
        RGB8,
        BAYER8
    };
        
	/**
	\brief Trigger mode of device. Device is the default (i.e. as fast as possible). GPIO1,2 denote external hardware triggers.
	*/
	enum class TriggerMode {
		Device,
		Software,
		GPIO1,
		GPIO2
    };
        
	/**
	\brief Signal type which is classed as a trigger when working with an external hardware trigger signal.
	*/
	enum class TriggerSignalType {
        Default,
        RisingEdge,
        FallingEdge,
        WhilstHigh,
        WhilstLow
    };

	/**
	\brief GPO mode of device.
	*/
	enum class GPOMode {
        On,
        Off,
        HighWhilstExposure,
        HighWhilstFrameActive,
        LowWhilstExposure,
        LowWhilstFrameActive
    };
    
	typedef std::pair<TriggerMode, TriggerSignalType> TriggerSettings;

	/**
	\brief The current state of the capture device
	*/
	enum class DeviceState : int {
		//bit pattern = [reserved] [reserved] [running] [open] [exists] 
		ExistsBit = 1 << 0,
		OpenBit = 1 << 1,
		RunningBit = 1 << 2,

		Empty = 0,
		Deleting = 1 << 5,
		Closed = ExistsBit,
        Waiting = OpenBit + ExistsBit,
		Running = RunningBit + OpenBit + ExistsBit
    };
	inline bool operator&(const DeviceState & lhs, const DeviceState & rhs) {
		return (static_cast<int>(lhs) & static_cast<int>(rhs)) != 0;
	}

	/**
	 \brief An exception class
	 */
	class Exception : public std::exception {
	public:
		Exception(std::string & message) : message(message) { }
		Exception(const char * message) : message(string(message)) { }
		virtual const char * what() const override { return this->message.c_str(); }
	protected:
		const string message;
	};
	
	/**
	@name Static helpers
	*/
	//@{
	static string toString(const CaptureSequenceType & captureSequenceType) {
		switch (captureSequenceType) {
		case CaptureSequenceType::Continuous:
			return "Continuous";
		case CaptureSequenceType::OneShot:
			return "OneShot";
		default:
			return "Unsupported";
		}
	}

	static string toString(const PixelMode & pixelMode) {
		switch (pixelMode) {
			case PixelMode::Unallocated:
				return "Unallocated";
			case PixelMode::L8:
				return "L8";
			case PixelMode::L12:
				return "L12";
			case PixelMode::L16:
				return "L16";
			case PixelMode::RGB8:
				return "RGB8";
			case PixelMode::BAYER8:
				return "BAYER8";
			default:
				return "Unsupported";
		}
	}

	static string toString(const TriggerMode & triggerMode) {
		switch (triggerMode) {
			case TriggerMode::Device:
				return "Device";
			case TriggerMode::GPIO1:
				return "GPIO1";
			case TriggerMode::GPIO2:
				return "GPIO2";
			case TriggerMode::Software:
				return "Software";
			default:
				return "Unsupported";
		}
	}

	static string toString(const TriggerSignalType & triggerSignalType) {
		switch (triggerSignalType) {
			case TriggerSignalType::Default:
				return "Default";
			case TriggerSignalType::FallingEdge:
				return "Falling edge";
			case TriggerSignalType::RisingEdge:
				return "Rising edge";
			case TriggerSignalType::WhilstHigh:
				return "Whilst high";
			case TriggerSignalType::WhilstLow:
				return "Whilst low";
			default:
				return "Unsupported";
		}
	}

	static string toString(const GPOMode & gpoMode) {
		switch (gpoMode) {
		case GPOMode::On:
			return "On";
		case GPOMode::Off:
			return "Off";
		case GPOMode::HighWhilstExposure:
			return "High whilst exposure";
		case GPOMode::HighWhilstFrameActive:
			return "High whilst frame active";
		case GPOMode::LowWhilstExposure:
			return "Low whilst exposure";
		case GPOMode::LowWhilstFrameActive:
			return "Low whilst frame active";
		default:
			return "Unsupported";
		}
	}

	static string toString(const DeviceState & deviceState) {
		switch (deviceState) {
		case DeviceState::Closed:
			return "Closed";
		case DeviceState::Deleting:
			return "Deleting";
		case DeviceState::Running:
			return "Running";
		case DeviceState::Waiting:
			return "Waiting";
		default:
			return "Unsupported";
		}
	}
    
	static bool isColor(const PixelMode & pixelMode) {
		switch (pixelMode) {
			case PixelMode::Unallocated:
			case PixelMode::L8:
			case PixelMode::L12:
			case PixelMode::L16:
			case PixelMode::BAYER8:
				return false;
			case PixelMode::RGB8:
				return true;
			default:
				return false;
		}
	}
	//@}
}