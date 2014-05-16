#pragma once

#include <queue>
#include <array>
	
#include "ofThread.h"
#include "ofEvents.h"

#include "ofxMachineVision/Constants.h"
#include "ofxMachineVision/Utils/ActionQueueThread.h"
#include "ofxMachineVision/Device/Blocking.h"

namespace ofxMachineVision {
	namespace Grabber {

		class Base;

		namespace Thread {
			/**
			\brief Thread::Blocking is a thread for handling Device::Blocking type devices
			We presume that the thread is running whilst the device is open
			*/
			class Blocking : public Utils::ActionQueueThread {
			public:
				Blocking(shared_ptr<Device::Blocking>, Grabber::Base *);
				~Blocking();
            
				/**
				\brief Open device and block until complete.
				\param deviceID The ID of the device to open. Perhaps this becomes Poco::Any later
				*/
				void open(int deviceID = 0);
				/**
				\brief Close device and block until complete
				*/
				void close();
				void startFreeRun();
				void stopFreeRun();
				void setExposure(Microseconds exposure);
				void setGain(float percent);
				void setFocus(float percent);
				void setBinning(int binningX = 1, int binningY = 1);
				void setROI(const ofRectangle &);
				void setTriggerMode(const TriggerMode &, const TriggerSignalType &);
				void setGPOMode(const GPOMode &);

				ofEvent<FrameEventArgs> evtNewFrame;
			protected:
				void idleFunction() override;
				ofMutex actionQueueLock;
				shared_ptr<Device::Blocking> device;
				Grabber::Base * grabber;
				shared_ptr<Frame> frame;
			};
		}
	}
}