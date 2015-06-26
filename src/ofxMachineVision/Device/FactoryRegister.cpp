#include "FactoryRegister.h"

#include "./NullDevice.h"
#include "./VideoInput.h"
#include "./OSXUVC.h"

namespace ofxMachineVision {
	namespace Device {
		//----------
		OFXPLUGIN_FACTORY_REGISTER_SINGLETON_SOURCE(FactoryRegister);

		//----------
		FactoryRegister::FactoryRegister() {
			this->add<NullDevice>();

			//register the webcam device whatever it is on this platform
			this->add<Webcam>();
		}
	}
}