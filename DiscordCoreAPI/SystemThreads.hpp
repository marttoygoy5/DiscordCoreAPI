// SystemThreads.hpp - Header for the "system threads" class.
// May 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SYSTEM_THREADS_
#define _SYSTEM_THREADS_

#include "pch.h"

namespace DiscordCoreInternal {

    class SystemThreads;

    struct ThreadContext {
        Scheduler* scheduler{ nullptr };
        shared_ptr<task_group> taskGroup{ nullptr };
        shared_ptr<DispatcherQueue> threadQueue{ nullptr };
    };

    class SystemThreadsAgent :public agent {
    protected:
        friend class SystemThreads;
        unbounded_buffer<bool> requestBuffer;
        unbounded_buffer<concurrent_vector<ThreadContext>*> submitBuffer;
        unbounded_buffer<concurrent_vector<ThreadContext>*> responseBuffer;
        single_assignment<exception> errorBuffer;
        
        SystemThreadsAgent(concurrent_vector<ThreadContext>* threads)
            : agent(*threads->at(0).scheduler)
        {}

        bool getError(exception& error) {
            if (try_receive(errorBuffer, error)) {
                return true;
            }
            return false;
        }

        task<void> submitThreads(concurrent_vector<ThreadContext>* threads) {
            send(submitBuffer, threads);
            co_return;
        }

        void run() {
            try {
                receive(requestBuffer, 1U);
                concurrent_vector<ThreadContext>* threads = receive(submitBuffer);
                send(responseBuffer, threads);
            }
            catch (const exception& e) {
                send(errorBuffer, e);
            }
            done();
        }
    };

    class SystemThreads  {
    public:

        ThreadContext mainThreadContext;
        static concurrent_vector<ThreadContext>* threads;

        task<concurrent_vector<ThreadContext>*> getThreads() {
            SystemThreadsAgent systemThreadsAgent(SystemThreads::threads);
            systemThreadsAgent.start();
            systemThreadsAgent.submitThreads(SystemThreads::threads).get();
            send(systemThreadsAgent.requestBuffer, true);
            concurrent_vector<ThreadContext>* threadsNew = receive(systemThreadsAgent.responseBuffer);
            agent::wait(&systemThreadsAgent);
            exception error;
            if (systemThreadsAgent.getError(error)) {
                cout << "SystemThreads::getThreads() Error: " << error.what() << endl << endl;
            }
            co_return threadsNew;
        }

        SystemThreads() {
            this->MaxThreads = thread::hardware_concurrency();
        };

        task<void> initialize() {
            DispatcherQueueOptions options{
                sizeof(DispatcherQueueOptions),
                DQTYPE_THREAD_DEDICATED,
                DQTAT_COM_ASTA
            };
            ABI::Windows::System::IDispatcherQueueController* ptrNew{};
            winrt::check_hresult(CreateDispatcherQueueController(options, &ptrNew));
            DispatcherQueueController queueController = { ptrNew, take_ownership_from_abi };
            this->mainThreadContext.threadQueue = make_shared<DispatcherQueue>(queueController.DispatcherQueue());
            SchedulerPolicy policy;
            policy.SetConcurrencyLimits(1, 1);
            policy.SetPolicyValue(PolicyElementKey::ContextPriority, THREAD_PRIORITY_HIGHEST);
            policy.SetPolicyValue(PolicyElementKey::DynamicProgressFeedback, ProgressFeedbackEnabled);
            policy.SetPolicyValue(PolicyElementKey::LocalContextCacheSize, 1);
            policy.SetPolicyValue(PolicyElementKey::SchedulerKind, ThreadScheduler);
            policy.SetPolicyValue(PolicyElementKey::SchedulingProtocol, EnhanceForwardProgress);
            policy.SetPolicyValue(PolicyElementKey::TargetOversubscriptionFactor, 1);
            policy.SetPolicyValue(PolicyElementKey::WinRTInitialization, InitializeWinRTAsMTA);
            CurrentScheduler::Create(policy);
            this->mainThreadContext.scheduler = CurrentScheduler::Get();
            shared_ptr<task_group> newTaskGroup = make_shared<task_group>();
            this->mainThreadContext.taskGroup = newTaskGroup;
            SystemThreads::threads = new concurrent_vector<ThreadContext>();
            for (unsigned int x = 0; x < this->MaxThreads - 1; x += 1) {
                DispatcherQueueController threadQueueController = DispatcherQueueController::CreateOnDedicatedThread();
                ABI::Windows::System::IDispatcherQueueController* ptrNew2{};
                winrt::check_hresult(CreateDispatcherQueueController(options, &ptrNew2));
                DispatcherQueueController queueController2 = { ptrNew2, take_ownership_from_abi };
                ThreadContext threadContext;
                DispatcherQueue threadQueue = queueController2.DispatcherQueue();
                policy.SetConcurrencyLimits(1, 1);
                apartment_context mainThread;
                co_await resume_foreground(threadQueue);
                Scheduler* newScheduler = Scheduler::Create(policy);
                threadContext.scheduler = newScheduler;
                threadContext.threadQueue = make_shared<DispatcherQueue>(threadQueue);
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
