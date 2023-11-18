#include "FolderWatcher.h"
#include "ofSystemUtils.h"
#include "ofImage.h"

namespace ofxMachineVision {
	namespace Device {
		//----------
		string
			FolderWatcher::getTypeName() const
		{
			return "FolderWatcher";
		}

		//----------
		shared_ptr<Device::Base::InitialisationSettings>
			FolderWatcher::getDefaultSettings() const
		{
			//use blank init settings
			return make_shared<InitialisationSettings>();
		}

		//----------
		ofxMachineVision::Specification
			FolderWatcher::open(shared_ptr<Base::InitialisationSettings> settingsUntyped)
		{
			auto settings = dynamic_pointer_cast<FolderWatcher::InitialisationSettings>(settingsUntyped);
			if (!settings) {
				throw(ofxMachineVision::Exception("Invalid initialisation settings"));
			}
			auto dir = ofDirectory(settings->folder);
			int width = 0;
			int height = 0;
			set<string> newFileList;

			// Get width,height of image and initial file list
			{
				for (auto file : dir) {
					newFileList.insert(file.getAbsolutePath());

					if (width == 0) {
						// The first time we get a valid width we take it
						auto image = ofImage(file);
						if (image.isAllocated()) {
							width = image.getWidth();
							height = image.getHeight();
							// We don't break out of the loop because we still want to list all files
						}
					}
				}
			}
			this->previousFiles = newFileList;
			this->lastDirCheck = chrono::system_clock::now();

			if (width == 0) {
				throw(ofxMachineVision::Exception("Ensure the folder contains a sample image at startup"));
			}

			ofxMachineVision::Specification specification(CaptureSequenceType::OneShot
				, width
				, height
				, "openFrameworks"
				, "FolderWatcher"
				, "ofLoadImage");

			this->initialisationSettings = settings;
			return specification;
		}

		//----------
		void
			FolderWatcher::close()
		{
			
		}

		//----------
		void
			FolderWatcher::updateIsFrameNew()
		{
			// Clear the isFrameNew flag
			this->markIsFrameNew = false;

			// Check if we need to look at files
			{
				auto now = chrono::system_clock::now();
				auto checkInterval = chrono::milliseconds((int)(1000.0f * this->initialisationSettings->period_s.get()));
				if (now - this->lastDirCheck < checkInterval) {
					// Don't need to check files - no action
					return;
				}
			}

			// Look for new files
			{
				auto dir = ofDirectory(this->initialisationSettings->folder);
				set<string> newFileList;
				for (auto file : dir) {
					newFileList.insert(file.getAbsolutePath());
				}
				auto updatedFileList = this->previousFiles;
				for (auto filePath : newFileList) {
					bool newFrameLoaded = false;

					if (this->previousFiles.find(filePath) == this->previousFiles.end()) {
						auto tryToLoadFrame = [&]() {
							// Found a file that wasn't there before
							auto image = ofImage(filePath);

							if (!image.isAllocated()) {
								// It isn't an image
								return false;
							}
							if (image.isAllocated()) {
								// This file is an image

								// Allocate the frame
								auto frame = ofxMachineVision::FramePool::X().getAvailableFrameFilledWith(image);
								frame->setFrameIndex(this->frameIndex++);

								// Get the timestamp of the file
								{
									const auto lastWriteTime = filesystem::last_write_time(filePath);
									const auto lastWriteTime_ticks = lastWriteTime.time_since_epoch().count();
									const auto lastWriteTime_timepoint = chrono::system_clock::time_point(chrono::system_clock::time_point::duration(lastWriteTime_ticks));

									// Store the first timepoint as an epoch for timestamps this session
									if (!this->firstTimestampInitialised) {
										this->firstTimestamp = lastWriteTime_timepoint;
									}
									frame->setTimestamp(lastWriteTime_timepoint - this->firstTimestamp);
								}

								this->frame = frame;
								this->markIsFrameNew = true;
								// it is an image
								return true;
							}
						};
						
						// Try to process the file a few times. The file might still be being written
						{
							int tries = 5;
							float sleepBetweenTries = 2.0f;
							for (int i = 0; i < tries; i++) {
								try {
									newFrameLoaded |= tryToLoadFrame();
									// If we get to here then the file has been handled
									updatedFileList.insert(filePath);
									break;
								}
								catch (...) {
									ofSleepMillis(sleepBetweenTries);
								}
							}
						}
					}

					if (newFrameLoaded && this->initialisationSettings->maxOneFilePerFrame.get()) {
						// Don't try and load any more files this frame
						break;
					}
				}
				this->previousFiles = updatedFileList;
			}
		}

		//----------
		bool
			FolderWatcher::isFrameNew()
		{
			return this->markIsFrameNew;
		}

		//----------
		shared_ptr<ofxMachineVision::Frame>
			FolderWatcher::getFrame()
		{
			return this->frame;
		}
	}
}