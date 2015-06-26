#pragma once

#include "Base.h"

#include "ofxPlugin.h"

using namespace std;

namespace ofxMachineVision {
	namespace Device {
		class FactoryRegister : public ofxPlugin::FactoryRegister<Device::Base> {
			OFXPLUGIN_FACTORY_REGISTER_SINGLETON_HEADER(FactoryRegister)
		public:
			FactoryRegister();
		};
	}
}