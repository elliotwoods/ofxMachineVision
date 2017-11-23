#pragma once
#include "Blocking.h"

namespace ofxMachineVision {
	namespace Device {
		class StillImages : public Blocking {
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
			shared_ptr<Frame> getFrame() override;
		protected:
			shared_ptr<Frame> frame;
			size_t frameIndex = 0;
			chrono::system_clock::time_point openTime;

			shared_ptr<Parameter<float>> exposure;
		};
	}
}
