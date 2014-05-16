#pragma once

#include "Base.h"
#include "Blocking.h"

namespace ofxMachineVision {
	namespace Device {
		enum Type {
			Type_Blocking,
			Type_NotImplemented
		};

		/**
		\brief Helper function to find DeviceType from a Device::Interface::Base *
		*/
		static Type getType(shared_ptr<Device::Base> device) {
			if (dynamic_cast<Device::Blocking *>(device.get()) != NULL) {
				return Type_Blocking;
			} else {
				return Type_NotImplemented;
			}
		}

		/**
		\brief Helper function to convert a DeviceType to human readable string
		*/
		static string toString(const Type & deviceType) {
			switch(deviceType) {
			case Type_Blocking:
				return "Blocking device";
			case Type_NotImplemented:
				return "Device not implemented";
			}
		}
	}
}