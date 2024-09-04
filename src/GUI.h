#ifndef GUI_H
#define GUI_H

#include "Renderer.hpp"
#include "cairo.h"
#include "gtk/gtk.h"
#include "raylib.h"
#include <curl/curl.h>
#include <format>
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
  GtkWidget *refresh = nullptr;

  GtkWidget *back_img;
  GtkWidget *forward_img;
  GtkWidget *tabs_img;
  GtkWidget *refresh_img;

  WebKitFaviconDatabase *database;
  cairo_surface_t *favicon;
  CURL *curl;
  CURLU *curlurl;
  CURLcode result;
  bool gui_enabled;

  std::string current_index;
  Tab *cur_tab;
  std::map<std::string, Tab *> *tabs;
  std::map<std::string, std::optional<SiteInfo>> *icons;

  bool image_filled = false;

public:
  bool renderer_active =
      false; // don't ask why this can't go in Renderer, I don't know either.
  GtkWidget *box = nullptr;
  Window();
  void setup();
  void toggle_view();
  void update_buttons();
  std::map<std::string, std::optional<SiteInfo>> *GetIcons();

  Renderer *renderer() { return this->render; };
  std::optional<Tab *> tab_at(TabPosition index) {
    if (has_tab(index)) {
      return this->tabs->at(hash_index(index));
    } else {
      return {};
    };
  };
  Tab *current_tab() { return this->tabs->at(current_index); };

  std::string hash_index(TabPosition index) {
    return std::format("{}{}{}", index.x, index.y, index.z);
    ;
  }

  bool is_current(TabPosition index) {
    return current_index == hash_index(index);
  };
  bool has_tab(TabPosition index) {
    return this->tabs->contains(hash_index(index));
  };
  void remove_tab(TabPosition index) { this->tabs->erase(hash_index(index)); }
  void set_tab(TabPosition index) {
    if (this->tabs->contains(current_index)) {
      gtk_widget_hide(GTK_WIDGET(this->current_tab()->webview));
    }
    auto tab = this->tabs->at(hash_index(index));
    if (tab != NULL) {
      current_index = hash_index(index);
      gtk_widget_hide(GTK_WIDGET(this->current_tab()->webview));
    }
  }
  void insert_tab(TabPosition index) {
    auto tab = new Tab();
    this->tabs->insert(std::pair(hash_index(index), tab));
  }

  void set_text(std::string str) {
    gtk_entry_set_text(GTK_ENTRY(this->search), str.c_str());
  }

  void disable_gui() {
    gui_enabled = false;
    gtk_widget_set_sensitive(back, FALSE);
    gtk_widget_set_sensitive(forward, FALSE);
    gtk_widget_set_sensitive(search, FALSE);
    gtk_widget_set_sensitive(refresh, FALSE);
  }

  void enable_gui() {
    gui_enabled = true;
    if (webkit_web_view_can_go_back(this->current_tab()->webview)) {
      gtk_widget_set_sensitive(back, TRUE);
    }
    if (webkit_web_view_can_go_forward(this->current_tab()->webview)) {
      gtk_widget_set_sensitive(forward, TRUE);
    }
    gtk_widget_set_sensitive(search, TRUE);
    gtk_widget_set_sensitive(refresh, TRUE);
  }

  void set_btn_status(bool back_st, bool forward_st) {
    gtk_widget_set_sensitive(back, back_st);
    gtk_widget_set_sensitive(forward, forward_st);
  }

  static void main_thread();
};

#endif // GUI_H
