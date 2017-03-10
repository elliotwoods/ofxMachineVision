#pragma once

#include "ofxSingleton.h"
#include "ofxMachineVision/Frame.h"

namespace ofxMachineVision {
	//we stay in the main namespace (not Utils)

	class FramePool : public ofxSingleton::UnmanagedSingleton<FramePool> {
	public:
		///Returns an allocated frame filled with the data
		shared_ptr<Frame> getAvailableFrameFilledWith(const ofPixels &);

		///Returns an allocated frame filled with the data
		shared_ptr<Frame> getAvailableFrameFilledWith(const unsigned char *, size_t width, size_t height, ofPixelFormat pixelFormat);

		///Returns an allocated frame
		shared_ptr<Frame> getAvailableAllocatedFrame(int width, int height, ofPixelFormat);

		///Returns a frame with unknown allocation
		shared_ptr<Frame> getAvailableFrame();

		///Returns an unallocated frame with zero timestamp, frame index
		shared_ptr<Frame> getCleanFrame();
	protected:
		mutex cachedFrameStorageMutex;
		vector<pair<unique_ptr<Frame::Storage>, weak_ptr<Frame>>> cache;
	};
}