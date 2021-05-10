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
        Scheduler* scheduler;
        shared_ptr<task_group> taskGroup;
        shared_ptr<DispatcherQueue> threadQueue{ nullptr };
    };    

    struct SystemThreads : implements<SystemThreads, winrt::Windows::Foundation::IInspectable> {
    public:

        ThreadContext mainThreadContext;

        vector<ThreadContext> Threads;

        SystemThreads() {
            this->MaxThreads = thread::hardware_concurrency();
        };

        task<void> initialize() {
            DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_CURRENT,
                DQTAT_COM_STA
            };
            ABI::Windows::System::IDispatcherQueueController* ptrNew{};
            winrt::check_hresult(CreateDispatcherQueueController(options, &ptrNew));
            DispatcherQueueController queueController = { ptrNew, take_ownership_from_abi };
            mainThreadContext.threadQueue = make_shared<DispatcherQueue>(queueController.DispatcherQueue());
            SchedulerPolicy policy;
            policy.SetConcurrencyLimits(1, 1);
            policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
            policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
            CurrentScheduler::Create(policy);
            mainThreadContext.scheduler = CurrentScheduler::Get();
            shared_ptr<task_group> newTaskGroup = make_shared<task_group>();
            mainThreadContext.taskGroup = newTaskGroup;
            for (unsigned int x = 0; x < this->MaxThreads - 2; x += 1) {
                co_await resume_background();
                ThreadContext threadContext;
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                DispatcherQueue threadQueue = threadQueueController.DispatcherQueue();
                SchedulerPolicy policy;
                policy.SetConcurrencyLimits(1, 1);
                policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
                policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
                co_await resume_foreground(threadQueue);
                Scheduler* newScheduler = Scheduler::Create(policy);
                threadContext.scheduler = newScheduler;
                threadContext.threadQueue = make_shared<DispatcherQueue>(threadQueue);
                shared_ptr<task_group> newTaskGroup = make_shared<task_group>();
                threadContext.taskGroup = newTaskGroup;
                newScheduler->Attach();
                this->Threads.push_back(threadContext);
            }
        }

        ~SystemThreads() {

        };

    protected:
        unsigned int MaxThreads;

    };
    
}
#endif
