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
			NullDevice(int width = 1024, int height = 768, float desiredFramerate = 30);
			string getTypeName() const override;
			Specification open(int deviceID) override;
			void close() override;
			bool startCapture() override;
			void stopCapture() override;
			void getFrame(shared_ptr<Frame>) override;
		protected:
			Specification specification;
			int frameInterval;
			int frameIndex;
		};
	}
}