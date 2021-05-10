// HTTPControler.hpp - Header for the HTTP Controller class.
// May 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_CONTROLLER_
#define _HTTP_CONTROLLER_

#include "pch.h"
#include "FoundationClasses.hpp"
#include "HttpAgents.hpp"
#include "RestAPI.hpp"

namespace CommanderNS {

	struct HTTPController :implements<HTTPController, winrt::Windows::Foundation::IInspectable> {
	public:
		com_ptr<RestAPI> pRestAPI;
		concurrent_vector<ThreadContext>* pSystemThreads;
		unbounded_buffer<HttpAgents::WorkloadData> getBuffer00;
		unbounded_buffer<HTTPData> getBuffer03;
		com_ptr<HttpAgents::RequestSender> getRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> getHTTPHandler{ nullptr };

		unbounded_buffer<HttpAgents::WorkloadData> putBuffer00;
		unbounded_buffer<HTTPData> putBuffer03;
		com_ptr<HttpAgents::RequestSender> putRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> putHTTPHandler{ nullptr };

		unbounded_buffer<HttpAgents::WorkloadData> postBuffer00;
		unbounded_buffer<HTTPData> postBuffer03;
		com_ptr<HttpAgents::RequestSender> postRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> postHTTPHandler{ nullptr };

		unbounded_buffer<HttpAgents::WorkloadData> deleteBuffer00;
		unbounded_buffer<HTTPData> deleteBuffer03;
		com_ptr<HttpAgents::RequestSender> deleteRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> deleteHTTPHandler{ nullptr };

		HTTPController(concurrent_vector<ThreadContext>* pSystemThreads) {
			this->pRestAPI = pRestAPI;
			this->pSystemThreads = pSystemThreads;
			this->getHTTPHandler = make_self<HttpAgents::HTTPHandler>(pSystemThreads->at(2).scheduler, this->getBuffer02, this->getBuffer01);
			this->getRequestSender = make_self<HttpAgents::RequestSender>(pSystemThreads->at(2).scheduler, this->getBuffer00, this->getBuffer01, this->getBuffer02, this->getBuffer03);
			this->putHTTPHandler = make_self<HttpAgents::HTTPHandler>(pSystemThreads->at(3).scheduler, this->putBuffer02, this->putBuffer01);
			this->putRequestSender = make_self<HttpAgents::RequestSender>(pSystemThreads->at(3).scheduler, this->putBuffer00, this->putBuffer01, this->putBuffer02, this->putBuffer03);
			this->postHTTPHandler = make_self<HttpAgents::HTTPHandler>(pSystemThreads->at(4).scheduler, this->postBuffer02, this->postBuffer01);
			this->postRequestSender = make_self<HttpAgents::RequestSender>(pSystemThreads->at(4).scheduler, this->postBuffer00, this->postBuffer01, this->postBuffer02, this->postBuffer03);
			this->deleteHTTPHandler = make_self<HttpAgents::HTTPHandler>(pSystemThreads->at(5).scheduler, this->deleteBuffer02, this->deleteBuffer01);
			this->deleteRequestSender = make_self<HttpAgents::RequestSender>(pSystemThreads->at(5).scheduler, this->deleteBuffer00, this->deleteBuffer01, this->deleteBuffer02, this->deleteBuffer03);
			this->getRequestSender->start();
			this->getHTTPHandler->start();
			this->putHTTPHandler->start();
			this->putRequestSender->start();
			this->postHTTPHandler->start();
			this->postRequestSender->start();
			this->deleteHTTPHandler->start();
			this->deleteRequestSender->start();
		};

		~HTTPController() {};

	protected:
		unbounded_buffer<HttpAgents::WorkloadData> getBuffer01;
		unbounded_buffer<HTTPData> getBuffer02;

		unbounded_buffer<HttpAgents::WorkloadData> putBuffer01;
		unbounded_buffer<HTTPData> putBuffer02;

		unbounded_buffer<HttpAgents::WorkloadData> postBuffer01;
		unbounded_buffer<HTTPData> postBuffer02;

		unbounded_buffer<HttpAgents::WorkloadData> deleteBuffer01;
		unbounded_buffer<HTTPData> deleteBuffer02;
	};
}
#endif
