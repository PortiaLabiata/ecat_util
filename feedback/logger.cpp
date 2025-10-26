#include <chrono>
#include "logger.hpp"

Logger logger; // Singleton
Logger *Logger::singleton;

void Logger::log(Logger::Severity severity, std::string str) {
	messages.emplace_back(
		Logger::LogMessage{
			std::chrono::system_clock::now(),
			severity,
			str
		}
	);
	auto entry = messages.rbegin();
	std::cout << entry->timestamp << '\t'
			<< "[" << Logger::severity_to_str(entry->severity) << "]" << '\t'
			<< entry->message
			<< std::endl;
}

const char *Logger::severity_to_str(Logger::Severity severity) {
	switch (severity) {
		case Logger::Severity::Debug:
			return "Debug";
		case Logger::Severity::Error:
			return "Error";
		case Logger::Severity::Log:
			return "Log";
		case Logger::Severity::Warning:
			return "Warning";
		default:
			return "Invalid severity";
	}
}

namespace EC {
	LogStream dbgstream() { return LogStream(logger, Logger::Severity::Debug); }
	LogStream logstream() { return LogStream(logger, Logger::Severity::Log); }
	LogStream wrnstream() { return LogStream(logger, Logger::Severity::Warning); }
	LogStream errstream() { return LogStream(logger, Logger::Severity::Error); }
}
