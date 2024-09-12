#include "RaylibWindow.hpp"
#include "rs_image.h"

#ifdef WIN32
#else
#include <sys/utsname.h>
#endif

#define LIGHTGOLD      \
  (Color) {            \
    255, 247, 130, 255 \
  }

static void InitBrowser(int argc, char** argv) {
  // This function should be called from the application entry point function to
  // execute a secondary process. It can be used to run secondary processes from
  // the browser client executable (default behavior) or from a separate
  // executable specified by the CefSettings.browser_subprocess_path value. If
  // called for the browser process (identified by no "type" command-line value)
  // it will return immediately with a value of -1. If called for a recognized
  // secondary process it will block until the process should exit and then
  // return the process exit code. The |application| parameter may be empty. The
  // |windows_sandbox_info| parameter is only used on Windows and may be NULL
  // (see cef_sandbox_win.h for details).
  CefMainArgs args(argc, argv);
  int exit_code = CefExecuteProcess(args, nullptr, nullptr);
  if (exit_code >= 0) {
    // Sub proccess has endend, so exit
    exit(exit_code);
  } else if (exit_code == -1) {
    // If called for the browser process (identified by no "type" command-line
    // value) it will return immediately with a value of -1
  }

  // Configurate Chromium
  CefSettings settings;
  // CefString(&settings.locales_dir_path) =
  // "/home/qq/MyGitHub/OffScreenCEF/godot/locales";
  // CefString(&settings.resources_dir_path) =
  // "/home/qq/MyGitHub/OffScreenCEF/godot/";
  // CefString(&settings.framework_dir_path) =
  // "/home/qq/MyGitHub/OffScreenCEF/godot/"; CefString(&settings.cache_path) =
  // "/home/qq/MyGitHub/OffScreenCEF/godot/";
  settings.windowless_rendering_enabled = true;
#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

#ifdef WIN32
#else
  utsname buf;
  uname(&buf);
  std::string os = std::format("{}; {} {}", getenv("XDG_SESSION_TYPE"),
                               buf.sysname, buf.machine);
#endif
  std::string user = std::format(
      "Mozilla/5.0 ({}) AppleWebKit/537.36 (KHTML, like Gecko) "
      "Chrome/{}.{}.{}.{} Safari/537.36",
      os, CHROME_VERSION_MAJOR, CHROME_VERSION_MINOR, CHROME_VERSION_BUILD,
      CHROME_VERSION_PATCH);

  _cef_string_utf16_t lstr = {0};
  auto fuck = CefString(user).ToString16();
  char16_t* what = (char16_t*)fuck.c_str();
  what[user.size() - 1] = 42;
  lstr.str = what;
  lstr.length = user.length();

  settings.user_agent = lstr;

  bool result = CefInitialize(args, settings, nullptr, nullptr);
  if (!result) {
    std::cerr << "CefInitialize: failed" << std::endl;
    exit(-2);
  }
}

RaylibWindow::RaylibWindow(int argc, char** argv) {
  KeycodeSetup();
  InitBrowser(argc, argv);

  window = new CEFGLWindow(width, height, "CEF OpenGL");
  window->init();

  InitWindow(width, height, "Ligma Browser");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  v = GetMousePosition();

  state = State::Browser;

  camera = (Camera3D){
      .position = (Vector3){3, 3, 3},
      .target = (Vector3){0, 0, 0},
      .up = (Vector3){0, 1, 0},
      .fovy = 90,
      .projection = CAMERA_PERSPECTIVE,
  };

  insertTab(TabPosition(0, 0, 0));
  setTab(TabPosition(0, 0, 0));

  this->time = new SystemTime();
}

void RaylibWindow::resizeTranslation() {
  // WINDOW SIZE TRANSLATION
  if (GetScreenWidth() != width || GetScreenHeight() != height) {
    current_tab->reshape(GetScreenWidth(), GetScreenHeight());
    width = GetScreenWidth();
    height = GetScreenHeight();
  }
};
void RaylibWindow::keyTranslation() {
  if (IsKeyReleased(KEY_F1)) {
    this->toggleView();
  }
  // KEYBOARD TRANSLATION
  for (int i = 0; i < 339; i++) {
    if (IsKeyPressed(i) || IsKeyPressedRepeat(i) || IsKeyReleased(i)) {
      auto key = i;
      auto ch = 0;
      if (key >= 32 && key <= 127) {
        ch = key;
      }

      if (key != KEY_NULL) {
        CefKeyEvent evt;
        evt.character = ch;
        evt.windows_key_code = RaylibKeyToJavascript((KeyboardKey)key);
        evt.native_key_code = glfwGetKeyScancode(key);
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
          evt.modifiers = EVENTFLAG_SHIFT_DOWN;
        } else if (!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT)) {
          if (key >= 65 && key <= 90) {
            evt.character += 32;
          }
        }
        if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) {
          evt.modifiers &= EVENTFLAG_ALT_DOWN;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
          evt.modifiers &= EVENTFLAG_CONTROL_DOWN;
        }
        if (IsKeyReleased(i)) {
          evt.type = KEYEVENT_KEYUP;

          current_tab->m_browser->GetHost()->SendKeyEvent(evt);
        } else {
          evt.type = KEYEVENT_RAWKEYDOWN;
          current_tab->m_browser->GetHost()->SendKeyEvent(evt);
          evt.type = KEYEVENT_CHAR;
          current_tab->m_browser->GetHost()->SendKeyEvent(evt);
        }
        evt.focus_on_editable_field = true;
      }
    }
  }
}

void RaylibWindow::mouseTranslation() {
  auto nv = GetMousePosition();
  if (v.x != nv.x || nv.y != nv.y) {
    v = nv;
    current_tab->m_mouse_x = v.x;
    current_tab->m_mouse_y = v.y;
  }
  CefMouseEvent evt;
  evt.x = v.x;
  evt.y = v.y;
  auto host = current_tab->m_browser->GetHost();

  auto left_down = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
  if (left_down) {
    if (leftClickCounter < 3) {
      leftClickCounter += 1;
    }
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_LEFT,
                              true, leftClickCounter);
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_LEFT,
                              false, leftClickCounter);
  }

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    current_tab->m_render_handler->UpdateDragCursor(current_tab->m_browser,
                                                    DRAG_OPERATION_MOVE);
  }

  auto right_down = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);
  if (right_down) {
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_MIDDLE,
                              true, 1);
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_MIDDLE,
                              false, 1);
  }

  auto middle_down = IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON);
  if (middle_down) {
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_RIGHT,
                              true, 1);
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_RIGHT,
                              false, 1);
  }

  host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_LEFT,
                            false, 1);
  auto scroll = GetMouseWheelMoveV();
  if (scroll.x != 0.0 || scroll.y != 0.0) {
    host->SendMouseWheelEvent(evt, scroll.x * 50.0, scroll.y * 50.0);
  }

  /*CefRefPtr<CefV8Context> v8_context =
      current_tab->m_render_handler.->GetV8Context();
  if (v8_context.get() && v8_context->Enter()) {
    CefRefPtr<CefV8Value> retval;
    CefRefPtr<CefV8Exception> exception;
    if (v8_context->Eval("document.body.style.cursor", "", 0, retval,
                         exception)) {
      CefString rect_x = retval->GetStringValue();
      std::println(std::cout, "{}", rect_x.ToString().c_str());
    }
    v8_context->Exit();
  }*/

  if (this->time->get_duration()->as_secs_f64() >= 1.0 / 1.0) {
    if (leftClickCounter > 0) {
      leftClickCounter -= 1;
    }
    this->time = new SystemTime();
  };
}

void RaylibWindow::render() {
  switch (state) {
    case State::Browser:
      renderBrowserWindow();
      break;
    case State::Tabs:
      renderTabs();
      break;
  }
}

void RaylibWindow::renderBrowserWindow() {
  // RENDERING
  BeginDrawing();

  ClearBackground(WHITE);
  if (current_tab->Progress() < 1.0) {
    DrawRectangle(0, 0,
                  (int)((float)GetScreenWidth() * current_tab->Progress()), 16,
                  BLUE);
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (IsTextureReady(current_tab->GetTexture())) {
    DrawTexture(current_tab->GetTexture(), 0, 0, WHITE);
  }

  CefDoMessageLoopWork();
  EndDrawing();
}

void RaylibWindow::renderTabs() {
  auto v = -GetMouseWheelMove();
  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    dimension -= v;
    if (time_since_scroll <= 32) {
      time_since_scroll += 1;
    }
  } else {
    if (camera.position.x + v >= 1) {
      camera.position.x += v;
      camera.position.y += v;
      camera.position.z += v;
    }
    if (time_since_scroll > 0) {
      time_since_scroll -= 1;
    }
  }

  std::string tab_title = "";

  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE,
                         RAYWHITE);

  BeginMode3D(camera);

  Ray ray = GetMouseRay(GetMousePosition(), camera);

  float mul = camera.position.x;
  for (float y = -mul * 10; y < mul * 10; y += 2) {
    for (float x = -mul * 10; x < mul * 10; x += 2) {
      Color col;

      auto coll = GetRayCollisionBox(
          ray,
          (BoundingBox){(Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                        (Vector3){floorf(x), 0, floorf(y)}});
      // If it's an open tab

      auto index = TabPosition(floorf(x), floorf(y), dimension);
      auto alpha = 1.0 - (std::abs(y / (mul + 25)) + std::abs(x / (mul + 25)));
      if (alpha <= 0.25) {
        continue;
      }
      if (hasTab(index)) {
        auto tab_ = tabAt(index);
        if (!tab_.has_value()) {
          continue;
        }
        auto tab = tab_.value();

        auto coll = GetRayCollisionBox(
            ray, (BoundingBox){
                     (Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                     (Vector3){floorf(x), 0, floorf(y)}});

        auto img = tab->GetIcon();
        if (img != NULL) {
          if (coll.hit) {
            col = LIGHTGOLD;
            tab_title = tab->title;
          } else {
            col = WHITE;
          }
          DrawModel(*img, (Vector3){x, 0, y}, 1.1, col);
        } else {
          if (coll.hit) {
            col = GOLD;
          } else {
            col = GRAY;
          }
          DrawCube((Vector3){x, 0, y}, 1.0, 1.0, 1.0, col);
        }
        if (coll.hit) {
          if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            setTab(index);
            toggleView();
            updateButtons();
          }
          if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            removeTab(index);
            if (isCurrent(index)) {
              disableGUI();
            }
          }
        }

      } else {
        if (coll.hit) {
          col = ColorAlpha(LIGHTGOLD, alpha);
          if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            insertTab(index);
            setTab(index);
            toggleView();
            updateButtons();
          }
        } else {
          col = ColorAlpha(LIGHTGRAY, alpha);
        }
        DrawCube((Vector3){x, 0, y}, 1.0, 1.0, 1.0, col);
      }

      // In any case, if the coll hits, set the pointer accordingly
      if (coll.hit) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
      } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      }
    }
  }
  EndMode3D();

  if (tab_title != "") {
    auto mousePos = GetMousePosition();
    auto dim = MeasureText(tab_title.c_str(), 20);
    DrawRectangle(mousePos.x - 2, mousePos.y - 2, dim + 30, 20 + 2, LIGHTGOLD);
    DrawText(tab_title.c_str(), mousePos.x + 19, mousePos.y - 1, 20, BLACK);
  }
  if (time_since_scroll != 0) {
    int y_by = time_since_scroll;
    if (y_by >= 8) {
      y_by = time_since_scroll;
    }
    auto num = std::format("{}", dimension);
    DrawText(num.c_str(), GetScreenWidth() / 2 - MeasureText(num.c_str(), 32),
             GetScreenHeight() - (y_by * 8), 32, GRAY);
  }

  CefDoMessageLoopWork();
  EndDrawing();
}

void RaylibWindow::insertTab(TabPosition index) {
  auto tab = window->createBrowser("https://google.com");
  auto t = tab.lock();
  t->reshape(width, height);

  this->tabs.insert(std::pair<std::string, std::shared_ptr<BrowserView>>(
      this->hashIndex(index), tab));
}

void RaylibWindow::setTab(TabPosition index) {
  current_tab = this->tabs.at(this->hashIndex(index));
}

void RaylibWindow::removeTab(TabPosition index) {
  this->tabs.erase(this->hashIndex(index));
}

bool RaylibWindow::hasTab(TabPosition index) {
  return this->tabs.contains(this->hashIndex(index));
}

bool RaylibWindow::isCurrent(TabPosition index) {
  return current_tab == this->tabs.at(this->hashIndex(index));
}

std::optional<TabInfo*> RaylibWindow::tabAt(TabPosition index) {
  if (tabs.contains(hashIndex(index))) {
    if (!tabinfos.contains(hashIndex(index))) {
      tabinfos.insert(std::pair(hashIndex(index),
                                TabInfo(this->tabs.at(hashIndex(index)))));
    };
    return &tabinfos.at(hashIndex(index));
  } else {
    return {};
  }
}

void RaylibWindow::disableGUI() {};

void RaylibWindow::toggleView() {
  if (this->state == State::Browser) {
    state = State::Tabs;
  } else {
    state = State::Browser;
  }
};

void RaylibWindow::updateButtons() {};

Model* TabInfo::GetIcon() {
  if (this->model.materialCount == 0) {
    auto view = this->view;
    auto downloadedFavicon = view->m_display_handler->downloadedFavicon;
    if (downloadedFavicon.empty()) {
      return NULL;
    }
    if (downloadedFavicon.data() == NULL) {
      return NULL;
    }
    try {
      auto img = DynamicImage(downloadedFavicon);
      if (img.width() == 0) {
        return NULL;
      }
      Image im = {0};
      im.width = img.width();
      im.height = img.height();
      im.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
      im.mipmaps = 1;
      // By no means the most efficient way to do this but we only do it once so
      // it's fine.
      std::println(std::cout, "width: {}, height: {}", img.width(),
                   img.height());

      auto pixels = img.pixels();

      pixels.for_each([&im](PixelResult pixel) {
        std::println(std::cout, "({} {})", pixel.x, pixel.y);
        auto color = pixel.color;
        ImageDrawPixel(&im, pixel.x, pixel.y,
                       (Color){color.r, color.g, color.b, color.a});
      });

      auto t = LoadTextureFromImage(im);
      this->texture = t;

      Model m = LoadModelFromMesh(GenMeshCube(1.0, 1.0, 1.0));
      Material map = m.materials[0];
      map.maps[MATERIAL_MAP_DIFFUSE].texture = t;

      m.materials[0] = map;
      this->model = m;

      this->title = view->m_display_handler->title;
    } catch (image_error* ex) {
      std::println(std::cout, "Couldn't get icon: {}", ex->what());
    }
  }
  return &this->model;
}