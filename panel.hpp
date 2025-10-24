#pragma once
#include <imgui.h>
#include "vector.hpp"

#define PANEL_CONSTRUCTOR(__CLASSNAME__) __CLASSNAME__(Vector2i dims, const char *title) : Panel(dims, title) {}

class Panel {
public:
	Panel(Vector2i dims, const char *_title) : title(_title) {
		size = {
			static_cast<float>(dims.x),
			static_cast<float>(dims.y)
		};
	}

	~Panel() = default;

	void render() {
		if (deactivated) {
			return;
		}
		ImGui::Begin(title);
		render_this();
		ImGui::End();
	}

	void deactivate() { deactivated = true; }
	void reactivate() {deactivated = false; }

private:
	virtual void render_this() = 0;
	const char *title;
	ImVec2 size;
	bool deactivated = false;
};

class PanelEmpty : public Panel {
public:
	PanelEmpty(Vector2i dims, const char *title) : Panel(dims, title) {};
private:	
	void render_this() override {
		ImGui::Button("Click me!");
	}
};
