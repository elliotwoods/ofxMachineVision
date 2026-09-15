#pragma once
#include "ofConstants.h"
#ifdef TARGET_OSX
#include "Callback.h"
#else
#include "Blocking.h"
#endif

namespace ofxMachineVision {
	namespace Device {
		#ifdef TARGET_OSX
		class StillImages : public Callback {
#else
		class StillImages : public Blocking {
#endif
		public:
			struct InitialisationSettings : Device::Base::InitialisationSettings {
				InitialisationSettings() {
					this->clear();
				}
			};
			string getTypeName() const override;

			shared_ptr<Device::Base::InitialisationSettings> getDefaultSettings() const override;
			Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) override;
			void close() override;
			virtual shared_ptr<Frame> getFrame();
#ifdef TARGET_OSX
			void singleShot() override;
#endif
		protected:
			shared_ptr<Frame> frame;
			size_t frameIndex = 0;
			chrono::system_clock::time_point openTime;

			shared_ptr<Parameter<float>> exposure;
		};
	}
}
