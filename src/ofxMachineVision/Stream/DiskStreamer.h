#pragma once

#include "ofxMachineVision/Frame.h"
#include "ofxMachineVision/Grabber/Base.h"

#include <map>
#include <sstream>
#include "ofSystemUtils.h"

namespace ofxMachineVision {
	namespace Stream {
		class DiskStreamer {
		public:
			enum State {
				NoGrabber,
				Waiting,
				Streaming
			};

			DiskStreamer();
			virtual ~DiskStreamer();

			void setGrabber(ofxMachineVision::GrabberPtr);
			ofxMachineVision::GrabberPtr getGrabber();
			void clearGrabber();

			/** \b Set the folder to stream output to, if folder doesn't exist, we'll try and create it **/
			void setOutputFolder(string path = "");

			/** \b Start recording **/
			void start();
			/** \b Stop recording **/
			void stop();

			bool getHasGrabber() const;
			bool getIsStreaming() const;
			State getState() const;

		protected:
			void callbackFrame(FrameEventArgs &);

			State state;
			ofxMachineVision::GrabberPtr grabber;
			std::filesystem::path outputFolder;
		};
	}
}