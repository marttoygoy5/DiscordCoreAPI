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
        DispatcherQueueTimer queueTimer{ nullptr };
    };

    struct SystemThreads : implements<SystemThreads, winrt::Windows::Foundation::IInspectable> {
    public:

        static ThreadContext mainThreadContext;

        concurrent_vector<ThreadContext> Threads;

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
            mainThreadContext.queueTimer = mainThreadContext.threadQueue->CreateTimer();
            SchedulerPolicy policy01;
            policy01.SetConcurrencyLimits(1, 1);
            policy01.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
            policy01.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
            CurrentScheduler::Create(policy01);
            mainThreadContext.scheduler = CurrentScheduler::Get();
            mainThreadContext.taskGroup = make_shared<task_group>();;
            for (unsigned int x = 0; x < this->MaxThreads - 2; x += 1) {
                co_await resume_background();
                ThreadContext threadContext;
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                SchedulerPolicy policy02;
                policy02.SetConcurrencyLimits(1, 1);
                policy02.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
                policy02.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
                threadContext.threadQueue = make_shared<DispatcherQueue>(threadQueueController.DispatcherQueue());
                co_await resume_foreground(*threadContext.threadQueue.get());
                CurrentScheduler::Create(policy02);
                threadContext.scheduler = CurrentScheduler::Get();
                threadContext.queueTimer = threadContext.threadQueue->CreateTimer();
                threadContext.taskGroup = make_shared<task_group>();
                this->Threads.push_back(threadContext);
            }
        }

        ~SystemThreads() {
        };

    protected:
        unsigned int MaxThreads;

    };
    
    ThreadContext SystemThreads::mainThreadContext;
}
#endif
