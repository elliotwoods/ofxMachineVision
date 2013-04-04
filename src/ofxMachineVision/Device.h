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

namespace ofxMachineVision {
    /**
     \brief An abstract interface for machine vision cameras.
     */
    class Device : ofBaseDraws, ofBaseUpdates, ofBaseHasTexture, ofBaseHasPixels {
    public:

        enum FreeRunMode {
            FreeRunMode_OwnThread,
            FreeRunMode_NeedsThread,
            FreeRunMode_PollEveryFrame
        };
        
        enum Feature {
            Feature_ROI,
            Feature_Binning,
            Feature_PixelClock,
            Feature_Triggering,
            Feature_FreeRun,
            Feature_OneShot,
            Feature_Exposure,
            Feature_Gain
        };
        
        enum PixelMode {
            Pixel_Unallocated,
            Pixel_L8,
            Pixel_L12,
            Pixel_L16,
            Pixel_RGB8,
            Pixel_BAYER8
        };
        
        enum TriggerMode {
            Trigger_Device,
            Trigger_Software,
            Trigger_GPIO1,
            Trigger_GPIO2
        };
        
        enum TriggerSignalType {
            TriggerSignal_Default,
            TriggerSignal_RisingEdge,
            TriggerSignal_FallingEdge,
            TriggerSignal_WhilstHigh,
            TriggerSignal_WhilstLow
        };
        
        enum DeviceState {
            State_Closed,
            State_Waiting,
            State_Running
        };
        
        class Specification {
        public:    
            typedef std::set<Feature> FeatureSet;
            typedef std::set<PixelMode> PixelModeSet;
            typedef std::set<TriggerMode> TriggerModeSet;
            typedef std::set<TriggerSignalType> TriggerSignalTypeSet;

            Specification();
            Specification(const Specification &);
            Specification(int sensorWidth, int sensorHeight, string manufacturer, string modelName);
            
            bool getValid() const { return this->valid; }
            const FeatureSet & getFeatures() const { return this->features; }
            const PixelModeSet & getPixelModes() const { return this->pixelModes; }
            const TriggerModeSet & getTriggerModes() const { return this->triggerModes; }
            const TriggerSignalTypeSet & getTriggerSignalTypes() const { return this->triggerSignalTypes; }
            
            bool supports(const Feature &);
            bool supports(const PixelMode &);
            bool supports(const TriggerMode &);
            bool supports(const TriggerSignalType &);
            
            int getSensorWidth() const { return this->sensorWidth; }
            int getSensorHeight() const { return this->sensorHeight; }
            
            const string & getManufacturer() const { return this->manufacturer; }
            const string & getModelName() const { return this->modelName; }
            
            string toString() const;

            void addFeature(const Feature &);
            void addPixelMode(const PixelMode &);
            void addTriggerMode(const TriggerMode &);
            void addTriggerSignalType(const TriggerSignalType &);
            
        protected:
            bool valid;
            FeatureSet features;
            PixelModeSet pixelModes;
            TriggerModeSet triggerModes;
            TriggerSignalTypeSet triggerSignalTypes;
            
            int sensorWidth;
            int sensorHeight;
            
            string manufacturer;
            string modelName;
        };
        
        class PollDeviceThread : public ofThread {
        public:
            PollDeviceThread() { this->running = false; };
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
            bool running;
            
            friend Device;
        };
        
        virtual ~Device() { };
        
        /**
         @name Setup
         */
        //@{
        void open(int deviceID = 0);
        void close();
        bool getIsOpen() const { return this->deviceState != State_Closed; }
        //@}

        /**
         @name Product specification
         Some of the members of Device::Specification are exposed.
         */
        //@{
        const Specification & getSpecification() const { return this->specification; }
        int getSensorWidth() const { return this->getSpecification().getSensorWidth(); }
        int getSensorHeight() const { return this->getSpecification().getSensorHeight(); }
        const string & getManufacturer() const { return this->getSpecification().getManufacturer(); };
        const string & getModelName() const { return this->getSpecification().getModelName(); };
        //@}
        
        
        /**
         @name Capture resolution settings
         */
        //@{
        void setBinning(int binningX = 1, int binningY = 1);
        //@}
        
        /**
         @name Capture
         */
        //@{
        void startFreeRunCapture(TriggerMode triggerMode = Trigger_Device);
        void stopFreeRunCapture();
        bool isFreeRunCaptureRunning() const;
        void oneShotCapture(ofPixels &, TriggerMode triggerMode = Trigger_Device) const;
        /** \brief Abbreviated form of `startFreeRunCapture(Trigger_Device);` */
        void start() { this->startFreeRunCapture(); }
        /** \brief Abbreviated form of `stopFreeRunCapture();` */
        void stop() { this->stopFreeRunCapture(); }
        //@}
        
        /**
         @name Triggered capture
         Triggered capture (sometimes referred to as 'Free Run Mode') is a mode where a frame is
         captured from the device every time a trigger is given. The default trigger is the
         "Device Trigger" which fires every (1 / fps). Other triggers (e.g. GPIO triggers) cause
         the device only to trigger on a specific event (e.g. electrical signal on a GPIO).
         */
        //@{
        const TriggerMode & getTriggerMode() const;
        void setTriggerMode(const TriggerMode &);
        //@}
        
        /**
         @name Region of Interest
         */
        //@{
        const ofRectangle &  getROI() const;
        void setROI(const ofRectangle &);
        void clearROI();
        //@}
        
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
        
        /**
         @name Static helpers
         */
        //@{
        static string toString(const Feature &);
        static string toString(const PixelMode &);
        static string toString(const TriggerMode &);
        static string toString(const TriggerSignalType &);
        static bool isColor(const PixelMode &);
        //@}
        
        ofEvent<ofPixels> newFrame;
        
    protected:
        /**
         \brief Your subclass must declare some parameters for operation when it calls Device's constructor.
         @param freeRunMode use freeRunMode_OwnThread or freeRunMode_NeedsThread
         */
        Device(FreeRunMode freeRunMode){
            this->driverFreeRunMode = freeRunMode;
            this->deviceState = State_Closed;
            this->setUseTexture(true);
            this->isFrameNew = false;
            this->hasNewFrameWaiting = false;
        }
        
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
        virtual bool customStart(TriggerMode) = 0;
        virtual void customStop() = 0;
        /** \brief Poll for new frames.
         This function should only be overried for devices with
         `FreeRunMode_PollEveryFrame` driver types
         \return `true` if new frame found.
         */
        virtual bool customPollFrame() { }
        virtual void customSetROI(const ofRectangle &) { }
        virtual void customOneShotCapture(ofPixels &) { };
        virtual void customSetExposure(int ms) { };
        //@}
        
        /**
         @name Threaded callbacks
         */
        //@{
        void pollForNewFrame();
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
        ofPixels pixels;
        void allocateTexture();
        bool useTexture;
        ofTexture texture;
        
        PollDeviceThread pollThread;
        
        bool isFrameNew;
        bool hasNewFrameWaiting;
        /** \endcond */
    };
}