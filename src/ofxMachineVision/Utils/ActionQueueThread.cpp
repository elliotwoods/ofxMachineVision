#include "ActionQueueThread.h"
#include "ofAppRunner.h"

namespace ofxMachineVision {
	namespace Utils {
		//-----------
		void ActionQueueThread::setIdleFunction(std::function<void()> function) {
			this->idleFunction = function;
		}

		//-----------
		void ActionQueueThread::performInThread(std::function<void ()> action, bool blocking) {
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
						action();
						this->lockFunctionQueue.lock();
						this->functionQueue.pop();
						this->lockFunctionQueue.unlock();
					} else {
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