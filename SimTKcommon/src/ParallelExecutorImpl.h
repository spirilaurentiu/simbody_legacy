#ifndef SimTK_SimTKCOMMON_PARALLEL_EXECUTOR_IMPL_H_
#define SimTK_SimTKCOMMON_PARALLEL_EXECUTOR_IMPL_H_

/* -------------------------------------------------------------------------- *
 *                       Simbody(tm): SimTKcommon                             *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK biosimulation toolkit originating from           *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org/home/simbody.  *
 *                                                                            *
 * Portions copyright (c) 2008-12 Stanford University and the Authors.        *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#include "SimTKcommon/internal/ParallelExecutor.h"
#include "SimTKcommon/internal/ThreadLocal.h"
#include "SimTKcommon/internal/Array.h"
#include <pthread.h>

namespace SimTK {

class ParallelExecutorImpl;

/**
 * This class stores per-thread information used while executing a task.
 */

class ThreadInfo {
public:
    ThreadInfo(int index, ParallelExecutorImpl* executor) : index(index), executor(executor), running(false) {
    }
    const int index;
    ParallelExecutorImpl* const executor;
    bool running;
};

/**
 * This is the internal implementation class for ParallelExecutor.
 */

class ParallelExecutorImpl : public PIMPLImplementation<ParallelExecutor, ParallelExecutorImpl> {
public:
    ParallelExecutorImpl(int numThreads);
    ~ParallelExecutorImpl();
    ParallelExecutorImpl* clone() const;
    void execute(ParallelExecutor::Task& task, int times);
    int getThreadCount() {
        return threads.size();
    }
    ParallelExecutor::Task& getCurrentTask() {
        return *currentTask;
    }
    int getCurrentTaskCount() {
        return currentTaskCount;
    }
    bool isFinished() {
        return finished;
    }
    pthread_mutex_t* getLock() {
        return &runLock;
    }
    pthread_cond_t* getCondition() {
        return &runCondition;
    }
    void incrementWaitingThreads();
    static ThreadLocal<bool> isWorker;
private:
    bool finished;
    pthread_mutex_t runLock;
    pthread_cond_t runCondition, waitCondition;
    Array_<pthread_t> threads;
    Array_<ThreadInfo*> threadInfo;
    ParallelExecutor::Task* currentTask;
    int currentTaskCount;
    int waitingThreadCount;
};

} // namespace SimTK

#endif // SimTK_SimTKCOMMON_PARALLEL_EXECUTOR_IMPL_H_
