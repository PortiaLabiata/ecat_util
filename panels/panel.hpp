#pragma once
#include <list>
#include <imgui.h>
#include <ecat/core.hpp>

#define PANEL_CONSTRUCTOR(__CLASSNAME__) __CLASSNAME__(ImVec2 size, const char *title) : Panel(size, title) {}

template <typename ...Tags>
struct ExtensibleEnum {};

class Panel {
public:
	Panel(ImVec2 _size, const char *_title) : title(_title), size(_size) {}
	virtual ~Panel() {}; 

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
private:	
	void render_this() override {
		ImGui::Button("Click me!");
	}
};

template <typename T>
class View {
public:
	virtual ~View() {}

	void set_object(T *_obj) { obj = _obj; }
protected:
	T *obj = nullptr;
};

class SlaveView : public View<ECATSlave>{
public:
	~SlaveView() override {}
	virtual void render() = 0;
};

class MasterView : public View<ECATMaster> {
public:
	~MasterView() override {}
	virtual void render() = 0;

	void attach_view(SlaveView *view) { attached_views.push_back(view); }
	void update_views(ECATSlave *slave) {
		for (const auto &view : attached_views) {
			view->set_object(slave);
		}
	}

protected:
	std::list<SlaveView*> attached_views;
};

