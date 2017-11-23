#pragma once

#include "ofxMachineVision/Frame.h"
#include "ofxMachineVision/Grabber/Base.h"

#include <map>

namespace ofxMachineVision {
	namespace Stream {
		class Recorder : public std::map<chrono::nanoseconds, shared_ptr<Frame>> {
		public:
			enum class State {
				NoGrabber,
				GrabberNotReady,
				Ready,
				Recording,
				Saving,
				Loading
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

			chrono::nanoseconds getFirstTimestamp() const;
			chrono::nanoseconds getLastTimestamp() const;
			chrono::nanoseconds getDuration() const;

			static string toString(const State &);

		protected:
			void callbackNewFrame(shared_ptr<Frame> &);

			State state;
			ofxMachineVision::Grabber::Base * grabber;
		};
	}
}