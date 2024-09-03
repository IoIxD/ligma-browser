#include "GUI.h"
#include "Instance.hpp"
#include "Renderer.hpp"
#include "cairo.h"
#include "glib-object.h"
#include "gtk/gtk.h"
#include "raylib.h"
#include "webkit/WebKitSettings.h"
#include <GL/gl.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/urlapi.h>

void back_btn(GtkWidget *btn, GdkEventButton *event, gpointer userdata) {
  Window *inst = GetInstance();
  webkit_web_view_go_back(inst->current_tab()->webview);
};
void forward_btn(GtkWidget *btn, GdkEventButton *event, gpointer userdata) {
  Window *inst = GetInstance();
  webkit_web_view_go_forward(inst->current_tab()->webview);
};
void tab_btn(GtkWidget *btn, GdkEventButton *event, gpointer userdata) {
  Window *inst = (Window *)userdata;
  inst->ToggleView();
}
void search_bar(GtkWidget *btn, GdkEventButton *event, gpointer userdata) {
  Window *inst = GetInstance();
  auto url = std::string(gtk_entry_get_text(GTK_ENTRY(btn)));
  if (!url.starts_with("http://") || !url.starts_with("https://")) {
    url = "https://" + url;
  }
  webkit_web_view_load_uri(inst->current_tab()->webview, url.c_str());
}

Window::Window() {
  gtk_init(nullptr, nullptr);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(win), 800, 600);
  gtk_window_set_title(GTK_WINDOW(win), "");
  g_signal_connect(win, "destroy", gtk_main_quit, NULL);

  tabs = new std::map<uint64_t, Tab *>();
  insert_tab(TabPosition(0, 0, 0));
  set_tab(TabPosition(0, 0, 0));

  render = new Renderer();

  curl = curl_easy_init();
  curlurl = curl_url();

  icons = new std::map<uint64_t, std::optional<SiteInfo>>();

  tabbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  back = gtk_button_new_with_label("<");
  gtk_widget_set_size_request(back, 32, 32);
  g_signal_connect(back, "clicked", G_CALLBACK(back_btn), this);
  gtk_box_pack_start(GTK_BOX(tabbar), GTK_WIDGET(back), FALSE, FALSE, 0);
  forward = gtk_button_new_with_label(">");
  gtk_widget_set_size_request(forward, 32, 32);
  g_signal_connect(forward, "clicked", G_CALLBACK(forward_btn), this);
  gtk_box_pack_start(GTK_BOX(tabbar), GTK_WIDGET(forward), FALSE, FALSE, 0);
  tabswitch = gtk_button_new_with_label("||");
  gtk_widget_set_size_request(tabswitch, 32, 32);
  g_signal_connect(tabswitch, "clicked", G_CALLBACK(tab_btn), this);
  gtk_box_pack_start(GTK_BOX(tabbar), GTK_WIDGET(tabswitch), FALSE, FALSE, 0);

  search = gtk_entry_new();
  g_signal_connect(search, "activate", G_CALLBACK(search_bar), this);
  gtk_widget_set_size_request(search, 32, 32);
  gtk_box_pack_start(GTK_BOX(tabbar), GTK_WIDGET(search), TRUE, TRUE, 0);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(render->embed), FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(current_tab()->webview), TRUE,
                     TRUE, 0);

  gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(render->embed), FALSE,
                            FALSE, 0, GTK_PACK_START);

  gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(tabbar), FALSE, FALSE, 0);

  gtk_widget_hide(GTK_WIDGET(render->embed));

  gtk_container_add(GTK_CONTAINER(win), box);

  gtk_widget_show_all(win);

  SetInstance(this);
}

void Window::setup() {}

void Window::main_thread() { gtk_main(); }

void Window::ToggleView() {
  int webwidth, webheight, renwidth, renheight;
  gtk_widget_get_size_request(GTK_WIDGET(current_tab()->webview), &webwidth,
                              &webheight);
  gtk_widget_get_size_request(GTK_WIDGET(render->embed), &renwidth, &renheight);

  if (this->view == View::Web) {
    gtk_widget_show(GTK_WIDGET(render->embed));
    gtk_widget_hide(GTK_WIDGET(current_tab()->webview));
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(render->embed), TRUE,
                              TRUE, 0, GTK_PACK_START);
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(current_tab()->webview),
                              FALSE, FALSE, 0, GTK_PACK_START);

    this->view = View::Render;
  } else {
    gtk_widget_hide(GTK_WIDGET(render->embed));
    gtk_widget_show(GTK_WIDGET(current_tab()->webview));
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(render->embed), FALSE,
                              FALSE, 0, GTK_PACK_START);
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(current_tab()->webview),
                              TRUE, TRUE, 0, GTK_PACK_START);

    this->view = View::Web;
  }
}

void load_changed(WebKitWebView *self, WebKitLoadEvent load_event,
                  gpointer user_data) {
  Tab *tab = (Tab *)user_data;
  tab->DestroyIcon();
  tab->UpdateIcon();

  GetInstance()->set_text(
      webkit_web_view_get_uri(GetInstance()->current_tab()->webview));
}
Tab::Tab(const std::string &str) {
  context = webkit_web_context_new();

  const auto config = webkit_settings_new();
  webkit_settings_set_enable_webgl(config, true);
  webkit_settings_set_enable_developer_extras(config, true);
  webkit_settings_set_enable_write_console_messages_to_stdout(config, true);
  webkit_web_context_set_favicon_database_directory(context, "./database");

  webview = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(context));
  g_signal_connect(webview, "load-changed", G_CALLBACK(load_changed), this);
  g_signal_connect(webview, "load-finished", G_CALLBACK(load_changed), this);

  const char *load_url = str.c_str();
  webkit_web_view_load_uri(webview, load_url);

  if (GetInstance() != NULL) {
    if (GetInstance()->box != NULL) {
      printf("addig\n");
      gtk_box_pack_start(GTK_BOX(GetInstance()->box), GTK_WIDGET(webview), TRUE,
                         TRUE, 0);
      gtk_widget_set_size_request(GTK_WIDGET(webview), 800, 1);
    }
  }
}

std::map<uint64_t, std::optional<SiteInfo>> *Window::GetIcons() {
  if (this->image_filled) {
    for (int i = 0; i < this->icons->size(); i++) {
      for (auto tab = this->icons->begin(); tab != this->icons->end(); tab++) {
        if (tab->second.has_value()) {
          if (tab->second->favicon.has_value()) {
            UnloadTexture(tab->second->favicon.value());
          }
        }
      }
    }
    this->icons->erase(this->icons->begin(), this->icons->end());
  }
  // otherwise, populate the icon array!
  auto tabCount = this->tabs->size();
  for (auto tab = this->tabs->begin(); tab != this->tabs->end(); tab++) {
    this->icons->insert(
        std::pair(*(uint64_t *)&tab->first, tab->second->GetIcon()));
    auto tab_icon = webkit_web_view_get_favicon(tab->second->webview);
  }
  this->image_filled = this->icons->size() == this->tabs->size();
  return this->icons;
}

void Tab::UpdateIcon() {
  auto tab_icon = webkit_web_view_get_favicon(this->webview);
  if (tab_icon != nullptr) {
    auto image = gtk_image_new_from_surface(tab_icon);

    int tex_w = cairo_image_surface_get_width(tab_icon);
    int tex_h = cairo_image_surface_get_height(tab_icon);
    unsigned char *data = cairo_image_surface_get_data(tab_icon);

    unsigned int texid;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glRotatef(90, 0, 1, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    this->texture = (Texture){
        .id = texid,
        .width = tex_w,
        .height = tex_h,
        .mipmaps = 1,
    };
  }
}
std::optional<SiteInfo> Tab::GetIcon() {
  auto tab_icon = webkit_web_view_get_favicon(this->webview);
  if (tab_icon != nullptr) {
    if (!this->texture.has_value()) {
      return {};
    } else {
      return SiteInfo(std::string(webkit_web_view_get_uri(this->webview)),
                      std::string(webkit_web_view_get_title(this->webview)),
                      this->texture.value());
    }

  } else {
    return {};
  }
}

void Tab::DestroyIcon() {
  if (this->texture.has_value()) {
    UnloadTexture(this->texture.value());
  }
}