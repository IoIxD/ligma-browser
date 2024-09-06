#include <memory>
#include <optional>
#include <string>
#include "BrowserView.hpp"
#include "CEFGLWindow.hpp"
#include "raylib.h"

class TabPosition {
 public:
  float x;
  float y;
  float z;
  TabPosition(float x, float y, float z) : x(x), y(y), z(z) {};
};

class TabInfo {
  BrowserView* view;

 public:
  TabInfo(BrowserView* view) : view(view) {};
  std::optional<Texture2D> GetIcon();
  std::string title;
};

class Window {
  CEFGLWindow* window;
  std::shared_ptr<BrowserView> current_tab;
  Vector2 v;
  int width = 800;
  int height = 600;

  void renderBrowserWindow();
  void renderTabs();

  float dimension;
  int time_since_scroll;
  Camera3D* camera;

  enum class State {
    Browser,
    Tabs,
  };

  State state;

 public:
  Window(int argc, char** argv);
  void keyTranslation();
  void mouseTranslation();
  void resizeTranslation();
  void render();

  bool hasTab(TabPosition index);
  std::optional<TabInfo> tabAt(TabPosition index);
  void insertTab(TabPosition index);
  void setTab(TabPosition index);
  void removeTab(TabPosition index);
  bool isCurrent(TabPosition index);
  void disableGUI();
  void toggleView();
  void updateButtons();
};
