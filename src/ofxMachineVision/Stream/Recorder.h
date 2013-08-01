#pragma once

#include "ofxMachineVision/Frame.h"
#include "ofxMachineVision/Grabber/Base.h"

#include <map>

namespace ofxMachineVision {
	namespace Stream {
		class Recorder : public std::map<Microseconds, Frame> {
		public:
			enum State {
				State_NoGrabber,
				State_GrabberNotReady,
				State_Ready,
				State_Recording,
				State_Saving,
				State_Loading
			};

			Recorder();

			void setGrabber(ofxMachineVision::Grabber::Base &);

			/** \b Start recording **/
			void start();
			/** \b Stop recording **/
			void stop();

			bool hasGrabber() const;
			const State & getState() const;
			bool getIsRecording() const;

			Microseconds getFirstTimestamp() const;
			Microseconds getLastTimestamp() const;
			Microseconds getDuration() const;

			static string toString(const State &);

		protected:
			void callbackNewFrame(FrameEventArgs &);

			State state;
			ofxMachineVision::Grabber::Base * grabber;
		};
	}
}