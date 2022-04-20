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
		
		virtual void syncToDevice() = 0;
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
			: parameter(new ofParameter<TypeName>(parameter))
			, AbstractParameter(units)
			, locallyAllocated(true) {
		}

		Parameter(ofParameter<TypeName> * parameter, const std::string& units = "")
			: parameter(parameter)
			, AbstractParameter(units)
			, locallyAllocated(false) {
		}

		~Parameter() {
			if (this->locallyAllocated) {
				delete this->parameter;
			}
		}

		ofAbstractParameter & getParameter() override {
			return * this->parameter;
		}

		GetDeviceValueFunction getDeviceValueFunction;
		SetDeviceValueFunction setDeviceValueFunction;

		void syncFromDevice() override {
			if (getDeviceValueFunction) {
				this->parameter->set(getDeviceValueFunction());
			}
		}

		void syncToDevice() override {
			if (setDeviceValueFunction) {
				try {
					setDeviceValueFunction(this->parameter->get());
				}
				catch (...) {
					this->syncFromDevice();
				}
			}
		}

		ofParameter<TypeName>* getParameterTypedAuto() {
			return dynamic_cast<ofParameter<TypeName> *>(&this->getParameter());
		}
	protected:
		ofParameter<TypeName> * parameter = nullptr;
		bool locallyAllocated = false;
	};

	template<>
	class Parameter<float> : public AbstractParameter {
	public:
		typedef std::function<float()> GetDeviceValueFunction;
		typedef std::function<void(float &, float &)> GetDeviceValueRangeFunction;
		typedef std::function<void(const float &)> SetDeviceValueFunction;

		Parameter(ofParameter<float> && parameter, const std::string & units = "")
			: parameter(new ofParameter<float>(parameter))
			, AbstractParameter(units)
			, locallyAllocated(true) {
		}

		Parameter(ofParameter<float>* parameter, const std::string& units = "")
			: parameter(parameter)
			, AbstractParameter(units)
			, locallyAllocated(false) {
		}

		~Parameter() {
			if (this->locallyAllocated) {
				delete this->parameter;
			}
		}

		ofAbstractParameter & getParameter() override {
			return * this->parameter;
		}

		ofParameter<float>* getParameterTypedAuto() {
			return dynamic_cast<ofParameter<float> *>(&this->getParameter());
		}

		GetDeviceValueFunction getDeviceValueFunction;
		GetDeviceValueRangeFunction getDeviceValueRangeFunction;
		SetDeviceValueFunction setDeviceValueFunction;

		void syncFromDevice() override {
			if (getDeviceValueRangeFunction) {
				float min, max;
				getDeviceValueRangeFunction(min, max);
				this->parameter->setMin(min);
				this->parameter->setMax(max);
			}
			if (getDeviceValueFunction) {
				this->parameter->set(getDeviceValueFunction());
			}
	}

		void syncToDevice() override {
			if (setDeviceValueFunction) {
				try {
					setDeviceValueFunction(this->parameter->get());
				}
				catch (...) {
					this->syncFromDevice();
				}
			}
		}
	protected:
		//we don't use parameter events locally because we can't control the threading pattern from here
		ofParameter<float> * parameter;
		bool locallyAllocated = false;
	};
}