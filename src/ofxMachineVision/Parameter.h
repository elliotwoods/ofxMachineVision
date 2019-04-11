#pragma once

#include "ofParameter.h"

namespace ofxMachineVision {
	class AbstractParameter {
	public:
		AbstractParameter(std::string units) 
		: units(units) {
		}

		virtual ofAbstractParameter & getParameter() = 0;

		template<class TypeName>
		ofParameter<TypeName> * getParameterTyped() {
			return dynamic_cast<ofParameter<TypeName> *>(&this->getParameter());
		}

		const std::string & getUnits() const {
			return this->units;
		}
		
		virtual void syncToDevice() const = 0;
		virtual void syncFromDevice() = 0;
	protected:
		std::string units;
	};

	template<class TypeName>
	class Parameter : public AbstractParameter {
	public:
		typedef std::function<TypeName()> GetDeviceValueFunction;
		typedef std::function<void(const TypeName &)> SetDeviceValueFunction;

		Parameter(ofParameter<TypeName> && parameter, const std::string & units = "")
			: parameter(parameter)
			, AbstractParameter(units) {
		}

		ofAbstractParameter & getParameter() override {
			return this->parameter;
		}

		GetDeviceValueFunction getDeviceValueFunction;
		SetDeviceValueFunction setDeviceValueFunction;

		void syncFromDevice() override {
			if (getDeviceValueFunction) {
				this->parameter.set(getDeviceValueFunction());
			}
		}

		void syncToDevice() const override {
			if (setDeviceValueFunction) {
				setDeviceValueFunction(this->parameter.get());
			}
		}
	protected:
		ofParameter<TypeName> parameter;
	};

	template<>
	class Parameter<float> : public AbstractParameter {
	public:
		typedef std::function<float()> GetDeviceValueFunction;
		typedef std::function<void(float &, float &)> GetDeviceValueRangeFunction;
		typedef std::function<void(const float &)> SetDeviceValueFunction;

		Parameter(ofParameter<float> && parameter, const std::string & units = "")
			: parameter(parameter)
			, AbstractParameter(units) {

		}

		ofAbstractParameter & getParameter() override {
			return this->parameter;
		}

		GetDeviceValueFunction getDeviceValueFunction;
		GetDeviceValueRangeFunction getDeviceValueRangeFunction;
		SetDeviceValueFunction setDeviceValueFunction;

		void syncFromDevice() override {
			if (getDeviceValueRangeFunction) {
				float min, max;
				getDeviceValueRangeFunction(min, max);
				this->parameter.setMin(min);
				this->parameter.setMax(max);
			}
			if (getDeviceValueFunction) {
				this->parameter.set(getDeviceValueFunction());
			}
	}

		void syncToDevice() const override {
			if (setDeviceValueFunction) {
				setDeviceValueFunction(this->parameter.get());
			}
		}
	protected:
		//we don't use parameter events locally because we can't control the threading pattern from here
		ofParameter<float> parameter;
	};
}