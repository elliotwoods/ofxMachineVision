#pragma once

#include "Base.h"

#include "ofxPlugin.h"

using namespace std;

namespace ofxMachineVision {
	namespace Device {
		class FactoryRegister : public ofxPlugin::FactoryRegister<Device::Base>, public ofxSingleton::Singleton<FactoryRegister> {
		public:
			FactoryRegister();
		};
	}
}