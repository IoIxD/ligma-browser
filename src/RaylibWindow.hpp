#ifndef __RAYLIB_WINDOW_HPP
#define __RAYLIB_WINDOW_HPP

#include <cstdint>
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include "BrowserView.hpp"
#include "CEFGLWindow.hpp"
#include "raylib.h"
#include "rs_systemtime.hpp"

class TabPosition {
 public:
  float x;
  float y;
  float z;
  TabPosition(float x, float y, float z) : x(x), y(y), z(z) {};
};

class TabInfo {
  std::shared_ptr<BrowserView> view;
  std::optional<Texture2D> texture;
  Model model = {0};

 public:
  TabInfo(std::shared_ptr<BrowserView> view) : view(view) {};
  Model* GetIcon();
  std::string title;

  friend class RaylibWindow;
};

class RaylibWindow {
  SystemTime* time;
  CEFGLWindow* window;
  std::map<std::string, std::shared_ptr<BrowserView>> tabs;
  std::map<std::string, TabInfo> tabinfos = std::map<std::string, TabInfo>();
  std::shared_ptr<BrowserView> current_tab;
  Vector2 v;
  int width = 800;
  int height = 600;

  void renderBrowserWindow();
  void renderTabs();

  float dimension = 0.0;
  int time_since_scroll = 0;
  int leftClickCounter = 0;
  Camera3D camera = {0};

  enum class State {
    Browser,
    Tabs,
  };

  State state;

  std::string hashIndex(TabPosition index) {
    return std::format("{}{}{}", index.x, index.y, index.z);
  }

 public:
  RaylibWindow(int argc, char** argv);
  void keyTranslation();
  void mouseTranslation();
  void resizeTranslation();
  void render();

  std::optional<TabInfo*> tabAt(TabPosition index);
  void insertTab(TabPosition index);
  void setTab(TabPosition index);
  void removeTab(TabPosition index);
  bool hasTab(TabPosition index);
  bool isCurrent(TabPosition index);
  void disableGUI();
  void toggleView();
  void updateButtons();
};

#endif