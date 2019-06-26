#include "VideoPlayer.h"

#include "ofSystemUtils.h"

namespace ofxMachineVision {
	namespace Device {

		//----------
		string VideoPlayer::getTypeName() const {
			return "VideoPlayer";
		}

		//----------
		vector<Device::Base::ListedDevice> VideoPlayer::listDevices() const {
			auto files = ofDirectory(".").getFiles();

			vector<Device::Base::ListedDevice> results;
			for (auto file : files) {
				auto extension = ofToLower(file.getExtension());
				if (extension == "mov"
					|| extension == "avi"
					|| extension == "mp4") {
					auto initialisationSetttings = dynamic_pointer_cast<VideoPlayer::InitialisationSettings>(VideoPlayer::getDefaultSettings());
					initialisationSetttings->filename.set(file.getFileName());

					results.push_back(ListedDevice{
							initialisationSetttings
						, "VideoPlayer"
						, file.getFileName()
						});
				}
			}

			return results;
		}

		//----------
		ofxMachineVision::Specification VideoPlayer::open(shared_ptr<Base::InitialisationSettings> initialisationSettings) {
			auto typedInitialisationSettings = dynamic_pointer_cast<VideoPlayer::InitialisationSettings>(initialisationSettings);
			if (!typedInitialisationSettings) {
				typedInitialisationSettings = make_shared<VideoPlayer::InitialisationSettings>();
			}

			if (typedInitialisationSettings->filename.get().empty()) {
				auto result = ofSystemLoadDialog("Select video file for ofxMachineVision to play back");
				if (!result.bSuccess) {
					throw(ofxMachineVision::Exception("No file selected"));
				} 
				typedInitialisationSettings->filename = result.filePath;
			}
			if (!ofFile::doesFileExist(filesystem::path(typedInitialisationSettings->filename.get()))) {
				throw(ofxMachineVision::Exception("File does not exist"));
			}

			this->videoPlayer = make_shared<ofVideoPlayer>();
			this->videoPlayer->load(typedInitialisationSettings->filename);
			this->videoPlayer->setSpeed(typedInitialisationSettings->playbackRate);

			ofxMachineVision::Specification specification(CaptureSequenceType::Continuous
				, this->videoPlayer->getWidth()
				, this->videoPlayer->getHeight()
				, "ofxMachineVision"
				, "Video player"
				, "ofVideoPlayer");
			return specification;
		}

		//----------
		void VideoPlayer::close() {
			this->videoPlayer.reset();
		}

		//----------
		bool VideoPlayer::startCapture() {
			this->videoPlayer->play();
			return this->videoPlayer->isPlaying();
		}

		//----------
		void VideoPlayer::stopCapture() {
			this->videoPlayer->stop();
		}

		//----------
		shared_ptr<ofxMachineVision::Frame> VideoPlayer::getFrame() {
			if (this->videoPlayer->isPlaying()) {

				auto frame = FramePool::X().getAvailableFrameFilledWith(this->videoPlayer->getPixels());
				frame->setTimestamp(chrono::nanoseconds((long long) (1e9 * this->videoPlayer->getPosition())));
				frame->setFrameIndex(this->videoPlayer->getCurrentFrame());
				return frame;
			}
			return shared_ptr<Frame>();
		}

		//----------
		void VideoPlayer::updateIsFrameNew() {
			this->videoPlayer->update();
		}

		//----------
		bool VideoPlayer::isFrameNew() {
			return this->videoPlayer->isFrameNew();
		}
	}
}