#include "GUI.h"
#include "Instance.hpp"
#include "Renderer.hpp"
#include "external/stb_image.h"
#include "glib-object.h"
#include "gtk/gtk.h"
#include "raylib.h"
#include "webkit/WebKitSettings.h"
#include <cstdio>
#include <curl/curl.h>
#include <curl/urlapi.h>
#include <optional>
#include <vector>

void back_btn() {
  printf("web browser goes back\n");
  webkit_web_view_go_back(GetInstance()->tab_at(0)->webview);
};
void forward_btn() {
  printf("web browser goes forward\n");
  webkit_web_view_go_forward(GetInstance()->tab_at(0)->webview);
};
void tab_btn() { GetInstance()->ToggleView(); }

Window::Window() {
  gtk_init(nullptr, nullptr);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(win), 800, 600);
  gtk_window_set_title(GTK_WINDOW(win), "");
  g_signal_connect(win, "destroy", gtk_main_quit, NULL);

  tabs = new std::vector<Tab *>();
  current_tab = new Tab();
  tabs->push_back(current_tab);

  render = new Renderer();

  curl = curl_easy_init();
  curlurl = curl_url();

  setup();
  SetInstance(this);
}

void Window::setup() {
  tabbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  back = gtk_button_new_with_label("<");
  g_signal_connect(back, "clicked", back_btn, NULL);
  gtk_box_pack_start(GTK_BOX(tabbar), GTK_WIDGET(back), FALSE, FALSE, 0);
  forward = gtk_button_new_with_label(">");
  g_signal_connect(forward, "clicked", forward_btn, NULL);
  gtk_box_pack_start(GTK_BOX(tabbar), GTK_WIDGET(forward), FALSE, FALSE, 0);
  tabswitch = gtk_button_new_with_label("||");
  g_signal_connect(tabswitch, "clicked", tab_btn, NULL);
  gtk_box_pack_start(GTK_BOX(tabbar), GTK_WIDGET(tabswitch), FALSE, FALSE, 0);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(render->embed), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(current_tab->webview), TRUE, TRUE,
                     0);

  gtk_widget_set_size_request(GTK_WIDGET(render->embed), 800, 1);
  gtk_widget_set_size_request(GTK_WIDGET(current_tab->webview), 800, 600);
  gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(render->embed), FALSE,
                            FALSE, 0, GTK_PACK_START);

  gtk_box_pack_end(GTK_BOX(box), GTK_WIDGET(tabbar), FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(win), box);

  gtk_widget_show_all(win);
}

void Window::main_thread() { gtk_main(); }

void Window::ToggleView() {
  int webwidth, webheight, renwidth, renheight;
  gtk_widget_get_size_request(GTK_WIDGET(current_tab->webview), &webwidth,
                              &webheight);
  gtk_widget_get_size_request(GTK_WIDGET(render->embed), &renwidth, &renheight);

  if (this->view == View::Web) {
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(render->embed), TRUE,
                              TRUE, 0, GTK_PACK_START);
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(current_tab->webview),
                              FALSE, FALSE, 0, GTK_PACK_START);

    gtk_widget_set_size_request(GTK_WIDGET(render->embed), 800, 600);
    gtk_widget_set_size_request(GTK_WIDGET(current_tab->webview), 800, 1);

    this->view = View::Render;
  } else {
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(current_tab->webview),
                              TRUE, TRUE, 0, GTK_PACK_START);
    gtk_box_set_child_packing(GTK_BOX(box), GTK_WIDGET(render->embed), FALSE,
                              FALSE, 0, GTK_PACK_START);

    gtk_widget_set_size_request(GTK_WIDGET(render->embed), 800, 1);
    gtk_widget_set_size_request(GTK_WIDGET(current_tab->webview), 800, 600);

    this->view = View::Web;
  }
}

void what(WebKitFaviconDatabase *self, gchar *page_uri, gchar *favicon_uri,
          gpointer user_data) {
  Tab *tab = (Tab *)user_data;
}
Tab::Tab(const std::string &str) {
  const auto config = webkit_settings_new();
  webkit_settings_set_enable_webgl(config, true);
  webkit_settings_set_enable_developer_extras(config, true);
  webkit_settings_set_enable_write_console_messages_to_stdout(config, true);
  webview = WEBKIT_WEB_VIEW(webkit_web_view_new_with_settings(config));
  g_signal_connect(webview, "notify::favicon", G_CALLBACK(what), this);

  const char *load_url = str.c_str();
  webkit_web_view_load_uri(webview, load_url);
}

size_t write_data(void *contents, size_t size, size_t nmemb, std::string *s) {
  size_t newLength = size * nmemb;
  try {
    s->append((char *)contents, newLength);
  } catch (std::bad_alloc &e) {
    // handle memory problem
    return 0;
  }
  return newLength;
}
bool Window::GetIcons(std::vector<SiteInfo> *icons) {
  auto tabCount = this->tabs->size();
  for (int i = 0; i < tabCount; i++) {
    auto tab = this->tabs->at(i);
    auto tab_icon = webkit_web_view_get_favicon(tab->webview);
    if (tab_icon == NULL) {
      // webkit_web_view_get_favicon seems to return null well after the favicon
      // should be loaded. My options for getting help are going to a mailing
      // list, ran by the GNOME devs, where they've taken more then a day to
      // approve me to post. Or I could work around this!

      auto url = std::string(webkit_web_view_get_uri(tab->webview));
      url += "favicon.ico";

      std::string buffer;

      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

      result = curl_easy_perform(curl);

      if (result != CURLE_OK) {
        printf("CURL Error getting favicon: %s\n", curl_easy_strerror(result));
      }

      int *size;
      if (buffer.data() != NULL) {
        Image image;
        // auto loaded = LoadICO(buffer.data(), buffer.size(), &image);
        // printf("Loaded? %d\n", loaded);
        // if (loaded) {
        icons->push_back(
            SiteInfo(std::string(webkit_web_view_get_uri(tab->webview)), {}));
        //}
      }
    } else {
      int tex_w = cairo_image_surface_get_width(tab_icon);
      int tex_h = cairo_image_surface_get_height(tab_icon);
      unsigned char *data = cairo_image_surface_get_data(tab_icon);

      auto format_raw = cairo_image_surface_get_format(tab_icon);

      Image img = {.data = data,
                   .width = tex_w,
                   .height = tex_h,
                   .mipmaps = 1,
                   .format = PIXELFORMAT_UNCOMPRESSED_R32G32B32A32};

      icons->push_back(
          SiteInfo(std::string(webkit_web_view_get_uri(tab->webview)), img));
    }
  }

  return this->tabs->size() == icons->size();
}