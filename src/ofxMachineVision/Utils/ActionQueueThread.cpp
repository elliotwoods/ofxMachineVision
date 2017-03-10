#include "ActionQueueThread.h"
#include "ofAppRunner.h"

#include "../Constants.h"

namespace ofxMachineVision {
	namespace Utils {
		//----------
		ActionQueueThread::ActionQueueThread() {
			this->thread = std::thread([this]() {
				while (!this->needsCloseThread) {
					ActionFunction actionFunction;
					while (actionQueue.tryReceive(actionFunction)) {
						try {
							actionFunction();
						}
						OFXMV_CATCH_ALL_TO_ERROR;
					}

					if (this->idleFunction) {
						try {
							this->idleFunction();
						}
						OFXMV_CATCH_ALL_TO_ERROR;
					}
					else {
						//no idle function, we can wait
						this_thread::sleep_for(chrono::milliseconds(1));
					}
				}
			});
		}

		//----------
		ActionQueueThread::~ActionQueueThread() {
			this->needsCloseThread = true;
			this->thread.join();
		}

		//-----------
		void ActionQueueThread::setIdleFunction(ActionFunction function) {
			this->idleFunction = function;
		}

		//-----------
		void ActionQueueThread::performInThread(ActionFunction && action, bool blocking) {
			if (blocking) {
				//wrap the action in another action which will respond when complete
				ofThreadChannel<int> responder;
				ActionFunction wrappedAction = [action, &responder]() {
					action();
					responder.send(NULL);
				};
				
				//send the wrappedAction to the thread
				this->actionQueue.send(move(wrappedAction));
				
				//block until response arrives
				int response;
				responder.receive(response);
			}
			else {
				this->actionQueue.send(move(action));
			}
		}

		//----------
		void ActionQueueThread::blockUntilQueueEmpty() {
			while (!this->actionQueue.empty()) {
				this_thread::sleep_for(chrono::milliseconds(1));
			}
		}
	}
}