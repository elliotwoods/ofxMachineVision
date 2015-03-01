#pragma once

#include "ofConstants.h"
#ifdef TARGET_WIN32

#include "../../../libs/videoInput/include/videoInput.h"

#include "ofxMachineVision/Specification.h"
#include "Blocking.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief Implementation of Theo's videoInput library as an ofxMachineVision::Device::Blocking class
		*/
		class VideoInput : public Blocking {
		public:
			VideoInput(int width = 1920, int height = 1080, float desiredFramerate = 30);
			virtual string getTypeName() const override;
			Specification open(int deviceID) override;
			void close() override;
			bool startCapture() override;
			void stopCapture() override;
			void getFrame(shared_ptr<Frame>) override;
			
			void setExposure(Microseconds exposure) override;
			void setGain(float percent) override;
			void setFocus(float percent) override;
			void setSharpness(float percent) override;
			
			//--
			void showSettings();
			void resetTimestamp();
		protected:
			::videoInput device;
			int deviceID;
			int width, height;
			float desiredFramerate;

			LARGE_INTEGER timerFrequency;
			LARGE_INTEGER timerStart;
			int frameIndex;
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