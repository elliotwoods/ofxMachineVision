#pragma once

#include "ofParameter.h"

namespace ofxMachineVision {
	class AbstractParameter {
	public:
		virtual ofAbstractParameter & getParameter() = 0;
		virtual void syncFromDevice() = 0;
		virtual void syncToDevice() = 0;
	};

	template<class TypeName>
	class Parameter {
	public:
		typedef function<TypeName()> GetDeviceValueFunction;
		typedef function<void(const TypeName &)> SetDeviceValueFunction;

		Parameter(ofParameter<TypeName> && parameter)
			: parameter(parameter) {
		}

		ofAbstractParameter & getParameter() override {
			return this->parameter;
		}

		GetDeviceValueFunction getDeviceValue;
		SetDeviceValueFunction setDeviceValue;

		void syncFromDevice() override {
			if (getDeviceValue) {
				this->parameter.set(getDeviceValue());
			}
		}

		void syncToDevice() override {
			if (setDeviceValue) {
				setDeviceValue(this->parameter.get);
			}
		}
	protected:
		ofParameter<TypeName> parameter;
	};

	template<>
	class Parameter<float> : public AbstractParameter {
	public:
		typedef function<float()> GetDeviceValueFunction;
		typedef function<void(float &, float &)> GetDeviceValueRangeFunction;
		typedef function<void(const float &)> SetDeviceValueFunction;

		Parameter(ofParameter<float> && parameter)
			: parameter(parameter) {
			this->parameter.addListener(this, &Parameter::parameterCallback);
		}

		ofAbstractParameter & getParameter() override {
			return this->parameter;
		}

		GetDeviceValueFunction getDeviceValue;
		GetDeviceValueRangeFunction getDeviceValueRange;
		SetDeviceValueFunction setDeviceValue;

		void syncFromDevice() override {
			this->parameter.disableEvents();
			{
				if (getDeviceValueRange) {
					float min, max;
					getDeviceValueRange(min, max);
					this->parameter.setMin(min);
					this->parameter.setMax(max);
				}
				if (getDeviceValue) {
					this->parameter.set(getDeviceValue());
				}
			}
			this->parameter.disableEvents();
		}

		void syncToDevice() override {
			if (setDeviceValue) {
				setDeviceValue(this->parameter.get());
			}
		}
	protected:
		void parameterCallback(float & value) {
			syncToDevice();
		}
		ofParameter<float> parameter;
	};
}