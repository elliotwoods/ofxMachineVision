#pragma once

#include "ofBaseTypes.h"

#include "Base.h"

namespace ofxMachineVision {
	namespace Grabber {
		/**
		\brief Grabber::Simple is designed to give the same personality as ofVideoGrabber.
		Often you can get better performance by using Blocking or Callback Grabber instead
		*/
		class Simple : public Base {
		public:
			Simple(Device::Base * device) : Base(device) { }

			void open(int deviceID = 0);
			void close();
			void startCapture(const TriggerMode & = Trigger_Device, const TriggerSignalType & = TriggerSignal_Default);
			void stopCapture();

			void update() { };
			void draw(int x, int y) { };

			/**
			\name Capture properties
			*/
			//@{
			void setBinning(int binningX = 1, int binningY = 1);
			void setROI(const ofRectangle &);
			void setTriggerMode(const TriggerMode &, const TriggerSignalType &);
			//@}
		};
	}
};