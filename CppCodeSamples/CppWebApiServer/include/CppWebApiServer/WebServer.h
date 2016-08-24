#pragma once
#include <cpprest/http_listener.h>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace web::http::experimental::listener;
using namespace web::http;
using namespace web;

class WebServer
{
public:
	WebServer();
	~WebServer();

	void StartServer(std::wstring uri);
	void StopServer();

private:
	std::unique_ptr<http_listener> m_listener_ptr;
};

