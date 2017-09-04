#pragma once

#include "ofConstants.h"
#ifdef TARGET_WIN32

#include "../../../libs/videoInput/include/videoInput.h"

#include "ofxMachineVision/Specification.h"
#include "Blocking.h"
#include <chrono>

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief Implementation of Theo's videoInput library as an ofxMachineVision::Device::Blocking class
		*/
		class VideoInput : public Blocking {
		public:
			struct InitialisationSettings : Base::InitialisationSettings {
			public:
				InitialisationSettings() {
					add(width.set("Width", 1920));
					add(height.set("Height", 1080));
					add(idealFrameRate.set("Ideal frame rate", 30));
				}

				ofParameter<int> width;
				ofParameter<int> height;
				ofParameter<int> idealFrameRate;
			};

			virtual string getTypeName() const override;

			vector<ListedDevice> listDevices() const override;

			shared_ptr<Base::InitialisationSettings> getDefaultSettings() const override {
				return make_shared<InitialisationSettings>();
			}
			Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) override;
			void close() override;
			bool startCapture() override;
			void stopCapture() override;
			shared_ptr<Frame> getFrame() override;
			
			//--
			void showSettings();
			void resetTimestamp();
		protected:
			void setupFloatParameter(string name, long propertyCode);
			shared_ptr<::videoInput> device; // since we may allocate this Device class to find its name, let's use pointer here

			chrono::high_resolution_clock::time_point timerStart;
			int frameIndex;
			int deviceIndex;
		};
		
		class Webcam : public VideoInput {
		public:
			string getTypeName() const override {
				return "Webcam";
			}
		};
	}
}

#endif