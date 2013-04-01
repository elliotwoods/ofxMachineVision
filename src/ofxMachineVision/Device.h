/**
 \file      Device.h
 \author    Elliot Woods
 */

#pragma once

#include "ofBaseTypes.h"
#include "ofTexture.h"

#include <set>

namespace ofxMachineVision {
    /**
     \brief An abstract interface to be implemented by Machine Vision cameras.
     */
    class Device : ofBaseDraws, ofBaseUpdates, ofBaseHasTexture, ofBaseHasPixels {
    public:
        
        enum FreeRunMode {
            FreeRunMode_OwnThread,
            FreeRunMode_NeedsThread
        };
        
        enum DeviceFeature {
            Feature_ROI,
            Feature_Binning,
            Feature_PixelClock,
            Feature_Triggering
        };
        
        enum PixelMode {
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
        
        typedef std::set<DeviceFeature> DeviceFeatureSet;
        typedef std::set<PixelMode> PixelModeSet;
        typedef std::set<TriggerMode> TriggerModeSet;
        typedef std::set<TriggerSignalType> TriggerSignalTypeSet;
        
        virtual ~Device();
        
        /**
         @name Setup
         */
        //@{
        void open(int deviceID = 0);
        bool isOpen() const;
        void close();
        string getDetailedInfo() const;
        //@}

        /**
         @name Product information
         */
        //@{
        const string & getManufacturer() const { return this->manufacturer; };
        const string & getModelName() const { return this->modelName; };        
        //@}
        
        /**
         @name Capture resolution settings
         */
        //@{
        /** \brief Set resolution of capture frame */
        int getSensorWidth() const;
        int getSensorHeight() const;
        void setBinning(int binningX = 1, int binningY = 1);
        const DeviceFeatureSet & getDeviceFeatures() const { return this->deviceFeatures; }
        const PixelModeSet & getPixelModes() const { return this->devicePixelModes; }
        //@}
        
        /**
         @name Triggered capture
         Triggered capture (sometimes referred to as 'Free Run Mode') is a mode where a frame is
         captured from the device every time a trigger is given. The default trigger is the
         "Device Trigger" which fires every (1 / fps). Other triggers (e.g. GPIO triggers) cause
         the device only to trigger on a specific event (e.g. electrical signal on a GPIO).
         */
        //@{
        void startTriggeredCapture(TriggerMode = Trigger_Device);
        void stopTriggeredCapture();
        bool isTriggeredCaptureRunning() const;
        const TriggerMode & getTriggerMode() const;
        void setTriggerMode(const TriggerMode &);
        const TriggerModeSet & getDeviceTriggerModes() const { return this->deviceTriggerModes; }
        const TriggerSignalTypeSet & getDeviceTriggerSignalTypes() const { return this->deviceTriggerSignalTypes; }
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
        void draw(float x, float y);
        void draw(float x, float y, float w, float h) {
            this->draw(x, y, this->getWidth(), this->getHeight());
        }
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
        
        /**
         @name ofBaseHasTexture
         */
        //@{
        ofTexture & getTextureReference() { return this->texture; }
        void setUseTexture(bool useTexture);
        //@}
        
        /**
         @name ofBaseHasPixels
         */
        //@{
        unsigned char * getPixels() { return this->pixels.getPixels(); }
        ofPixels & getPixelsRef() { return this->pixels; }
        //@}
        
        /**
         @name toString helpers
         */
        //@{
        static string toString(const DeviceFeature &);
        static string toString(const PixelMode &);
        static string toString(const TriggerMode &);
        static string toString(const TriggerSignalType &);
        //@}
        
    protected:
        /**
         \brief Your subclass must declare some parameters for operation when it calls Device's constructor.
         @param freeRunMode use freeRunMode_OwnThread or freeRunMode_NeedsThread
         */
        Device(FreeRunMode freeRunMode);
        
        /**
         @name Custom camera functions
         \brief Override these functions in your custom camera class
         */
        //@{
        /** \brief Open the device.
         This function should populate:
         * deviceFeatures
         * devicePixelModes
         * deviceTriggerModes
         * manufacturer
         * modelName
         */
        virtual bool customOpen();
        virtual void customClose();
        virtual bool customStart();
        virtual void customStop();
        virtual void customSetROI(const ofRectangle &) { }
        virtual void customCaptureFrame() = 0;
        virtual void customSetExposure(int ms);
        //@}
        
        /** \cond PRIVATE */
        float fps;
        ofRectangle roi;
        ofTexture texture;
        ofPixels pixels;
        /** \endcond */
    private:
        FreeRunMode driverFreeRunMode;
        DeviceFeatureSet deviceFeatures;
        PixelModeSet devicePixelModes;
        TriggerModeSet deviceTriggerModes;
        TriggerSignalTypeSet deviceTriggerSignalTypes;
        string manufacturer;
        string modelName;
    };
}