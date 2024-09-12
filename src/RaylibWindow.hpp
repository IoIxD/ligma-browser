#ifndef __RAYLIB_WINDOW_HPP
#define __RAYLIB_WINDOW_HPP

#include <stdio.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <utility>
#include "BrowserView.hpp"
#include "CEFGLWindow.hpp"
#include "GLFW/glfw3.h"
#include "JSKeyCodes.hpp"
#include "cef_version.h"
#include "internal/cef_string.h"
#include "internal/cef_types.h"
#include "internal/cef_types_wrappers.h"
#include "raylib.h"
#include "rlgl.h"
#include "rs_image.hpp"
#include "rs_systemtime.hpp"

using namespace rs_image;

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