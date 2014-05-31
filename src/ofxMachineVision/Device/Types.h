#pragma once

#include "Base.h"
#include "Blocking.h"

namespace ofxMachineVision {
	namespace Device {
		enum Type {
			Type_Blocking,
			Type_Updating,
			Type_NotImplemented
		};

		/**
		\brief Helper function to find DeviceType from a Device::Interface::Base *
		*/
		static Type getType(shared_ptr<Device::Base> device) {
			if (dynamic_pointer_cast<Device::Blocking>(device)) {
				return Type_Blocking;
			} else if (dynamic_pointer_cast<Device::Updating>(device)) {
				return Type_Updating;
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
			case Type_Updating:
				return "Updating device";
			case Type_NotImplemented:
				return "Device not implemented";
			}
		}
	}
}