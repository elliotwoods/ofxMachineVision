#include "Recorder.h"

namespace ofxMachineVision {
	namespace Stream {
		//---------
		Recorder::Recorder() {
			this->state = State::NoGrabber;
			this->grabber = nullptr;
		}

		//---------
		void Recorder::setGrabber(ofxMachineVision::Grabber::Base & grabber) {
			if (this->getState() == State::NoGrabber || this->getState() == State::Ready) {
				this->grabber = & grabber;
				this->state = State::Ready;
			} else {
				OFXMV_ERROR << "Cannot set grabber, recorder currently in use.";
			}
		}

		//---------
		void Recorder::start() {
			this->stop();

			if (this->getState() != State::Ready) {
				OFXMV_ERROR << "Cannot start recorder, recorder is not ready to start.";
				return;
			}
			if (!this->grabber->getIsDeviceOpen()) {
				OFXMV_ERROR << "Cannot start recorder, grabber is not open.";
				this->state = State::GrabberNotReady;
				return;
			}

			this->grabber->onNewFrameReceived.addListener([this] (shared_ptr<Frame> & frame) {
				this->callbackNewFrame(frame);
			}, this);
			this->state = State::Recording;
		}

		//---------
		void Recorder::stop() {
			if (this->getIsRecording()) {
				this->grabber->onNewFrameReceived.removeListeners(this);
				this->state = State::Ready;
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
			return this->getState() == State::Recording;
		}
		
		//---------
		chrono::nanoseconds Recorder::getFirstTimestamp() const {
			if (this->empty()) {
				return chrono::nanoseconds(0);
			}
			return this->begin()->first;
		}

		//---------
		chrono::nanoseconds Recorder::getLastTimestamp() const {
			if (this->empty()) {
				//avoid division by 0
				return chrono::nanoseconds(1);
			}
			Recorder::const_iterator it = this->end();
			it--;
			return it->first;
		}
		
		//---------
		chrono::nanoseconds Recorder::getDuration() const {
			return this->getLastTimestamp() - this->getFirstTimestamp();
		}

		//---------
		string Recorder::toString(const State & state) {
			switch(state) {
			case State::NoGrabber:
				return "No grabber";
			case State::GrabberNotReady:
				return "Grabber not ready";
			case State::Ready:
				return "Ready";
			case State::Recording:
				return "Recording";
			case State::Saving:
				return "Saving";
			case State::Loading:
				return "Loading";
			}
		}

		//---------
		void Recorder::callbackNewFrame(shared_ptr<Frame> & frame) {
			std::map<chrono::nanoseconds, shared_ptr<Frame>>::emplace(frame->getTimestamp(), frame);
		}
	}
}
