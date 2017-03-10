#include "ActionQueueThread.h"
#include "ofAppRunner.h"

#include "../Constants.h"

namespace ofxMachineVision {
	namespace Utils {
		//-----------
		void ActionQueueThread::setIdleFunction(ActionFunction function) {
			this->idleFunction = function;
		}

		//-----------
		void ActionQueueThread::performInThread(ActionFunction action, bool blocking) {
			if (!this->isThreadRunning()) {
				ofLogWarning("ofxMachineVision") << "Cannot add items to ActionQueueThread whilst it is closed / closing";
				return;
			}

			this->lockFunctionQueue.lock();
			functionQueue.push(action);
			this->lockFunctionQueue.unlock();

			if (blocking) {
				this->blockUntilEmpty();
			}
		}

		//----------
		void ActionQueueThread::blockUntilEmpty() {
			bool empty = false;
			while (!empty) {
				ofSleepMillis(1);
				this->lockFunctionQueue.lock();
				empty = this->functionQueue.empty();
				this->lockFunctionQueue.unlock();
			}
		}

		//----------
		void ActionQueueThread::threadedFunction() {
			while(this->isThreadRunning()) {
				while (true) {
					ActionFunction action;
					this->lockFunctionQueue.lock();
					if (!this->functionQueue.empty()) {
						action = this->functionQueue.front();
					}
					this->lockFunctionQueue.unlock();

					if (action) {
						try {
							action();
						}
						OFXMV_CATCH_ALL_TO_ERROR;

						this->lockFunctionQueue.lock();
						{
							this->functionQueue.pop();
						}
						this->lockFunctionQueue.unlock();
					}
					else {
						break;
					}
				}

				if (this->idleFunction) {
					this->idleFunction();
				}
			}
		}
	}
}