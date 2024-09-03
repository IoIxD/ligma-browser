#ifndef GUI_H
#define GUI_H

#include "Renderer.hpp"
#include "cairo.h"
#include "gtk/gtk.h"
#include "raylib.h"
#include <curl/curl.h>
#include <optional>
#include <string>
#include <vector>
#include <webkit2/webkit2.h>

class SiteInfo {
public:
  std::string url;
  std::string title;
  std::optional<Texture2D> favicon;
  unsigned int textureID;

  SiteInfo() {
    url = "";
    title = "";
    favicon = {};
    textureID = 0;
  }
  SiteInfo(std::string url, std::string title, std::optional<Texture2D> favicon,
           unsigned int textureID)
      : url(url), title(title), favicon(favicon), textureID(textureID) {};
};

class Tab {

public:
  WebKitWebView *webview;
  explicit Tab(const std::string &str = "https://google.com");
};

class Window {
  enum class View {
    Web,
    Render,
  };
  View view = View::Web;
  GtkWidget *win = nullptr;
  GtkWidget *tabbar = nullptr;
  GtkWidget *box = nullptr;
  GtkWidget *back = nullptr;
  GtkWidget *forward = nullptr;
  GtkWidget *tabswitch = nullptr;
  GtkWidget *search = nullptr;
  std::vector<Tab *> *tabs;
  Renderer *render = nullptr;
  Tab *current_tab;
  WebKitFaviconDatabase *database;
  cairo_surface_t *favicon;
  CURL *curl;
  CURLU *curlurl;
  CURLcode result;

  std::vector<SiteInfo> *icons;
  bool image_filled = false;

public:
  Window();
  void setup();
  void ToggleView();
  std::vector<SiteInfo> *GetIcons();

  Renderer *renderer() { return this->render; };
  Tab *tab_at(int index) { return this->tabs->at(0); }

  static void main_thread();
};

#endif // GUI_H
