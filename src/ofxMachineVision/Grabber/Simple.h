#pragma once

#include "ofBaseTypes.h"
#include "ofTexture.h"

#include "Base.h"
#include "ofxMachineVision/Frame.h"

namespace ofxMachineVision {
	namespace Grabber {
		/**
		\brief Grabber::Simple is designed to give the same personality as ofVideoGrabber.
		You may be able to get improved performance by using Blocking or Callback Grabber instead
		*/
		class Simple : public Base, public ofBaseDraws, public ofBaseHasTexture, public ofBaseHasPixels, public ofBaseUpdates {
		public:
			Simple(DevicePtr device);

			void open(int deviceID = 0);
			void close();
			void startCapture(const TriggerMode & = Trigger_Device, const TriggerSignalType & = TriggerSignal_Default);
			void stopCapture();

			/**
			 \name ofBaseUpdates
			 */
			//@{
			void update();
			//@}
			
			/**
			 \name ofBaseDraws
			 */
			//@{
			void draw(float x, float y) {
				this->draw(x, y, this->getWidth(), this->getHeight());
			}
			void draw(float x, float y, float w, float h);
			/** \brief Get the width of the most recent captured frame */
			float getWidth();
			/** \brief Get the height of the most recent captured frame */
			float getHeight();
			//@}

		    /**
			 \name ofBaseHasTexture
			 */
			//@{
			ofTexture & getTextureReference() { return this->texture; }
			void setUseTexture(bool useTexture);
			//@}
        
			/**
			 \name ofBaseHasPixels
			 \brief These functions are virtual in case you want to allocate your own ofPixels
			 */
			//@{
			unsigned char * getPixels();
			ofPixels & getPixelsRef();
			//@}

			/**
			\name Capture properties
			*/
			//@{
			void setExposure(Microseconds exposure);
			void setGain(float percent);
			void setFocus(float percent);
			void setBinning(int binningX = 1, int binningY = 1);
			void setROI(const ofRectangle &);
			void setTriggerMode(const TriggerMode &, const TriggerSignalType &);
			void setGPOMode(const GPOMode &);
			//@}

			bool isFrameNew() const { return this->currentFrameNew; }
			
			float getFps() const { return this->fps; }
			Microseconds getLastTimestamp() const { return this->lastTimestamp; }
			long getLastFrameIndex() const { return this->lastFrameIndex; }

		protected:
			void callbackNewFrame(FrameEventArgs &);

			ofPixels pixels;
			ofMutex waitingPixelsLock;
			ofPixels waitingPixels;

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