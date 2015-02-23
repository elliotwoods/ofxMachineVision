#pragma once

#include "Base.h"

#include <memory>
#include <map>
#include <string>

using namespace std;

namespace ofxMachineVision {
	namespace Device {
		class BaseFactory {
		public:
			virtual shared_ptr<Device::Base> make() = 0;
			virtual string getDeviceName() const {
				return this->deviceTypeName;
			}
		protected:
			string deviceTypeName;
		};

		template<typename DeviceType>
		class Factory : public BaseFactory {
		public:
			Factory() {
				this->deviceTypeName = this->makeTyped()->getTypeName();
			}

			shared_ptr<Device::Base> make() override {
				return this->makeTyped();
			}

			shared_ptr<DeviceType> makeTyped() {
				return make_shared<DeviceType>();
			}
		};

		class FactoryRegister {
			static FactoryRegister * singleton;
		public:
			static FactoryRegister & X();

			template<typename DeviceType>
			void registerDevice() {
				auto factory = make_shared<Factory<DeviceType>>();
				this->registerFactory(factory);
			}

			void registerFactory(shared_ptr<BaseFactory> factory) {
				const auto deviceName = factory->getDeviceName();
				this->factories.insert(pair<string, shared_ptr<BaseFactory>>(deviceName, factory));
				
			}
		protected:
			map<string, shared_ptr<BaseFactory>> factories;
		};
	}
}