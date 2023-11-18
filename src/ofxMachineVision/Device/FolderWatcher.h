#pragma once
#include "Updating.h"

namespace ofxMachineVision {
	namespace Device {
		class FolderWatcher : public Updating {
		public:
			struct InitialisationSettings : Device::Base::InitialisationSettings {
				InitialisationSettings() {
					add(this->folder);
					add(this->period_s);
					add(this->maxOneFilePerFrame);
				}

				ofParameter<filesystem::path> folder{ "Folder", "" };
				ofParameter<float> period_s{ "Period [s]", 1.0f };
				ofParameter<bool> maxOneFilePerFrame{ "Max one file per frame", true };
			};
			string getTypeName() const override;

			shared_ptr<Device::Base::InitialisationSettings> getDefaultSettings() const override;
			Specification open(shared_ptr<Base::InitialisationSettings> = nullptr) override;
			void close() override;

			void updateIsFrameNew() override;
			bool isFrameNew() override;
			shared_ptr<Frame> getFrame() override;
		protected:
			shared_ptr<InitialisationSettings> initialisationSettings;
			size_t frameIndex = 0;
			
			// The timestamp for the frame is taken relative to the EXIF timestamp of the first file
			chrono::system_clock::time_point firstTimestamp;
			bool firstTimestampInitialised = false;

			chrono::system_clock::time_point lastDirCheck;

			set<string> previousFiles;

			// Frame that will be returned by getFrame
			shared_ptr<Frame> frame;
			bool markIsFrameNew = false;
		};
	}
}
