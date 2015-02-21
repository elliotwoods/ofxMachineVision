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
				this->state = State_Ready;
			} else {
				OFXMV_ERROR << "Cannot set grabber, recorder currently in use.";
			}
		}

		//---------
		void Recorder::start() {
			this->stop();

			if (this->getState() != State_Ready) {
				OFXMV_ERROR << "Cannot start recorder, recorder is not ready to start.";
				return;
			}
			if (!this->grabber->getIsDeviceOpen()) {
				OFXMV_ERROR << "Cannot start recorder, grabber is not open.";
				this->state = State_GrabberNotReady;
				return;
			}

			this->grabber->onNewFrameReceived.addListener([this] (FrameEventArgs& frame) {
				this->callbackNewFrame(frame);
			}, this);
			this->state = State_Recording;
		}

		//---------
		void Recorder::stop() {
			if (this->getIsRecording()) {
				this->grabber->onNewFrameReceived.removeListeners(this);
				this->state = State_Ready;
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
		bool Recorder::getIsRecording() const {
			return this->getState() == State_Recording;
		}
		
		//---------
		Microseconds Recorder::getFirstTimestamp() const {
			if (this->empty()) {
				return 0;
			}
			return this->begin()->first;
		}

		//---------
		Microseconds Recorder::getLastTimestamp() const {
			if (this->empty()) {
				return 1;
			}
			Recorder::const_iterator it = this->end();
			it--;
			return it->first;
		}
		
		//---------
		Microseconds Recorder::getDuration() const {
			return this->getLastTimestamp() - this->getFirstTimestamp();
		}

		//---------
		string Recorder::toString(const State & state) {
			switch(state) {
			case State_NoGrabber:
				return "No grabber";
			case State_GrabberNotReady:
				return "Grabber not ready";
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
			auto addition = std::pair<Microseconds, Frame>(frame.getTimestamp(), Frame(frame));
			std::map<Microseconds, Frame>::insert(addition);
		}
	}
}
