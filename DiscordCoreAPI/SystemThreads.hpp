// SystemThreads.hpp - Header for the "system threads" class.
// May 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SYSTEM_THREADS_
#define _SYSTEM_THREADS_

#include "pch.h"

namespace CommanderNS {

    struct ThreadContext {
        task_group* taskGroup;
        Scheduler* scheduler;
        shared_ptr<DispatcherQueue> threadQueue{ nullptr };
    };
    
    class SystemThreads  {
    public:

        apartment_context mainThread;

        vector<ThreadContext> Threads;

        SystemThreads() {
            this->MaxThreads = thread::hardware_concurrency();
        };

        task<void> initialize() {
            cout << "THREAD ID  00: " << this_thread::get_id() << endl;
            for (unsigned int x = 0; x < this->MaxThreads - 2; x += 1) {
                co_await resume_background();
                cout << "THREAD ID  11: " << this_thread::get_id() << endl;
                ThreadContext threadContext;
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                DispatcherQueue threadQueue = threadQueueController.DispatcherQueue();
                SchedulerPolicy policy;
                policy.SetConcurrencyLimits(1, 1);
                policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
                policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
                co_await resume_foreground(threadQueue);
                cout << "THREAD ID  22: " << this_thread::get_id() << endl;
                Scheduler* newScheduler = Scheduler::Create(policy);
                threadContext.scheduler = newScheduler;
                threadContext.threadQueue = make_shared<DispatcherQueue>(threadQueue);
                task_group* newTaskGroup;
                newTaskGroup = new task_group;
                threadContext.taskGroup = newTaskGroup;
                newScheduler->Attach();
                this->Threads.push_back(threadContext);
                cout << "THREAD ID  33: " << this_thread::get_id() << endl;
            }
        }

        ~SystemThreads() {

        };

    protected:
        unsigned int MaxThreads;

    };
    
}
#endif
