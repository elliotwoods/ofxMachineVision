#pragma once

#include "videoInput.h"

#include "ofxMachineVision/Specification.h"
#include "Blocking.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief Implementation of Theo's videoInput library as an ofxMachineVision::Device::Updating class
		*/
		class VideoInputDevice : public Blocking {
		public:
			VideoInputDevice(int width = 640, int height = 480);
			Specification open(int deviceID);
			void close();
			void setExposure(Microseconds exposure);
			void setGain(float percent);
			void setFocus(float percent);
			bool startCapture();
			void stopCapture();
			void getFrame(Frame & frame);
			
			void showSettings(); //must be called directly on this type
		protected:
			videoInput device;
			int deviceID;
			int width, height;

			LARGE_INTEGER timerFrequency;
			LARGE_INTEGER timerStart;
			int frameIndex;
		};
	}
}