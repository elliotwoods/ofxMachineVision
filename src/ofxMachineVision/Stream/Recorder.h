#pragma once

//#include "ofxXimea.h"

#include "ofxMachineVision/Frame.h"
#include "ofxMachineVision/Grabber/Base.h"

#include <set>

namespace ofxMachineVision {
	namespace Stream {
		class Recorder : public std::set<Frame> {
		public:
			enum State {
				State_NoGrabber,
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
			bool isRecording() const;

			Frame::Timestamp getFirstTimestamp() const;
			Frame::Timestamp getLastTimestamp() const;
			Frame::Timestamp getDuration() const;

			static string toString(const State &);

		protected:
			void callbackNewFrame(FrameEventArgs &);

			State state;
			ofxMachineVision::Grabber::Base * grabber;
		};
	}
}