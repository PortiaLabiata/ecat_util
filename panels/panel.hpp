#pragma once
#include <imgui.h>

#define PANEL_CONSTRUCTOR(__CLASSNAME__) __CLASSNAME__(ImVec2 size, const char *title) : Panel(size, title) {}

template <typename ...Tags>
struct ExtensibleEnum {};

class Panel {
public:
	Panel(ImVec2 _size, const char *_title) : title(_title), size(_size) {}
	virtual ~Panel() {}

	void render() {
		ImGui::Begin(title);
		render_this();
		ImGui::End();
	}

private:
	virtual void render_this() = 0;
	const char *title;
	const ImVec2 size;
};

class PanelEmpty : public Panel {
public:
	PANEL_CONSTRUCTOR(PanelEmpty);
	~PanelEmpty() override {}
private:	
	void render_this() override {
		ImGui::Button("Click me!");
	}
};
