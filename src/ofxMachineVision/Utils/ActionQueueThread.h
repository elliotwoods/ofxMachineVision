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
			void setIdleFunction(ActionFunction);
			void performInThread(ActionFunction, bool blocking = true);
			void blockUntilEmpty();
		protected:
			void threadedFunction() override;
			std::function<void()> idleFunction;

			ofMutex lockFunctionQueue;
			queue<ActionFunction> functionQueue;
		};
	}
}