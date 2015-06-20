#pragma once

#include "ofBaseTypes.h"
#include "ofTexture.h"

#include "Base.h"
#include "ofxMachineVision/Frame.h"
#include "ofxCvMin.h"

namespace ofxMachineVision {
	namespace Grabber {
		/**
		\brief Grabber::Simple is designed to give the same personality as ofVideoGrabber.
		You may be able to get improved performance by using Blocking or Callback Grabber instead
		*/
		class Simple : public Base, public ofBaseDraws, public ofBaseHasTexture, public ofBaseHasPixels, public ofBaseUpdates {
		public:
			Simple();
			~Simple();

			void open(int deviceID = 0);
			void close();
			void startCapture(const TriggerMode & = Trigger_Device, const TriggerSignalType & = TriggerSignal_Default);
			void stopCapture();
			void singleShot();

			bool isFrameNew() const { return this->currentFrameNew; }
			float getFps() const { return this->fps; }
			Microseconds getLastTimestamp() const { return this->lastTimestamp; }
			long getLastFrameIndex() const { return this->lastFrameIndex; }
			shared_ptr<Frame> getFreshFrame(float timeoutSeconds = 5.0f);
			shared_ptr<Frame> getFreshFrameAveraged(int numExposures = 1);

			/**
			 \name ofBaseUpdates
			 */
			//@{
			void update() override;
			//@}
			
			/**
			 \name ofBaseDraws
			 */
			//@{
			void draw(float x, float y) override {
				this->draw(x, y, this->getWidth(), this->getHeight());
			}
			void draw(float x, float y, float w, float h) override;
			void draw(const ofRectangle & rect) override {
				draw(rect.x, rect.y, rect.width, rect.height);
			}
			/** \brief Get the width of the most recent captured frame */
			float getWidth() override;
			/** \brief Get the height of the most recent captured frame */
			float getHeight() override;
			//@}

		    /**
			 \name ofBaseHasTexture
			 */
			//@{
			ofTexture & getTextureReference() override { return this->texture; }
			void setUseTexture(bool useTexture) override;
			//@}
        
			/**
			 \name ofBaseHasPixels
			 */
			//@{
			unsigned char * getPixels() override;
			ofPixels & getPixelsRef() override;
			//@}

			/**
			\name Capture properties
			*/
			//@{
			void setExposure(Microseconds exposure) override;
			void setGain(float percent) override;
			void setFocus(float percent) override;
			void setSharpness(float percent) override;
			void setBinning(int binningX = 1, int binningY = 1) override;
			void setROI(const ofRectangle &) override;
			void setTriggerMode(const TriggerMode &, const TriggerSignalType &) override;
			void setGPOMode(const GPOMode &) override;
			//@}

			/**
			\name Frame functions

			Note our Frame policy:
			* frame is a shared_ptr inside Grabber::Simple
			* For each frame capture, a new shared_ptr<Frame> is instantiated (so you can keep old copies of frames elsewhere as you wish)
			* Where no valid frame is avalable, frame == Frame() (e.g. before first frame arrives).
			* getFrame() and setFrame() lock the pointer. You should not use frame = ... directly

			Other Grabbers can have different policies (e.g. you may choose to reuse the same Frame object for mulitple frame captures).
			*/
			//@{
			shared_ptr<Frame> getFrame();
			void setFrame(shared_ptr<Frame>);
			//@}
		protected:
			void callInRightThread(std::function<void()>);
			void notifyNewFrame(shared_ptr<Frame>);

			shared_ptr<Frame> frame;
			ofMutex framePointerLock;

			ofTexture texture;
			bool useTexture;

			bool newFrameWaiting;
			bool currentFrameNew;

			float fps;
			Microseconds lastTimestamp;
			long lastFrameIndex;
		};
	}
};