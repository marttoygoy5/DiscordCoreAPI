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
        shared_ptr<DispatcherQueue> launcherQueue{ nullptr };
    };    
    
    class SystemThreads  {
    public:

        DispatcherQueueController MessageQueueController{ nullptr };
        DispatcherQueue MessageThreadQueue{ nullptr };
        DispatcherQueueController LauncherQueueController{ nullptr };
        DispatcherQueue LauncherThreadQueue{ nullptr };
        vector<DispatcherQueue> threadQueues;
        vector<ThreadContext> Threads;

        SystemThreads() {
            this->MaxThreads = thread::hardware_concurrency();
        };

        task<void> initialize() {
            cout << "THREAD ID  00: " << this_thread::get_id() << endl;
            this->MessageQueueController = this->MessageQueueController.CreateOnDedicatedThread();
            this->MessageThreadQueue = this->MessageQueueController.DispatcherQueue();
            co_await resume_background();
            this->LauncherQueueController = this->LauncherQueueController.CreateOnDedicatedThread();
            this->LauncherThreadQueue = this->LauncherQueueController.DispatcherQueue();
            cout << "THREAD ID  11: " << this_thread::get_id() << endl;
            co_await resume_foreground(this->MessageThreadQueue);
            cout << "THREAD ID  22: " << this_thread::get_id() << endl;
            for (unsigned int x = 0; x < this->MaxThreads - 2; x += 1) {
                ThreadContext threadContext;
                threadContext.launcherQueue = make_shared<DispatcherQueue>(this->LauncherThreadQueue);
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                DispatcherQueue threadQueue = threadQueueController.DispatcherQueue();
                this->threadQueues.push_back(threadQueue);
                SchedulerPolicy policy;
                policy.SetConcurrencyLimits(1, 1);
                policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
                policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
                co_await resume_foreground(this->threadQueues.at(x));
                cout << "THREAD ID 2323: " << this_thread::get_id() << endl;
                Scheduler* newScheduler = Scheduler::Create(policy);
                threadContext.scheduler = newScheduler;
                threadContext.threadQueue = make_shared<DispatcherQueue>(this->threadQueues.at(x));
                task_group* newTaskGroup;
                newTaskGroup = new task_group;
                threadContext.taskGroup = newTaskGroup;
                newScheduler->Attach();
                this->Threads.push_back(threadContext);
                cout << "THREAD ID  33: " << this_thread::get_id() << endl;
                co_await resume_foreground(this->MessageThreadQueue);
                cout << "THREAD ID  44: " << this_thread::get_id() << endl;
            }
        }

        ~SystemThreads() {

        };

    protected:
        unsigned int MaxThreads;

    };
    
}
#endif
