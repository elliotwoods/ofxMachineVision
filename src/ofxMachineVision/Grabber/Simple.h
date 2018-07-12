#pragma once

#include "ofBaseTypes.h"
#include "ofTexture.h"
#include "ofMath.h"

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
			Simple();
			~Simple();

			shared_ptr<Device::Base::InitialisationSettings> getDefaultInitialisationSettings();
			bool open(shared_ptr<Device::Base::InitialisationSettings> = nullptr) override;
			void close() override;
			void startCapture() override;
			void stopCapture() override;
			void singleShot();

			void reopen(); // also restarts capture if currently alive

			bool isSingleShot() const;
			bool isFrameNew() const { return this->currentFrameNew; }
			float getFps() const { return this->fps; }
			chrono::nanoseconds getLastTimestamp() const { return this->lastTimestamp; }
			long getLastFrameIndex() const { return this->lastFrameIndex; }

			///Get a frame from the grabber and (attempt to) ensure that it was captured after the time of the function call
			/// Notes :
			///		* This function (by default) gives you a copy of the frame, so you don't need to worry about locking
			///		* If you want to ensure that this fresh frame is available from the other functions of this class (e.g. getPixels()), you should call update() after getFreshFrame()
			shared_ptr<Frame> getFreshFrame(chrono::microseconds timeout = chrono::seconds(20));

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
			void draw(float x, float y) const override {
				this->draw(x, y, this->getWidth(), this->getHeight());
			}
			void draw(float x, float y, float w, float h) const override;
			void draw(const ofRectangle & rect) const override {
				draw(rect.x, rect.y, rect.width, rect.height);
			}
			/** \brief Get the width of the most recent captured frame */
			float getWidth() const override;
			/** \brief Get the height of the most recent captured frame */
			float getHeight() const override;
			//@}

		    /**
			 \name ofBaseHasTexture
			 */
			//@{
			ofTexture & getTexture() override;
			const ofTexture & getTexture() const override;
			void setUseTexture(bool useTexture) override;
			bool isUsingTexture() const override;
			//@}
        
			/**
			 \name ofBaseHasPixels
			 \brief Note that these getter functions are cached once per app frame. Use getFrame() for more direct access.
			 */
			//@{
			const ofPixels & getPixels() const override;
			ofPixels & getPixels() override;
			//@}

			/**
			\name Device parameters
			*/
			//@{
			void setExposure(float percent) { this->setParameterByRatio("Exposure", percent); }
			void setGain(float percent) { this->setParameterByRatio("Gain", percent); }
			void setFocus(float percent) { this->setParameterByRatio("Focus", percent); }
			void setSharpness(float percent) { this->setParameterByRatio("Sharpness", percent); }
			void setBinning(const Binning & binning) { this->setParameter("Binning", binning); }
			void setROI(const ofRectangle & roi) { this->setParameter("ROI", roi); };

			const vector<shared_ptr<AbstractParameter>> getDeviceParameters() const;

			template<class TypeName>
			void setParameter(string parameterName, TypeName value) {
				//remove capitalization
				parameterName = ofToLower(parameterName);

				//search for matching parameter
				const auto & parameters = this->getDevice()->getParameters();
				for (auto & parameter : parameters) {
					//check if names match
					if (ofIsStringInString(ofToLower(parameter->getParameter().getName()), parameterName)) {
						//check if it's right type
						auto typedOfParameter = dynamic_cast<ofParameter<TypeName> *>(& parameter->getParameter());
						if (typedOfParameter) {
							//it's the right name and type
							typedOfParameter->set(value);
							this->syncToDevice(* parameter);
							return;
						}
					}
				}
			}

			void setParameterByRatio(string parameterName, float percent) {
				//remove capitalization
				parameterName = ofToLower(parameterName);

				//search for matching parameter
				const auto & parameters = this->getDevice()->getParameters();
				for (auto & parameter : parameters) {
					//check if names match
					if (ofIsStringInString(ofToLower(parameter->getParameter().getName()), parameterName)) {
						//check if it's right type
						auto typedOfParameter = dynamic_cast<ofParameter<float> *>(& parameter->getParameter());
						if (typedOfParameter) {
							//it's the right name and type
							typedOfParameter->set(ofMap(percent, 0, 1, typedOfParameter->getMin(), typedOfParameter->getMax(), true));
							this->syncToDevice(* parameter);
							return;
						}
					}
				}
			}

			void syncToDevice(AbstractParameter &) override;
			void syncFromDevice(AbstractParameter &) override;
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
			shared_ptr<Frame> getFrame() const;
			//@}
		protected:
			void callInRightThread(std::function<void()>, bool blocking);
			void notifyNewFrame(shared_ptr<Frame>);

			void setFrame(shared_ptr<Frame>);
			void clearCachedFrame();

			mutable std::mutex framePointerMutex;
			shared_ptr<Frame> frame;

			ofPixels pixels; // every app frame we cache a set of pixels which we can use for simple access. if getPixels() returned from frame, then there would be locking issues.
			ofTexture texture;
			bool useTexture;

			bool newFrameWaiting;
			bool currentFrameNew;

			float fps;
			chrono::nanoseconds lastTimestamp;
			long lastFrameIndex;

			shared_ptr<Device::Base::InitialisationSettings> lastInitialisationSettingsUsed;
		};
	}
};