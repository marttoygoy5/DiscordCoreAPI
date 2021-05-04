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
        DispatcherQueue threadQueue{ nullptr };
        shared_ptr<DispatcherQueue> messageQueue{ nullptr };
    };        

    struct SystemThreads : implements<SystemThreads, winrt::Windows::Foundation::IInspectable> {
    public:

        DispatcherQueueController MessageQueueController{ nullptr };
        DispatcherQueue MessageThreadQueue{ nullptr };
        DispatcherQueueController LauncherQueueController{ nullptr };
        DispatcherQueue LauncherThreadQueue{ nullptr };
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
                threadContext.messageQueue = make_shared<DispatcherQueue>(this->MessageThreadQueue);
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                DispatcherQueue threadQueue = threadQueueController.DispatcherQueue();
                threadContext.threadQueue = threadQueue;
                SchedulerPolicy policy;
                policy.SetConcurrencyLimits(1, 1);
                policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
                policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
                co_await resume_foreground(threadContext.threadQueue);
                cout << "THREAD ID 2323: " << this_thread::get_id() << endl;
                Scheduler* newScheduler = Scheduler::Create(policy);
                threadContext.scheduler = newScheduler;
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