#pragma once
#include <list>
#include <imgui.h>
#include <ecat/core.hpp>

#define PANEL_CONSTRUCTOR(__CLASSNAME__) __CLASSNAME__(ImVec2 size, const char *title) : Panel(size, title) {}

class Panel {
public:
	Panel(ImVec2 _size, const char *_title) : title(_title), size(_size) {}

	virtual ~Panel() = default;	

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
using PanelPtr = std::unique_ptr<Panel>;

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
	View(std::shared_ptr<T> _obj) : obj(_obj) {}
	View() = default;
	virtual ~View() {}

	void set_object(std::shared_ptr<T> _obj) { obj = _obj; }
protected:
	std::weak_ptr<T> obj;
};

template <typename T>
class ViewRaw {
public:
	ViewRaw(T *_obj) : obj(_obj) {}
	ViewRaw() = default;
	virtual ~ViewRaw() {}
	
	void set_object(T *_obj) { obj = _obj; }
protected:
	T *obj;
};

// Do not create with default constructor,
// use factory function MasterView::construct_slave_view instead!
class SlaveView : public View<ECATSlave>{
public:
	~SlaveView() override {}
	virtual void render() = 0;
};
using SlaveViewPtr = std::unique_ptr<SlaveView>;


class MasterView : public View<ECATMaster> {
public:
	MasterView() = default;
	MasterView(std::shared_ptr<ECATMaster> master) : View<ECATMaster>(master) {}

	~MasterView() override {
		for (auto& view : attached_views) {
			// FIXME: add lock check?
			view.reset();
		}
	}
	virtual void render() = 0;

	// Factory method
	template <typename T>
	void construct_slave_view() {
		std::unique_ptr<T> view = std::make_unique<T>();
		attached_views.push_back(std::move(view));
	}

	void update_views(ECATSlavePtr slave) {
		for (const auto &view : attached_views) {
			view->set_object(slave);
		}
	}

protected:
	std::list<SlaveViewPtr> attached_views;
};
using MasterViewPtr = std::unique_ptr<MasterView>;

