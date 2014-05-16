#pragma once

#include "ofThread.h"
#include "Poco/Any.h"
#include <queue>

using namespace std;

namespace ofxMachineVision {
	namespace Utils {
		typedef std::function<void()> ActionFunction;
		class ActionQueueThread : public ofThread {
		public:
			void perform(std::function<void ()>, bool blocking = true);
			void blockUntilEmpty();
		protected:
			void threadedFunction() override;
			virtual void idleFunction() = 0; //< override this function

			ofMutex lockFunctionQueue;
			queue<ActionFunction> functionQueue;
		};
	}
}