#pragma once

#include <set>
#include <string>

#include "Constants.h"
using namespace std;

namespace ofxMachineVision {
	class Specification {
    public:    
        typedef std::set<Feature> FeatureSet;
        typedef std::set<PixelMode> PixelModeSet;
        typedef std::set<TriggerMode> TriggerModeSet;
        typedef std::set<TriggerSignalType> TriggerSignalTypeSet;

        Specification();
		Specification(int deviceID) { this->deviceID = deviceID;}
        Specification(const Specification &);
        Specification(int sensorWidth, int sensorHeight, string manufacturer, string modelName);
            
        bool getValid() const { return this->valid; }
		int getDeviceID() const { return this->deviceID; }
			
        const FeatureSet & getFeatures() const { return this->features; }
        const PixelModeSet & getPixelModes() const { return this->pixelModes; }
        const TriggerModeSet & getTriggerModes() const { return this->triggerModes; }
        const TriggerSignalTypeSet & getTriggerSignalTypes() const { return this->triggerSignalTypes; }
            
        bool supports(const Feature &);
        bool supports(const PixelMode &);
        bool supports(const TriggerMode &);
        bool supports(const TriggerSignalType &);

        int getSensorWidth() const { return this->sensorWidth; }
        int getSensorHeight() const { return this->sensorHeight; }

        const string & getManufacturer() const { return this->manufacturer; }
        const string & getModelName() const { return this->modelName; }

        string toString() const;

		void setDeviceID(int deviceID) { this->deviceID = deviceID; }
        void addFeature(const Feature &);
        void addPixelMode(const PixelMode &);
        void addTriggerMode(const TriggerMode &);
        void addTriggerSignalType(const TriggerSignalType &);

    protected:
        bool valid;
        FeatureSet features;
        PixelModeSet pixelModes;
        TriggerModeSet triggerModes;
        TriggerSignalTypeSet triggerSignalTypes;
            
        int sensorWidth;
        int sensorHeight;
            
        string manufacturer;
        string modelName;
			
		int deviceID;
    };
}
