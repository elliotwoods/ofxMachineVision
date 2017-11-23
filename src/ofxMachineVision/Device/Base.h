#pragma once

#include "ofxMachineVision/Specification.h"
#include "ofxMachineVision/Frame.h"
#include "ofxMachineVision/Parameter.h"
#include "ofxMachineVision/Utils/FramePool.h"
#include "ofxMachineVision/Constants.h"

#include "ofRectangle.h"
#include "ofParameter.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief Root base class for Device interfaces
		This interface specifies the functions that every Device::Interface should have.

		Some notes on Device::Interface :
		* No state should be stored in the interface
		* Each interface should offer a set of functions from the perspective of the camera API

		*/
		class Base {
		public:
			struct InitialisationSettings : ofParameterGroup {
			public:
				InitialisationSettings() {
					this->add(this->deviceID.set("Device ID", 0));
				}
				InitialisationSettings(int deviceID)
					: InitialisationSettings() {
					this->deviceID = deviceID;
				}

				ofParameter<int> deviceID;
			};

			struct ListedDevice {
				shared_ptr<InitialisationSettings> initialisationSettings;
				string manufacturer;
				string model;
			};

			virtual ~Base() { }
			virtual string getTypeName() const = 0;

			virtual void initOnMainThread() { }

			virtual vector<ListedDevice> listDevices() const {
				return vector<ListedDevice>();
			}

			virtual shared_ptr<Base::InitialisationSettings> getDefaultSettings() const = 0;
			virtual Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) = 0;
			virtual void close() = 0;

			virtual bool startCapture() {
				return false;
			};
			virtual void stopCapture() { }
			virtual void singleShot() { }
			
			const vector<shared_ptr<AbstractParameter>> & getParameters() const;
		protected:
			template<typename SettingsType>
			/**
			\name Convert settings to derived settings type
			*/
			shared_ptr<SettingsType> getTypedSettings(shared_ptr<Base::InitialisationSettings> baseSettings) {
				if (!baseSettings) {
					//no settings
					return make_shared<SettingsType>();
				}
				else {
					auto settings = dynamic_pointer_cast<SettingsType>(baseSettings);
					if (settings) {
						//settings are of correctType
						return settings;
					}
					else {
						//settings are for another type (e.g. base)
						settings = make_shared<SettingsType>();
						*static_pointer_cast<Base::InitialisationSettings>(settings) = *baseSettings;
						return settings;
					}
				}
			}

			vector<shared_ptr<AbstractParameter>> parameters;
		private:
			InitialisationSettings initialisationSettings;
		};
	}

	typedef shared_ptr<Device::Base> DevicePtr;
}