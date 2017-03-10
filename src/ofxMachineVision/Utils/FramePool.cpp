#include "FramePool.h"

OFXSINGLETON_DEFINE_UNMANAGED(ofxMachineVision::FramePool);

namespace ofxMachineVision {
	//----------
	shared_ptr<ofxMachineVision::Frame> FramePool::getAvailableFrameFilledWith(const ofPixels & pixels) {
		auto frame = this->getAvailableAllocatedFrame(pixels.getWidth(), pixels.getHeight(), pixels.getPixelFormat());
		memcpy(frame->getPixels().getData(), pixels.getData(), pixels.getTotalBytes());
		return frame;
	}

	//----------
	shared_ptr<ofxMachineVision::Frame> FramePool::getAvailableFrameFilledWith(const unsigned char * data, size_t width, size_t height, ofPixelFormat pixelFormat) {
		auto frame = this->getAvailableAllocatedFrame(width, height, pixelFormat);
		memcpy(frame->getPixels().getData(), data, frame->getPixels().getTotalBytes());
		return frame;
	}

	//----------
	shared_ptr<ofxMachineVision::Frame> FramePool::getAvailableAllocatedFrame(int width, int height, ofPixelFormat pixelFormat) {
		{
			unique_lock<mutex> lock(this->cachedFrameStorageMutex);

			//Check through existing pool for available storage
			for (auto & it : this->cache) {
				//check if Frame::Storage is not currently in use (any Frame associated with it has expired)
				if (it.second.expired()) {
					//check if it matches our specification
					auto & pixels = it.first->pixels;
					if (pixels.getWidth() == width
						&& pixels.getHeight() == height
						&& pixels.getPixelFormat() == pixelFormat) {

						//yes it matches! let's build a Frame off this Storage
						auto frame = make_shared<Frame>(*it.first);
						it.second = frame;

						return frame;
					}
				}
			}
		}

		//if we get to here, no frames in the pool are suitable by type, so we loosen our specification
		auto frame = this->getAvailableFrame();

		//but also we promised to deliver a correctly allocated frame, so let's do that
		frame->storage.pixels.allocate(width, height, pixelFormat);

		return frame;
	}

	//----------
	shared_ptr<ofxMachineVision::Frame> FramePool::getAvailableFrame() {
		{
			unique_lock<mutex> lock(this->cachedFrameStorageMutex);

			for (auto & it : this->cache) {
				//check if Frame::Storage is not currently in use (any Frame associated with it has expired)
				if (it.second.expired()) {
					//ok, let's use it
					auto frame = make_shared<Frame>(*it.first);
					it.second = frame;
					return frame;
				}
			}
		}

		//if we get to here, no frames in the pool are available, so we need to make one
		return this->getCleanFrame();
	}

	//----------
	shared_ptr<ofxMachineVision::Frame> FramePool::getCleanFrame() {
		auto storage = make_unique<Frame::Storage>();
		auto frame = make_shared<Frame>(*storage);
		{
			unique_lock<mutex> lock(this->cachedFrameStorageMutex);
			this->cache.emplace_back(move(storage), frame);
		}
		return frame;
	}
}
