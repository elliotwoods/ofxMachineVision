#pragma once

#include "ofBaseTypes.h"
#include "ofTexture.h"

#include "Base.h"
#include "ofxMachineVision/Frame.h"

namespace ofxMachineVision {
	namespace Grabber {
		/**
		\brief Grabber::Simple is designed to give the same personality as ofVideoGrabber.
		Often you can get better performance by using Blocking or Callback Grabber instead
		*/
		class Simple : public Base {
		public:
			Simple(Device::Base * device);

			void open(int deviceID = 0);
			void close();
			void startCapture(const TriggerMode & = Trigger_Device, const TriggerSignalType & = TriggerSignal_Default);
			void stopCapture();

			void update();
			
			/**
			 @name ofBaseDraws
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
			 @name ofBaseHasTexture
			 */
			//@{
			ofTexture & getTextureReference() { return this->texture; }
			void setUseTexture(bool useTexture);
			//@}
        
			/**
			 @name ofBaseHasPixels
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
			void setBinning(int binningX = 1, int binningY = 1);
			void setROI(const ofRectangle &);
			void setTriggerMode(const TriggerMode &, const TriggerSignalType &);
			//@}

			bool isFrameNew() { return this->currentFrameNew; }

		protected:
			void callbackNewFrame(Frame &);

			ofPixels pixels;
			ofMutex waitingPixelsLock;
			ofPixels waitingPixels;

			ofTexture texture;
			bool useTexture;

			bool newFrameWaiting;
			bool currentFrameNew;
		};
	}
};