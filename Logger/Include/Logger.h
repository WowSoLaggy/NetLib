// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Module:		Logger
// Author:		Anton Egorov
// Description:	Class that logs any text to the console and the given file
//
// Usage example:
//
// void SomeFunc()
// {
//		LOG("SomeFunc()");
//		...
//		echo("Hello, world!");
//		...
// }
//
// void main()
// {
//		LOGINIT("Log.log", "MyProduct", "ThisFile.exe");
//		...
//		SomeFunc();
//		...
//		LOGDISPOSE();
// }
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef LOG_H
#define LOG_H


// Initializes logger. Should be called first of all. Writes the welcome message to the log
// Params:
// [in] std::string pLogFileName	- log file name (ex. "Logs\myLog.log"). Warning: Directory should exist
// [in] std::string pProductName	- product name used in the welcome message
// [in] std::string pFilePath		- name of the file to take version from. Can be empty
#define LOGINIT(pLogFileName, pProductName, pFilePath) Log::Logger::Init(pLogFileName, pProductName, pFilePath)

// Disposes logger. Writes the bye message to the log
#define LOGDISPOSE Log::Logger::Dispose()

// Define that should be called before any echo-calls. Initializes the log object for the current function
// Params:
// [in] std::string pPrefix	- function name
#define LOG(pPrefix) Log::Logger log(pPrefix)

// Logs the given message
//Params:
// [in] std::string pText - text to log
#define echo(pText) log.Echo(pText)


namespace Log
{

	// Class that logs any text to the console and the given file
	class Logger
	{
	public:

		// Logger constructor that should be called before any echo-calls. Initializes the log object for the current function
		// Params:
		// [in] std::string pPrefix	- function name
		Logger(std::string pPrefix);

		// Logger destructor
		virtual ~Logger();

		// Logs the given message
		//Params:
		// [in] std::string pText - text to log
		void Echo(std::string pText);

		// Initializes logger. Should be called first of all. Writes the welcome message to the log
		// Params:
		// [in] std::string pLogFileName	- log file name (ex. "Logs\myLog.log"). Warning: Directory should exist
		// [in] std::string pProductName	- product name used in the welcome message
		// [in] std::string pFilePath		- name of the file to take version from. Can be empty
		static void Init(std::string pLogFileName, std::string pProductName, std::string pFilePath);

		// Disposes logger. Writes the bye message to the log
		static void Dispose();

	private:

		std::string m_prefix;
		std::fstream m_fOut;

		static std::string m_productName;
		static std::string m_logFileName;
	};

} // Log


#endif // LOG_H
