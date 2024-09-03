#ifndef GUI_H
#define GUI_H

#include "Renderer.hpp"
#include "cairo.h"
#include "gtk/gtk.h"
#include "raylib.h"
#include <curl/curl.h>
#include <map>
#include <optional>
#include <string>
#include <webkit2/webkit2.h>

class SiteInfo {
public:
  std::string url;
  std::string title;
  std::optional<Texture2D> favicon;

  SiteInfo() {
    url = "";
    title = "";
    favicon = {};
  }
  SiteInfo(std::string url, std::string title, std::optional<Texture2D> favicon)
      : url(url), title(title), favicon(favicon) {};
};

class Tab {
  WebKitWebContext *context;
  std::optional<Texture2D> texture;

public:
  WebKitWebView *webview;
  explicit Tab(const std::string &str = "https://google.com");
  std::optional<SiteInfo> GetIcon();
  void DestroyIcon();
  void UpdateIcon();
};

class TabPosition {
public:
  float x;
  float y;
  float z; // z being 4d, not 3d.
  TabPosition(float x, float y, float z) : x(x), y(y), z(z) {};
};

class Window {
  enum class View {
    Web,
    Render,
  };
  View view = View::Web;
  GtkWidget *win = nullptr;
  GtkWidget *tabbar = nullptr;
  GtkWidget *back = nullptr;
  GtkWidget *forward = nullptr;
  GtkWidget *tabswitch = nullptr;
  GtkWidget *search = nullptr;
  Renderer *render = nullptr;
  WebKitFaviconDatabase *database;
  cairo_surface_t *favicon;
  CURL *curl;
  CURLU *curlurl;
  CURLcode result;

  uint64_t current_index;
  Tab *cur_tab;
  std::map<uint64_t, Tab *> *tabs;
  std::map<uint64_t, std::optional<SiteInfo>> *icons;

  bool image_filled = false;

public:
  GtkWidget *box = nullptr;
  Window();
  void setup();
  void ToggleView();
  std::map<uint64_t, std::optional<SiteInfo>> *GetIcons();

  Renderer *renderer() { return this->render; };
  std::optional<Tab *> tab_at(TabPosition index) {
    if (has_tab(index)) {
      return this->tabs->at(*(uint64_t *)&index);
    } else {
      return {};
    };
  };
  Tab *current_tab() { return this->tabs->at(current_index); };

  bool has_tab(TabPosition index) {
    return this->tabs->contains(*(uint64_t *)&index);
  };
  void set_tab(TabPosition index) {
    if (this->tabs->contains(current_index)) {
      gtk_widget_hide(GTK_WIDGET(this->current_tab()->webview));
    }
    auto tab = this->tabs->at(*(uint64_t *)&index);
    if (tab != NULL) {
      current_index = *(uint64_t *)&index;
      gtk_widget_hide(GTK_WIDGET(this->current_tab()->webview));
    }
  }
  void insert_tab(TabPosition index) {
    auto tab = new Tab();
    this->tabs->insert(std::pair(*(uint64_t *)&index, tab));
  }

  static void main_thread();
};

#endif // GUI_H
