#pragma once

#include "Base.h"

#include "Blocking.h"
#include "Updating.h"
#include "Callback.h"

namespace ofxMachineVision {
	namespace Device {
		enum class Type {
			Blocking,
			Updating,
			Callback,
			NotImplemented
		};

		/**
		\brief Helper function to find Device::Type from a Device::Interface::Base *
		*/
		static Type getType(shared_ptr<Device::Base> device) {
			if (dynamic_pointer_cast<Device::Blocking>(device)) {
				return Type::Blocking;
			} else if (dynamic_pointer_cast<Device::Updating>(device)) {
				return Type::Updating;
			} else if (dynamic_pointer_cast<Device::Callback>(device)) {
				return Type::Callback;
			} else {
				return Type::NotImplemented;
			}
		}

		/**
		\brief Helper function to convert a Device::Type to human readable string
		*/
		static string toString(const Type & deviceType) {
			switch(deviceType) {
			case Type::Blocking:
				return "Blocking device";
			case Type::Updating:
				return "Updating device";
			case Type::Callback:
				return "Callback device";
			case Type::NotImplemented:
				return "Device not implemented";
			}
		}
	}
}