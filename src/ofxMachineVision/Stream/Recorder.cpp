#include "Recorder.h"

namespace ofxMachineVision {
	namespace Stream {
		//---------
		Recorder::Recorder() {
			this->state = State_NoGrabber;
			this->grabber = nullptr;
		}

		//---------
		void Recorder::setGrabber(ofxMachineVision::Grabber::Base & grabber) {
			if (this->getState() == State_NoGrabber || this->getState() == State_Ready) {
				this->grabber = & grabber;
			} else {
				OFXMV_ERROR << "Cannot set grabber, recorder currently in use.";
			}
		}

		//---------
		void Recorder::start() {
			if (this->getState() != State_Ready) {
				OFXMV_ERROR << "Cannot start recorder, recorder is not ready to start.";
			}
			ofAddListener(this->grabber->newFrameReceived, this, &Recorder::callbackNewFrame);
		}

		//---------
		void Recorder::stop() {
			if (this->isRecording()) {
				ofRemoveListener(this->grabber->newFrameReceived, this, &Recorder::callbackNewFrame);
			}
		}

		//---------
		bool Recorder::hasGrabber() const {
			return this->grabber != nullptr;
		}

		//---------
		const Recorder::State & Recorder::getState() const {
			return this->state;
		}

		//---------
		bool Recorder::isRecording() const {
			return this->getState() == State_Recording;
		}
		
		//---------
		Frame::Timestamp Recorder::getFirstTimestamp() const {
			if (this->empty()) {
				return 0;
			}
			return this->begin()->getTimestamp();
		}

		//---------
		Frame::Timestamp Recorder::getLastTimestamp() const {
			if (this->empty()) {
				return 1;
			}
			Recorder::const_iterator it = this->end();
			it--;
			return it->getTimestamp();
		}
		
		//---------
		Frame::Timestamp Recorder::getDuration() const {
			return this->getLastTimestamp() - this->getFirstTimestamp();
		}

		//---------
		string Recorder::toString(const State & state) {
			switch(state) {
			case State_NoGrabber:
				return "No grabber";
			case State_Ready:
				return "Ready";
			case State_Recording:
				return "Recording";
			case State_Saving:
				return "Saving";
			case State_Loading:
				return "Loading";
			}
		}

		//---------
		void Recorder::callbackNewFrame(FrameEventArgs & frameEventArgs) {
			Frame & frame = * frameEventArgs.frame;
			this->insert(Frame(frame));
		}
	}
}