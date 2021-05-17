// SystemThreads.hpp - Header for the "system threads" class.
// May 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SYSTEM_THREADS_
#define _SYSTEM_THREADS_

#include "pch.h"

namespace DiscordCoreInternal {

    struct SystemThreads;

    struct ThreadContext {
        Scheduler* scheduler;
        shared_ptr<task_group> taskGroup;
        shared_ptr<DispatcherQueue> threadQueue{ nullptr };
    };

    class SystemThreadsAgent :public agent {
    protected:
        friend struct SystemThreads;
        unbounded_buffer<bool> requestBuffer;
        unbounded_buffer<concurrent_vector<ThreadContext>*> submitBuffer;
        unbounded_buffer <concurrent_vector<ThreadContext>*> responseBuffer;

        SystemThreadsAgent(concurrent_vector<ThreadContext>* threads)
            : agent(*threads->at(0).scheduler)
        {}

        task<void> submitThreads(concurrent_vector<ThreadContext>* threads) {
            send(submitBuffer, threads);
            co_return;
        }

        void run() {
            bool startVal = receive(requestBuffer, 50U);
            concurrent_vector<ThreadContext>* threads = receive(submitBuffer);
            send(responseBuffer, threads);
            done();
        }
    };

    struct SystemThreads : implements<SystemThreads, winrt::Windows::Foundation::IInspectable> {
    public:

        ThreadContext mainThreadContext;
        static concurrent_vector<ThreadContext>* threads;

        task<concurrent_vector<ThreadContext>*> getThreads() {
            SystemThreadsAgent systemThreadsAgent(SystemThreads::threads);
            systemThreadsAgent.start();
            systemThreadsAgent.submitThreads(SystemThreads::threads).get();
            send(systemThreadsAgent.requestBuffer, true);
            concurrent_vector<ThreadContext>* threadsNew = receive(systemThreadsAgent.responseBuffer);
            co_return threadsNew;
        }

        SystemThreads() {
            this->MaxThreads = thread::hardware_concurrency();
        };

        task<void> initialize() {
            DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_DEDICATED,
                DQTAT_COM_STA
            };
            ABI::Windows::System::IDispatcherQueueController* ptrNew{};
            winrt::check_hresult(CreateDispatcherQueueController(options, &ptrNew));
            DispatcherQueueController queueController = { ptrNew, take_ownership_from_abi };
            this->mainThreadContext.threadQueue = make_shared<DispatcherQueue>(queueController.DispatcherQueue());
            SchedulerPolicy policy;
            policy.SetConcurrencyLimits(1, 1);
            policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
            policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
            CurrentScheduler::Create(policy);
            this->mainThreadContext.scheduler = CurrentScheduler::Get();
            shared_ptr<task_group> newTaskGroup = make_shared<task_group>();
            this->mainThreadContext.taskGroup = newTaskGroup;
            SystemThreads::threads = new concurrent_vector<ThreadContext>();
            for (unsigned int x = 0; x < this->MaxThreads - 1; x += 1) {
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                ThreadContext threadContext;
                DispatcherQueue threadQueue = threadQueueController.DispatcherQueue();
                policy.SetConcurrencyLimits(1, 1);
                policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
                policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
                apartment_context mainThread;
                Scheduler* newScheduler = Scheduler::Create(policy);
                threadContext.scheduler = newScheduler;
                threadContext.threadQueue = make_shared<DispatcherQueue>(threadQueue);
                co_await resume_foreground(threadQueue);
                newTaskGroup = make_shared<task_group>();
                threadContext.taskGroup = newTaskGroup;
                newScheduler->Attach();
                co_await mainThread;
                SystemThreads::threads->push_back(threadContext);
            }
        }

    protected:
        friend class SystemThreadsAgent;
        unsigned int MaxThreads;

    };
    concurrent_vector<ThreadContext>* SystemThreads::threads;
}
#endif
