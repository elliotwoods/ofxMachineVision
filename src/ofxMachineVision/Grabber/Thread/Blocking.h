#pragma once

#include <queue>
#include <array>
#include "Poco/Any.h"
	
#include "ofThread.h"
#include "ofEvents.h"

#include "ofxMachineVision/Constants.h"
#include "ofxMachineVision/Device/Blocking.h"

namespace ofxMachineVision {
	namespace Grabber {

		class Base;

		namespace Thread {
			/**
			\brief Thread::Blocking is a thread for handling Device::Blocking type devices
			We presume that the thread is running whilst the device is open
			*/
			class Blocking : public ofThread {
			public:
				class Action {
				public:
					enum Type {
						Type_Open,
						Type_Close,
						Type_StartFreeRun,
						Type_StopFreeRun,
						Type_SetExposure,
						Type_SetGain,
						Type_SetFocus,
						Type_SetBinning,
						Type_SetROI,
						Type_SetTriggerSettings,
						Type_SetGPOMode
					};

					Action(const Type &);
					Action(const Type &, Poco::Any);

					const Type & getType() const { return this->type; }
					const Poco::Any & getArguments() const { return this->arguments; }
					
					template<typename T>
					T getArgument() const {
						if (this->getArguments().type() == typeid(T)) {
							return Poco::AnyCast<T>(this->getArguments());
						} else {
							return T();
						}
					}

					template<typename T, int size>
					array<T, size> getArrayArgument() const {
						return this->getArgument<array<T, size>>();
					}
				protected:
					Type type;
					Poco::Any arguments;
				};

				Blocking(ofPtr<Device::Blocking>, Grabber::Base *);
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

				void addAction(const Action &, bool blockUntilComplete = false);
				void blockUntilActionQueueEmpty();

				ofEvent<FrameEventArgs> evtNewFrame;
			protected:
				void threadedFunction();
				queue<Action> actionQueue;
				ofMutex actionQueueLock;
				ofPtr<Device::Blocking> device;
				Grabber::Base * grabber;
				ofPtr<Frame> frame;
			};
		}
	}
}