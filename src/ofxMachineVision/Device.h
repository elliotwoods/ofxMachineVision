#pragma once

/**
 \file      Device.h
 \author    Elliot Woods
*/

namespace ofxMachineVision {
    /**
     \brief An abstract interface to be implemented by Machine Vision cameras.
     */
    class Device : ofBaseDraws {
    public:
        Device();
        virtual ~Device();
        
        void setup(int deviceID = 0);
        void update();
        
        ////
        //ofBaseDraws
        void draw();
        //
        ////
    protected:
        
    private:
        int width, height;
    };
}