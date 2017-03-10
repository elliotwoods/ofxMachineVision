#pragma once

#include "ofConstants.h"
#ifdef TARGET_WIN32

#include "../../../libs/videoInput/include/videoInput.h"

#include "ofxMachineVision/Specification.h"
#include "Updating.h"

#include "ofVideoPlayer.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		\brief Implementation of Theo's videoInput library as an ofxMachineVision::Device::Blocking class
		*/
		class VideoPlayer : public Updating {
		public:
			struct InitialisationSettings : Base::InitialisationSettings {
			public:
				InitialisationSettings() {
					this->remove(deviceID);
					this->add(filename);
					this->add(playbackRate);
					this->add(loop);
				}

				ofParameter<string> filename{ "Filename", "" };
				ofParameter<float> playbackRate{ "Playback rate", 1.0f , 0.01f, 10.0f};
				ofParameter<bool> loop{ "Loop", true };
			};

			virtual string getTypeName() const override;
			shared_ptr<Base::InitialisationSettings> getDefaultSettings() const override {
				return make_shared<InitialisationSettings>();
			}
			Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) override;
			void close() override;
			bool startCapture() override;
			void stopCapture() override;
			shared_ptr<Frame> getFrame() override;

			void updateIsFrameNew() override;
			bool isFrameNew() override;

		protected:
			shared_ptr<ofVideoPlayer> videoPlayer; // since we may allocate this Device class to find its name, let's use pointer here
		};
	}
}

#endif