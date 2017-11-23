#include "Base.h"

namespace ofxMachineVision {
	namespace Device {
		//----------
		const vector<shared_ptr<ofxMachineVision::AbstractParameter>> & Base::getParameters() const {
			return this->parameters;
		}
	}
}