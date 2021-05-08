// HTTPHandler.hpp - The header for the HTTPHandler class.
// May 8, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_HANDLER_
#define _HTTP_HANDLER_

#include "pch.h"
#include "HttpAgents.hpp"

namespace CommanderNS {

	struct HTTPHandler: implements<HTTPHandler, winrt::Windows::Foundation::IInspectable> {
	public:
		HTTPHandler(com_ptr<RestAPI> pRestAPI) {
			
			this->pRestAPI = pRestAPI;
			this->GETHandler = make_self<HttpAgents::HttpHandler>(this->pRestAPI->pSystemThreads->Threads.at(2).scheduler, this->pRestAPI, this->GETbuffer1, this->GETbuffer2);
			this->GETSender = make_self<HttpAgents::RequestSender>(this->pRestAPI->pSystemThreads->Threads.at(2).scheduler, this->GETbuffer2, this->GETbuffer1);
			this->POSTHandler = make_self<HttpAgents::HttpHandler>(this->pRestAPI->pSystemThreads->Threads.at(3).scheduler, this->pRestAPI, this->POSTbuffer1, this->POSTbuffer2);
			this->POSTSender = make_self<HttpAgents::RequestSender>(this->pRestAPI->pSystemThreads->Threads.at(3).scheduler, this->POSTbuffer2, this->POSTbuffer1);
			this->PUTHandler = make_self<HttpAgents::HttpHandler>(this->pRestAPI->pSystemThreads->Threads.at(4).scheduler, this->pRestAPI, this->PUTbuffer1, this->PUTbuffer2);
			this->PUTSender = make_self<HttpAgents::RequestSender>(this->pRestAPI->pSystemThreads->Threads.at(4).scheduler, this->PUTbuffer2, this->PUTbuffer1);
			this->DELETEHandler = make_self<HttpAgents::HttpHandler>(this->pRestAPI->pSystemThreads->Threads.at(5).scheduler, this->pRestAPI, this->DELETEbuffer1, this->DELETEbuffer2);
			this->DELETESender = make_self<HttpAgents::RequestSender>(this->pRestAPI->pSystemThreads->Threads.at(5).scheduler, this->DELETEbuffer2, this->DELETEbuffer1);
			this->GETHandler.as<HttpAgents::HttpHandler>()->start();
			this->GETSender.as<HttpAgents::RequestSender>()->start();
			this->POSTHandler.as<HttpAgents::HttpHandler>()->start();
			this->POSTSender.as<HttpAgents::RequestSender>()->start();
			this->PUTHandler.as<HttpAgents::HttpHandler>()->start();
			this->PUTSender.as<HttpAgents::RequestSender>()->start();
			this->DELETEHandler.as<HttpAgents::HttpHandler>()->start();
			this->DELETESender.as<HttpAgents::RequestSender>()->start();
	
		}

		json getPOSTData(WorkloadData workloadData) {
			POSTSender->setWorkloadData(workloadData);
			agent::wait(this->POSTSender.get());
			agent::wait(this->POSTHandler.get());
			json jsonValue = this->POSTSender->getData();
			return jsonValue;
		}

		~HTTPHandler() {};

		com_ptr<HttpAgents::HttpHandler> GETHandler{ nullptr };
		com_ptr<HttpAgents::HttpHandler> POSTHandler{ nullptr };
		com_ptr<HttpAgents::HttpHandler> PUTHandler{ nullptr };
		com_ptr<HttpAgents::HttpHandler> DELETEHandler{ nullptr };
		com_ptr<HttpAgents::RequestSender> GETSender{ nullptr };
		com_ptr<HttpAgents::RequestSender> POSTSender{ nullptr };
		com_ptr<HttpAgents::RequestSender> PUTSender{ nullptr };
		com_ptr<HttpAgents::RequestSender> DELETESender{ nullptr };

		com_ptr<RestAPI>pRestAPI{ nullptr };
		unbounded_buffer<HTTPData> GETbuffer1;
		unbounded_buffer<WorkloadData> GETbuffer2;
		unbounded_buffer<HTTPData> POSTbuffer1;
		unbounded_buffer<WorkloadData> POSTbuffer2;
		unbounded_buffer<HTTPData> PUTbuffer1;
		unbounded_buffer<WorkloadData> PUTbuffer2;
		unbounded_buffer<HTTPData> DELETEbuffer1;
		unbounded_buffer<WorkloadData> DELETEbuffer2;
		
		

		
	};

}
#endif