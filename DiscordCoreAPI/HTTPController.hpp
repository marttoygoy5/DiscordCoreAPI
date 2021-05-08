// HTTPControler.hpp - Header for the HTTP Controller class.
// May 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HTTP_CONTROLLER_
#define _HTTP_CONTROLLER_

#include "pch.h"
#include "HttpAgents.hpp"
#include "RestAPI.hpp"

namespace CommanderNS {

	struct HTTPController :implements<HTTPController, winrt::Windows::Foundation::IInspectable> {
	public:
		HTTPController() {};
		HTTPController(concurrent_vector<ThreadContext> systemThreads, com_ptr<RestAPI> pRestAPI) {
			this->getHTTPHandler = make_self<HttpAgents::HTTPHandler>(systemThreads.at(2).scheduler, pRestAPI, this->getBuffer02, this->getBuffer01);
			this->getRequestSender = make_self<HttpAgents::RequestSender>(systemThreads.at(2).scheduler, this->getBuffer00, this->getBuffer01, this->getBuffer02, this->getBuffer03);
			this->postHTTPHandler = make_self<HttpAgents::HTTPHandler>(systemThreads.at(3).scheduler, pRestAPI, this->postBuffer02, this->postBuffer01);
			this->postRequestSender = make_self<HttpAgents::RequestSender>(systemThreads.at(3).scheduler, this->postBuffer00, this->postBuffer01, this->postBuffer02, this->postBuffer03);
			this->putHTTPHandler = make_self<HttpAgents::HTTPHandler>(systemThreads.at(4).scheduler, pRestAPI, this->putBuffer02, this->putBuffer01);
			this->putRequestSender = make_self<HttpAgents::RequestSender>(systemThreads.at(4).scheduler, this->putBuffer00, this->putBuffer01, this->putBuffer02, this->putBuffer03);
			this->postHTTPHandler->start();
			this->postRequestSender->start();
			this->putHTTPHandler->start();
			this->putRequestSender->start();
			this->deleteHTTPHandler->start();
			this->deleteRequestSender->start();
		};

		~HTTPController() {};

		unbounded_buffer<HttpAgents::WorkloadData> getBuffer00;
		unbounded_buffer<HttpAgents::HTTPData> getBuffer03;
		unbounded_buffer<HttpAgents::WorkloadData> putBuffer00;
		unbounded_buffer<HttpAgents::HTTPData> putBuffer03;
		unbounded_buffer<HttpAgents::WorkloadData> postBuffer00;
		unbounded_buffer<HttpAgents::HTTPData> postBuffer03;
		unbounded_buffer<HttpAgents::WorkloadData> deleteBuffer00;
		unbounded_buffer<HttpAgents::HTTPData> deleteBuffer03;
	protected:
		unbounded_buffer<HttpAgents::WorkloadData> getBuffer01;
		unbounded_buffer<HttpAgents::HTTPData> getBuffer02;
		com_ptr<HttpAgents::RequestSender> getRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> getHTTPHandler{ nullptr };

		unbounded_buffer<HttpAgents::WorkloadData> postBuffer01;
		unbounded_buffer<HttpAgents::HTTPData> postBuffer02;
		com_ptr<HttpAgents::RequestSender> postRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> postHTTPHandler{ nullptr };

		unbounded_buffer<HttpAgents::WorkloadData> putBuffer01;
		unbounded_buffer<HttpAgents::HTTPData> putBuffer02;
		com_ptr<HttpAgents::RequestSender> putRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> putHTTPHandler{ nullptr };

		unbounded_buffer<HttpAgents::WorkloadData> deleteBuffer01;
		unbounded_buffer<HttpAgents::HTTPData> deleteBuffer02;
		com_ptr<HttpAgents::RequestSender> deleteRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> deleteHTTPHandler{ nullptr };
	};
}
#endif
