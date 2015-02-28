#include "FactoryRegister.h"

#include "./VideoInput.h"
#include "./OSXUVC.h"

namespace ofxMachineVision {
	namespace Device {
		//----------
		FactoryRegister * FactoryRegister::singleton = nullptr;

		//----------
		FactoryRegister & FactoryRegister::X() {
			if (!FactoryRegister::singleton) {
				FactoryRegister::singleton = new FactoryRegister();
			}

			auto & factoryRegister = * FactoryRegister::singleton;

			//register the webcam device whatever it is on this platform
			factoryRegister.registerDevice<Webcam>();

			return factoryRegister;
		}

		//----------
		const map<string, shared_ptr<BaseFactory>> & FactoryRegister::getFactories() const {
			return this->factories;
		}
	}
}