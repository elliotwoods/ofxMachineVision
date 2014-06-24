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
			NullDevice(int width = 1920, int height = 1080, float desiredFramerate = 30);
			Specification open(int deviceID) override;
			bool startCapture() override;
			void stopCapture() override;
			void close() override;
			void getFrame(shared_ptr<Frame>) override;
		protected:
			Specification specification;
			int frameInterval;
			int frameIndex;
		};
	}
}