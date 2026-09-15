#pragma once

#include "ofConstants.h"
#ifdef TARGET_OSX
#include "Updating.h"
#include "ofVideoGrabber.h"

namespace ofxMachineVision {
	namespace Device {
		// AVFoundation capture through openFrameworks. Device controls are reported
		// only by camera backends which support them.
		class OSXUVC : public Updating {
		public:
			string getTypeName() const override { return "OSXUVC"; }
			vector<ListedDevice> listDevices() const override;
			shared_ptr<Base::InitialisationSettings> getDefaultSettings() const override;
			Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) override;
			bool startCapture() override;
			void stopCapture() override;
			void close() override;
			void updateIsFrameNew() override;
			bool isFrameNew() override;
			shared_ptr<Frame> getFrame() override;
		protected:
			shared_ptr<ofVideoGrabber> device;
			shared_ptr<Frame> frame;
			chrono::steady_clock::time_point timerStart;
			uint64_t frameIndex = 0;
			bool capturing = false;
		};

		class Webcam : public OSXUVC {
		public:
			string getTypeName() const override { return "Webcam"; }
		};
	}
}
#endif
