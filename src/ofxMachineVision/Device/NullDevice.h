#pragma once

#include "ofConstants.h"

#include "ofxMachineVision/Specification.h"
#include "Blocking.h"

namespace ofxMachineVision {
	namespace Device {
		/**
		 \brief A null device which feeds black frames at desired framerate
		 */
		class NullDevice : public Blocking {
		public:
			struct InitialisationSettings : Base::InitialisationSettings {
			public:
				InitialisationSettings() {
					add(width.set("Width", 1024));
					add(height.set("Height", 768));
					add(frameRate.set("Frame rate", 30.0f));
				}

				ofParameter<int> width;
				ofParameter<int> height;
				ofParameter<float> frameRate;
			};

			string getTypeName() const override;
			shared_ptr<Base::InitialisationSettings> getDefaultSettings() override {
				return make_shared<InitialisationSettings>();
			}
			Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) override;
			void close() override;
			bool startCapture() override;
			void stopCapture() override;
			void getFrame(shared_ptr<Frame>) override;
		protected:
			InitialisationSettings settings;
			int frameIndex = 0;
		};
	}
}