#pragma once
#include <chrono>
#include <list> // We need to append entries fast, but during rendering we will traverse them all anyawy
#include <sstream>
#include <iostream>

struct LogSingletonError {};

class Logger {
public:
	Logger() {
		if (singleton) {
			throw LogSingletonError();
		}
		singleton = this;
	}
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger& other) = delete;

	enum class Severity {
		Debug,
		Log,
		Warning,
		Error,
	};

	struct LogMessage {
		std::chrono::time_point<std::chrono::system_clock> timestamp;
		Severity severity;
		std::string message;
	};

	static Logger *get_singleton() { return singleton; }
	static const char *severity_to_str(Severity severity);

	void log(Severity severity, std::string msg);
	const std::list<LogMessage>& get_messages() const { return messages; }

private:
	std::list<LogMessage> messages;
	static Logger *singleton;
};

class LogStream {
public:
	LogStream(Logger& _logger, Logger::Severity _severity) : severity(_severity), logger(_logger) {}	
	~LogStream() {
		logger.log(severity, stream.str());
	}

	template <typename T>
	LogStream& operator<<(const T& v) {
		stream << v;
		return *this;
	}

private:
	const Logger::Severity severity;
	Logger& logger;
	std::stringstream stream;
};

namespace Log {
	LogStream dbgstream();
	LogStream logstream();
	LogStream wrnstream();
	LogStream errstream();
}
