#include "DiskStreamer.h"

namespace ofxMachineVision {
	namespace Stream {
		//---------
		DiskStreamer::DiskStreamer() {
			this->state = NoGrabber;
			this->outputFolder = Poco::Path(ofToDataPath(""));
		}

		//---------
		DiskStreamer::~DiskStreamer() {
			this->clearGrabber();
		}

		//---------
		void DiskStreamer::setGrabber(ofxMachineVision::GrabberPtr grabber) {
			if (this->state == Streaming) {
				OFXMV_ERROR << "We can't change the grabber whilst we're in Streaming state";
				return;
			}

			this->clearGrabber();
			this->grabber = grabber;
			if (grabber != ofxMachineVision::GrabberPtr()) {
				this->state = Waiting;
				this->grabber->onNewFrameReceived.addListener([this] (FrameEventArgs& args) {
					this->callbackFrame(args);
				}, this);
			} else {
				this->state = NoGrabber;
			}
		}

		//---------
		void DiskStreamer::clearGrabber() {
			if (this->getHasGrabber()) {
				this->grabber->onNewFrameReceived.removeListeners(this);
				this->grabber = GrabberPtr();
			}
		}

		//---------
		void DiskStreamer::setOutputFolder(string pathString) {
			if (pathString == "") {
				pathString = ofSystemLoadDialog("Select output folder for streaming", true).getPath();
			} else {
				pathString = ofToDataPath(pathString, true);
			}
			
			auto path = Poco::Path(pathString);
			if (path.isFile()) {
				//we're missing trailing slash
				path = Poco::Path(pathString + "/");
			}
		
			Poco::File dummyFile(path);
			dummyFile.createDirectories();

			this->outputFolder = path;
		}

		//---------
		void DiskStreamer::start() {
			if (this->state == NoGrabber) {
				OFXMV_ERROR << "Cannot start DiskStreamer, no grabber attached";
				return;
			}
			this->state = Streaming;
		}

		//---------
		void DiskStreamer::stop() {
			if (this->state == NoGrabber) {
				OFXMV_ERROR << "Cannot stop DiskStreamer, no grabber attached";
				return;
			}
			this->state = Waiting;
		}

		//---------
		bool DiskStreamer::getHasGrabber() const {
			return this->grabber != GrabberPtr();
		}

		//---------
		bool DiskStreamer::getIsStreaming() const {
			return this->state == Streaming;
		}

		//---------
		DiskStreamer::State DiskStreamer::getState() const {
			return this->state;
		}

		//---------
		void DiskStreamer::callbackFrame(FrameEventArgs & args) {
			if (this->state == Streaming) {
				stringstream filename;
				filename << outputFolder.toString() << args.frame->getTimestamp() << ".raw";
				int size = args.frame->getPixelsRef().size();
				ofstream file(filename.str(), ios::out | ios::binary);
				file.write((char*) args.frame->getPixels(), size);
				file.close();
			}
		}
	}
}