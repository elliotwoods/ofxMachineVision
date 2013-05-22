#include "./Specification.h"
#include <sstream>

namespace ofxMachineVision {
	//---------
	Specification::Specification() :
	valid(false),
	sensorWidth(0), sensorHeight(0),
	manufacturer(""), modelName("") {
	}

	//---------
	Specification::Specification(const Specification & other) :
	valid(other.getValid()),
	sensorWidth(other.getSensorWidth()), sensorHeight(other.getSensorHeight()),
	manufacturer(other.getManufacturer()), modelName(other.getModelName()),
	features(other.getFeatures()),
	pixelModes(other.getPixelModes()),
	triggerModes(other.getTriggerModes()),
	triggerSignalTypes(other.getTriggerSignalTypes()),
	gpoModes(other.getGPOModes())
	{
	}

	//---------
	Specification::Specification(int sensorWidth, int sensorHeight, string manufacturer, string modelName) :
	valid(true),
	sensorWidth(sensorWidth), sensorHeight(sensorHeight),
	manufacturer(manufacturer), modelName(modelName) {
	}

	//---------
	bool Specification::supports(const Feature & feature) {
		return this->features.count(feature) > 0;
	}

	//---------
	bool Specification::supports(const PixelMode & pixelMode) {
		return this->pixelModes.count(pixelMode) > 0;
	}

	//---------
	bool Specification::supports(const TriggerMode & triggerMode) {
		return this->triggerModes.count(triggerMode) > 0;
	}
	
	//---------
	bool Specification::supports(const TriggerSignalType & triggerSignalType) {
		return this->triggerSignalTypes.count(triggerSignalType) > 0;
	}

	//---------
	bool Specification::supports(const GPOMode & gpoMode) {
		return this->gpoModes.count(gpoMode) > 0;
	}

	//---------
	string Specification::toString() const {
		stringstream ss;
		ss << "//--" << endl;

		ss << "[Manufacturer]\t\t" << this->getManufacturer() << endl;
		ss << "[Model]\t\t\t" << this->getModelName() << endl;
        
		ss << "[Sensor width]\t\t" << this->getSensorWidth() << endl;
		ss << "[Sensor height]\t\t" << this->getSensorHeight() << endl;

		ss << "[Features]\t\t";
		const FeatureSet & features = this->getFeatures();
		for(FeatureSet::const_iterator it = features.begin(); it != features.end(); it++) {
			if (it != features.begin())
				ss << endl << "\t\t\t";
			ss << ofxMachineVision::toString(*it);
		}
		ss << endl;
        
		ss << "[Pixel modes]\t\t";
		const PixelModeSet & pixelModes = this->getPixelModes();
		for(PixelModeSet::const_iterator it = pixelModes.begin(); it != pixelModes.end(); it++) {
			if (it != pixelModes.begin())
				ss << ", ";
			ss << ofxMachineVision::toString(*it);
		}
		ss << endl;
        
		ss << "[Trigger modes]\t\t";
		const TriggerModeSet & triggerModes = this->getTriggerModes();
		for(TriggerModeSet::const_iterator it = triggerModes.begin(); it != triggerModes.end(); it++) {
			if (it != triggerModes.begin())
				ss << ", ";
			ss << ofxMachineVision::toString(*it);
		}
		ss << endl;
        
		ss << "[Trigger signal types]\t";
		const TriggerSignalTypeSet & triggerSignalTypes = this->getTriggerSignalTypes();
		for(TriggerSignalTypeSet::const_iterator it = triggerSignalTypes.begin(); it != triggerSignalTypes.end(); it++) {
			if (it != triggerSignalTypes.begin())
				ss << ", ";
			ss << ofxMachineVision::toString(*it);
		}
		ss << endl;
        
		ss << "[GPO modes]\t\t";
		const GPOModeSet & gpoModes = this->getGPOModes();
		for(GPOModeSet::const_iterator it = gpoModes.begin(); it != gpoModes.end(); it++) {
			if (it != gpoModes.begin())
				ss << ", ";
			ss << ofxMachineVision::toString(*it);
		}
		ss << endl;

		ss << "//--" << endl;
        
		return ss.str();
	}

	//---------
	void Specification::addFeature(const Feature & feature) {
		this->features.insert(feature);
	}

	//---------
	void Specification::addPixelMode(const PixelMode & pixelMode) {
		this->pixelModes.insert(pixelMode);
	}

	//---------
	void Specification::addTriggerMode(const TriggerMode & triggerMode) {
		this->triggerModes.insert(triggerMode);
	}
	
	//---------
	void Specification::addTriggerSignalType(const TriggerSignalType & triggerSignalType) {
		this->triggerSignalTypes.insert(triggerSignalType);
	}

	//---------
	void Specification::addGPOMode(const GPOMode & gpoMode) {
		this->gpoModes.insert(gpoMode);
	}
}