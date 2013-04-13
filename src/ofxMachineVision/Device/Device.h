/**
 \file      Device.h
 \author    Elliot Woods
 */

#pragma once

#include "ofBaseTypes.h"
#include "ofTexture.h"
#include "ofThread.h"
#include "ofEventUtils.h"

#include <set>
#include <queue>

namespace ofxMachineVision {
    /**
     \brief An abstract interface for machine vision cameras.
     */
    class Device : ofBaseDraws, ofBaseUpdates, ofBaseHasTexture, ofBaseHasPixels {
	public:       
        class PollDeviceThread : public ofThread {
        public:
            ~PollDeviceThread() { stop(); }
            /**
             @param device The device to poll
             @param updateLoopPeriod Period in micro-seconds between update checks
             */
            void start(Device * device, float updateLoopPeriod = 100);
            void stop();
            void setDevice(Device * device) { this->device = device; }
            void setUpdateLoopPeriod(float);
        protected:
            void threadedFunction();
            Device * device;
            float updateLoopPeriod;
        };

		class RunDeviceThread : public ofThread {
		public:
            enum Action {
                Action_Open,
                Action_Close,
                Action_StartFreeRun,
                Action_StopFreeRun
            };
            
			~RunDeviceThread() { }
            
            void open() { this->addAction(Action_Open); this->blockUntilActionQueueEmpty(); }
            void close() { this->addAction(Action_Close); this->blockUntilActionQueueEmpty();  }
            void startFreeRun() { this->addAction(Action_StartFreeRun); this->blockUntilActionQueueEmpty(); }
            void stopFreeRun() { this->addAction(Action_StopFreeRun); this->blockUntilActionQueueEmpty(); }
            
            void addAction(const Action &);
            void setDevice(Device * device) { this->device = device; }
			
			void blockUntilActionQueueEmpty();
		protected:
			void threadedFunction();
            
            queue<Action> actionQueue;
            ofMutex actionQueueLock;
            
            Device * device;
		};
        
        virtual ~Device() { };
        
        /**
         @name ofBaseDraws
         */
        //@{
        void draw(float x, float y) {
            this->draw(x, y, this->getWidth(), this->getHeight());
        }
        void draw(float x, float y, float w, float h);
        /** \brief Get the width of the current capture mode */
        float getWidth() { return (int) this->roi.width; }
        /** \brief Get the height of the current capture mode */
        float getHeight() { return (int) this->roi.height; }
        //@}
        
        /**
         @name ofBaseUpdates
         */
        //@{
        /// \brief Call update every frame to update the texture and isFrameNew flag
        void update();
        //@}
        
        bool getIsFrameNew() const;
        
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
        virtual unsigned char * getPixels() { return this->pixels.getPixels(); }
        virtual ofPixels & getPixelsRef() { return this->pixels; }
        //@}
                
        ofEvent<Frame> newFrame;
        
    protected:
        /**
         \brief Your subclass must declare some parameters for operation when it calls Device's constructor.
         @param freeRunMode use freeRunMode_OwnThread or freeRunMode_NeedsThread
         */
        Device(const FreeRunMode & freeRunMode);
        
        /**
         @name Custom camera functions
         \brief Override these functions in your custom camera class
         */
        //@{
        /** \brief Open the device and return the specification, which includes:
         - `std::set` of supported `DeviceFeature`
         - `std::set` of supported `PixelMode`
         - `std::set` of supported `TriggerMode`s
         - `std::set` of supported `TriggerSignalType`
         - `manufacturer`
         - `modelName`
         - `sensorWidth`
         - `sensorHeight`
         */
        virtual Specification customOpen(int deviceID) = 0;
        virtual void customClose() = 0;
        /** \brief Start the free run capture.
         This function should allocate Device::pixels
         */
        virtual bool customStart(const TriggerMode &, const TriggerSignalType &) = 0;
        virtual void customStop() = 0;
        /** \brief Poll for new frames.
         This function should only be overried for devices with
         `FreeRunMode_PollEveryFrame` driver types
         \return `true` if new frame found.
         */
        virtual bool customPollFrame() { return false; }
        virtual void customSetROI(const ofRectangle &) { }
        virtual void customOneShotCapture(ofPixels &) { };
        virtual void customSetExposure(int ms) { };
        //@}
        
        /**
         @name Threaded callbacks
         */
        //@{
		void callbackAction(const RunDeviceThread::Action &);
		void callbackPollFrame();
        //@}
        
        /**
         @name Events
         */
        //@{
        void onNewFrame();
        //@}
        
        /** \cond PRIVATE */
        FreeRunMode driverFreeRunMode;
        Specification specification;
        DeviceState deviceState;
        
        float fps;
        ofRectangle roi;
        PixelMode pixelMode;
        
        void allocatePixels();
        Frame frame;
        void allocateTexture();
        bool useTexture;
        ofTexture texture;
        
        PollDeviceThread pollThread;
        RunDeviceThread runDeviceThread;
		
        bool isFrameNew;
        bool hasNewFrameWaiting;
        /** \endcond */
        
        friend PollDeviceThread;
        friend RunDeviceThread;
    };
}