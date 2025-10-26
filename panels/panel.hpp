#pragma once
#include <list>
#include <imgui.h>
#include <ecat/core.hpp>

#define PANEL_CONSTRUCTOR(__CLASSNAME__) __CLASSNAME__(ImVec2 size, const char *title) : Panel(size, title) {}

class Panel {
public:
	Panel(ImVec2 _size, const char *_title) : title(_title), size(_size) {
		instances.push_back(this);
	}

	virtual ~Panel() {
		for (auto ptr = instances.begin(); ptr < instances.end(); ptr++) {
			if (this == *ptr) {
				instances.erase(ptr);	
			}
		}
	}; 

	static void delete_all() {
		for (auto panel : instances) {
			delete panel;
		}
	}

	void render() {
		ImGui::Begin(title);
		render_this();
		ImGui::End();
	}

private:
	virtual void render_this() = 0;
	const char *title;
	const ImVec2 size;
	static std::vector<Panel*> instances;
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

// Do not create with default constructor,
// use factory function MasterView::construct_slave_view instead!
class SlaveView : public View<ECATSlave>{
public:
	~SlaveView() override {}
	virtual void render() = 0;
};


class MasterView : public View<ECATMaster> {
public:
	MasterView() = default;
	MasterView(ECATMaster *master) : View<ECATMaster>() {
		set_object(master);
	}

	~MasterView() override {
		for (auto view : attached_views) {
			delete view;
		}
	}
	virtual void render() = 0;

	// Factory method
	template <typename T>
	T *construct_slave_view() {
		T *view = new T();
		attach_view(view);
		return view;
	}

	void attach_view(SlaveView *view) { attached_views.push_back(view); }
	void update_views(ECATSlave *slave) {
		for (const auto &view : attached_views) {
			view->set_object(slave);
		}
	}

protected:
	// FIXME: If SlaveView is deleted, updating it will cause segfault!
	// Add method like ptr_lookup() and remove list entry from SlaveView's
	// destructor
	std::list<SlaveView*> attached_views;
};

