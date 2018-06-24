#include "StillImages.h"
#include "ofSystemUtils.h"
#include "ofImage.h"

namespace ofxMachineVision {
	namespace Device {
		//----------
		string StillImages::getTypeName() const {
			return "StillImages";
		}

		//----------
		shared_ptr<Device::Base::InitialisationSettings> StillImages::getDefaultSettings() const {
			//use blank init settings
			return make_shared<InitialisationSettings>();
		}

		//----------
		ofxMachineVision::Specification StillImages::open(shared_ptr<Base::InitialisationSettings> /*= nullptr*/) {
			this->openTime = chrono::system_clock::now();

			{
				this->exposure = make_shared<Parameter<float>>(ofParameter<float>("Exposure", 1.0f, 0.0f, 16.0f));
				this->parameters.push_back(this->exposure);
			}

			auto frame = this->getFrame();

			ofxMachineVision::Specification specification(CaptureSequenceType::OneShot
			, frame->getPixels().getWidth()
			, frame->getPixels().getHeight()
			, "openFrameworks"
			, "StillImages"
			, "ofLoadImage");
			specification.getCaptureSequenceType();

			return specification;
		}

		//----------
		void StillImages::close() {

		}

		//----------
		shared_ptr<ofxMachineVision::Frame> StillImages::getFrame() {
			auto result = ofSystemLoadDialog("Select image file");
			if (result.bSuccess) {
				auto frame = ofxMachineVision::FramePool::X().getAvailableFrame();

				frame->setFrameIndex(this->frameIndex++);
				auto timeSinceOpen = chrono::system_clock::now() - this->openTime;
				frame->setTimestamp(timeSinceOpen);

				const auto & exposure = this->exposure->getParameter().cast<float>().get();
				if (exposure == 1.0f) {
					//don't apply exposure
					ofLoadImage(frame->getPixels(), result.filePath);
				}
				else {
					//apply an exposure value
					ofShortPixels shortPixels;
					ofLoadImage(shortPixels, result.filePath);
					for (auto & pixel : shortPixels) {
						pixel = (float)pixel * exposure;
					}
					frame->getPixels() = shortPixels;
				}

				return frame;
			}

			//if we're here then return an invalid frame
			return nullptr;
		}
	}
}