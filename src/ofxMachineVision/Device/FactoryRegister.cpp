#include "FactoryRegister.h"

#include "./NullDevice.h"
#include "./VideoInput.h"
#include "./OSXUVC.h"
#include "./VideoPlayer.h"

OFXSINGLETON_DEFINE(ofxMachineVision::Device::FactoryRegister);

namespace ofxMachineVision {
	namespace Device {
		//----------
		FactoryRegister::FactoryRegister() {
			this->add<NullDevice>();

			//register the webcam device whatever it is on this platform
			this->add<Webcam>();

			this->add<VideoPlayer>();

			this->loadPlugins();
		}
	}
}