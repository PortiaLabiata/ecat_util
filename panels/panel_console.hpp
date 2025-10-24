#pragma once
#include "panel.hpp"
#include <cstdarg>
#include <cstdio>
#include <vector>
#include <string>
#include <stdarg.h>

#define COLOR_STATUS ImVec4{0, 1, 0, 1}
#define COLOR_DEBUG ImVec4{1, 1, 1, 1}
#define COLOR_ERROR ImVec4{1, 0, 0, 1}

struct SingletonError {};

class PanelConsole : public Panel {
public:
	PanelConsole(ImVec2 size) : Panel(size, "Console") {
		if (singleton) {
			throw SingletonError();
		}
		singleton = this;
	}

	~PanelConsole() override {}

	static PanelConsole *get_singleton() { return singleton; }

	enum class LogLevel {
		debug = 0,
		status,
		error
	};

	void log(LogLevel level, const char *fmt, ...) {
		std::va_list args;
		va_start(args, fmt);
		char text[BUFFER_SIZE];
		std::snprintf(text, BUFFER_SIZE, fmt, args);
		contents.emplace_back(ConsoleEntry(level, text));
		va_end(args);
	}

private:
	struct ConsoleEntry {
		ConsoleEntry(LogLevel _level, const char *_text) :
			level(_level), text(std::string(_text)) {};

		const LogLevel level;
		std::string text;

		ImVec4 get_color() {
			switch (level) {
				case LogLevel::debug:
					return COLOR_DEBUG;
				case LogLevel::status:
					return COLOR_STATUS;
				case LogLevel::error:
					return COLOR_ERROR;
				default:
					return COLOR_STATUS;
			}
		}
	};

	std::vector<ConsoleEntry> contents;
	static const size_t BUFFER_SIZE = 256;
	static inline PanelConsole *singleton = nullptr;
	void render_this() override {
		if (ImGui::Button("Clear")) {
			contents.clear();
		}
		for (auto entry : contents) {
			ImGui::TextColored(entry.get_color(), "%s", entry.text.c_str());
		}
	}
};
