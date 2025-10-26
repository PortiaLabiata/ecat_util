#pragma once
#include <feedback/logger.hpp>
#include "panel.hpp"

#define COLOR_STATUS ImVec4{0, 1, 0, 1}
#define COLOR_DEBUG ImVec4{1, 1, 1, 1}
#define COLOR_ERROR ImVec4{1, 0, 0, 1}

class LogView : public View<Logger> {
public:
	LogView() { set_object(Logger::get_singleton()); }
	virtual void render() = 0;
};

class ConsoleLogView : public LogView {
public:
	void render() override;
private:
	ImVec4 get_color(Logger::Severity severity) {
		switch (severity) {
			case Logger::Severity::Debug:
			case Logger::Severity::Log:
				return COLOR_DEBUG;
			case Logger::Severity::Warning:
				return COLOR_STATUS;
			case Logger::Severity::Error:
				return COLOR_ERROR;
			default:
				return ImVec4{0, 0, 0, 1};
		}
	}
};

class PanelConsole : public Panel {
public:
	PanelConsole(ImVec2 size) : Panel(size, "Console") {}
	void render_this() override {
		log_view.render();	
	}
private:
	ConsoleLogView log_view;
};
