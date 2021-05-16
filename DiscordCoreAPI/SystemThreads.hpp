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

    class SystemThreadsAgent :public agent{
    public:
        unbounded_buffer<bool> requestBuffer;
        unbounded_buffer<concurrent_vector<ThreadContext>> submitBuffer;
        unbounded_buffer<concurrent_vector<ThreadContext>> responseBuffer;

        SystemThreadsAgent(concurrent_vector<ThreadContext> Threads, com_ptr<SystemThreads> pSystemThreadsNew)
            : agent(*Threads.at(0).scheduler)
        {
            this->pSystemThreads = pSystemThreadsNew;
        }

        task<void> submitThreads(concurrent_vector<ThreadContext> threads) {
            send(submitBuffer, threads);
            co_return;
        }

        ~SystemThreadsAgent() {}

    protected:
        com_ptr<SystemThreads> pSystemThreads;
        void run() {
            bool startVal = receive(requestBuffer, 50U);
            concurrent_vector<ThreadContext> threads = receive(submitBuffer);
            send(responseBuffer, threads);
            done();
        }
    };

    struct SystemThreads : implements<SystemThreads, winrt::Windows::Foundation::IInspectable> {
    public:

        ThreadContext mainThreadContext;

        concurrent_vector<ThreadContext> Threads;
        shared_ptr<concurrent_vector<ThreadContext>> pThreads;

        task<shared_ptr<concurrent_vector<ThreadContext>>> getThreads() {
            com_ptr<SystemThreads> pSystemThreads;
            pSystemThreads.attach(this);
            SystemThreadsAgent systemThreadsAgent(this->Threads, pSystemThreads);
            systemThreadsAgent.start();
            systemThreadsAgent.submitThreads(this->Threads).get();
            send(systemThreadsAgent.requestBuffer, true);
            concurrent_vector<ThreadContext> threadsNew = receive(systemThreadsAgent.responseBuffer);
            co_return this->pThreads;
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
            mainThreadContext.threadQueue = make_shared<DispatcherQueue>(queueController.DispatcherQueue());
            SchedulerPolicy policy;
            policy.SetConcurrencyLimits(1, 1);
            policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
            policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
            CurrentScheduler::Create(policy);
            mainThreadContext.scheduler = CurrentScheduler::Get();
            shared_ptr<task_group> newTaskGroup = make_shared<task_group>();
            mainThreadContext.taskGroup = newTaskGroup;
            for (unsigned int x = 0; x < this->MaxThreads - 1; x += 1) {
                co_await resume_foreground(*this->mainThreadContext.threadQueue.get());
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                ThreadContext threadContext;
                DispatcherQueue threadQueue = threadQueueController.DispatcherQueue();
                policy.SetConcurrencyLimits(1, 1);
                policy.SetPolicyValue(concurrency::PolicyElementKey::ContextPriority, THREAD_PRIORITY_ABOVE_NORMAL);
                policy.SetPolicyValue(concurrency::PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
                co_await resume_foreground(threadQueue);
                Scheduler* newScheduler = Scheduler::Create(policy);
                threadContext.scheduler = newScheduler;
                threadContext.threadQueue = make_shared<DispatcherQueue>(threadQueue);
                newTaskGroup = make_shared<task_group>();
                threadContext.taskGroup = newTaskGroup;
                newScheduler->Attach();
                this->Threads.push_back(threadContext);
            }
            this->pThreads = make_shared<concurrent_vector<ThreadContext>>(this->Threads);
        }

    protected:
        friend class SystemThreadsAgent;
        unsigned int MaxThreads;

    };
    
}
#endif
