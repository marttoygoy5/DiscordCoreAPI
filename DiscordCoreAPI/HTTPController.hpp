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

	struct HTTPController:implements<HTTPController, winrt::Windows::Foundation::IInspectable> {
	public:
		HTTPController() {};
		HTTPController(concurrent_vector<ThreadContext> systemThreads, com_ptr<RestAPI> pRestAPI){
			this->postHTTPHandler = make_self<HttpAgents::HTTPHandler>(systemThreads.at(3).scheduler, pRestAPI, this->postBuffer02, this->postBuffer01);
			this->postRequestSender = make_self<HttpAgents::RequestSender>(systemThreads.at(3).scheduler, this->postBuffer00, this->postBuffer01, this->postBuffer02, this->postBuffer03);
			this->postHTTPHandler->start();
			this->postRequestSender->start();
				
		};

		~HTTPController() {};
		unbounded_buffer<HttpAgents::WorkloadData> postBuffer00;
		unbounded_buffer<HttpAgents::HTTPData> postBuffer03;
	protected:
		unbounded_buffer<HttpAgents::WorkloadData> postBuffer01;
		unbounded_buffer<HttpAgents::HTTPData> postBuffer02;
		com_ptr<HttpAgents::RequestSender> postRequestSender{ nullptr };
		com_ptr<HttpAgents::HTTPHandler> postHTTPHandler{ nullptr };
		


	};


}
#endif
