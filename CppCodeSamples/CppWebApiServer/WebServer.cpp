#include "stdafx.h"
#include <CppWebApiServer/WebServer.h>
#include <cpprest/http_listener.h>
#include <iostream>

using namespace web::http::experimental::listener;
using namespace web::http;
using namespace web;

WebServer::WebServer()
{
}

WebServer::~WebServer()
{
}

void WebServer::StartServer(std::wstring uri)
{
	m_listener_ptr = std::make_unique<http_listener>(uri);
	m_listener_ptr->support(methods::GET, [](http_request request)
	{
		std::wcout << L"WebServer CALLED? " << L" [" << std::this_thread::get_id() << L"] " << request.absolute_uri().to_string() << std::endl;

		utility::string_t requested_uri(request.absolute_uri().to_string());
		std::vector<utility::string_t> paths = uri::split_path(requested_uri);
		std::map<utility::string_t, utility::string_t> queries = uri::split_query(requested_uri);

		web::json::value jo;
		jo[L"access_token"] = json::value::string(L"dummy-access-token-value");

		request.reply(status_codes::OK, jo);
	});

	m_listener_ptr->support(methods::POST, [](http_request request)
	{
		std::wcout << L"WebServer CALLED? " << L" [" << std::this_thread::get_id() << L"] " << request.absolute_uri().to_string() << std::endl;
		request.reply(status_codes::OK, L"TEST PAGE? POST");
	});

	m_listener_ptr->support(methods::PUT, [](http_request request)
	{
		std::wcout << L"WebServer CALLED? " << L" [" << std::this_thread::get_id() << L"] " << request.absolute_uri().to_string() << std::endl;
		request.reply(status_codes::OK, L"TEST PAGE? PUT");
	});

	m_listener_ptr->support(methods::DEL, [](http_request request)
	{
		std::wcout << L"WebServer CALLED? " << L" [" << std::this_thread::get_id() << L"] " << request.absolute_uri().to_string() << std::endl;
		request.reply(status_codes::OK, L"TEST PAGE? DELETE");
	});

	/*m_listener_ptr->support([](http_request request)
	{
		std::wcout << L"WebServer CALLED? " << L" [" << std::this_thread::get_id() << L"] " << request.absolute_uri().to_string() << std::endl;
		request.reply(status_codes::OK, L"TEST PAGE?");
	});*/

	m_listener_ptr->open();
}

void WebServer::StopServer()
{
	m_listener_ptr->close().wait();
}

