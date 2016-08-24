// CppCodeSamples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TcpServerSample.hpp"
#include <CppWebApiServer/WebServer.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

std::mutex mux;
std::condition_variable cv;

int main()
{
	std::thread sample_executer([]()
	{
		std::wstring listen_uri(L"http://localhost:4881/test");

		std::unique_lock<std::mutex> local_lock(mux);

		WebServer web_server;
		web_server.StartServer(listen_uri);

		std::unique_ptr<TcpServerSample> tcp_server_ptr(new TcpServerSample);
		
		std::wcout << L"Server started (" << listen_uri << L")" << std::endl;
		cv.wait(local_lock);
		
		std::wcout << L"Server stopping..." << std::endl;
		web_server.StopServer();
		tcp_server_ptr.reset();
	});

	std::getc(stdin);
	cv.notify_one();

	sample_executer.join();
	std::wcout << L"Server stopped" << std::endl;
	std::getc(stdin);

	return 0;
}

