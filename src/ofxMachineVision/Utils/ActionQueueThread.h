#pragma once

//#include "ofThread.h"
#include "ofThreadChannel.h"

using namespace std;

namespace ofxMachineVision {
	namespace Utils {
		typedef std::function<void()> ActionFunction;

		class ActionQueueThread {
		public:
			ActionQueueThread();
			virtual ~ActionQueueThread();

			void setIdleFunction(ActionFunction);

			// returns false if exception is thrown
			bool performInThread(ActionFunction &&, bool blocking);

			void blockUntilQueueEmpty();
		protected:
			std::function<void()> idleFunction;
			ofThreadChannel<ActionFunction> actionQueue;
			std::thread thread;
			bool needsCloseThread = false;
		};
	}
}