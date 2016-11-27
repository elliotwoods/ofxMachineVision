#pragma once

#include "ofxMachineVision/Specification.h"
#include "ofxMachineVision/Frame.h"
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

			virtual ~Base() { }
			virtual string getTypeName() const = 0;

			virtual void initOnMainThread() { }

			virtual shared_ptr<Base::InitialisationSettings> getDefaultSettings() = 0;
			virtual Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) = 0;
			virtual void close() = 0;

			virtual bool startCapture() {
				return false;
			};
			virtual void stopCapture() {

			}
			virtual void singleShot() { }
			
			/**
			\name Optional interfaces
			You should declare which actions your camera supports in the Specification's Features
			*/
			//@{
			virtual void setExposure(Microseconds exposure) { };
			virtual void setGain(float percent) { };
			virtual void setFocus(float percent) { };
			virtual void setSharpness(float percent) { };
			virtual void setBinning(int binningX = 1, int binningY = 1) { };
			virtual void setROI(const ofRectangle &) { };
			virtual void setTriggerMode(const TriggerMode &, const TriggerSignalType &) { };
			virtual void setGPOMode(const GPOMode &) { };
			//@}
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
		private:
			InitialisationSettings initialisationSettings;
		};
	}

	typedef shared_ptr<Device::Base> DevicePtr;
}