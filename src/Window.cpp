#include "Window.hpp"
#include <raylib.h>
#include <format>
#include "CEFGLWindow.hpp"
#include "GLFW/glfw3.h"
#include "JSKeyCodes.hpp"
#include "internal/cef_types.h"
#include "internal/cef_types_wrappers.h"
#include "rlgl.h"

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

  bool result = CefInitialize(args, settings, nullptr, nullptr);
  if (!result) {
    std::cerr << "CefInitialize: failed" << std::endl;
    exit(-2);
  }
}

Window::Window(int argc, char** argv) {
  KeycodeSetup();
  InitBrowser(argc, argv);

  window = new CEFGLWindow(width, height, "CEF OpenGL");
  window->init();

  InitWindow(width, height, "Ligma Browser");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  current_tab = window->createBrowser("https://google.com").lock();
  current_tab->reshape(GetScreenWidth(), GetScreenHeight());

  v = GetMousePosition();

  state = State::Browser;

  *camera = (Camera3D){
      .position = (Vector3){3, 3, 3},
      .target = (Vector3){0, 0, 0},
      .up = (Vector3){0, 1, 0},
      .fovy = 90,
      .projection = CAMERA_PERSPECTIVE,
  };
}

void Window::resizeTranslation() {
  // WINDOW SIZE TRANSLATION
  if (GetScreenWidth() != width || GetScreenHeight() != height) {
    current_tab->reshape(GetScreenWidth(), GetScreenHeight());
    width = GetScreenWidth();
    height = GetScreenHeight();
  }
};
void Window::keyTranslation() {
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

void Window::mouseTranslation() {
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
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_LEFT,
                              true, 1);
    host->SendMouseClickEvent(evt, CefBrowserHost::MouseButtonType::MBT_LEFT,
                              false, 1);
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

  auto scroll = GetMouseWheelMoveV();
  if (scroll.x != 0.0 || scroll.y != 0.0) {
    host->SendMouseWheelEvent(evt, scroll.x * 50.0, scroll.y * 50.0);
  }
}

void Window::render() {
  switch (state) {
    case State::Browser:
      renderBrowserWindow();
      break;
    case State::Tabs:
      break;
  }
}

void Window::renderBrowserWindow() {
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

void Window::renderTabs() {
  auto v = -GetMouseWheelMove();
  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    dimension -= v;
    if (time_since_scroll <= 32) {
      time_since_scroll += 1;
    }
  } else {
    if (camera->position.x + v >= 1) {
      camera->position.x += v;
      camera->position.y += v;
      camera->position.z += v;
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

  BeginMode3D(*camera);

  Ray ray = GetMouseRay(GetMousePosition(), *camera);

  float mul = camera->position.x;
  for (float y = -mul * 10; y < mul * 10; y += 2) {
    for (float x = -mul * 10; x < mul * 10; x += 2) {
      Color col;

      auto coll = GetRayCollisionBox(
          ray,
          (BoundingBox){(Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                        (Vector3){floorf(x), 0, floorf(y)}});
      // If it's an open tab

      auto index = TabPosition(floorf(x), floorf(y), dimension);
      auto alpha = 1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25)));
      if (alpha <= 0.25) {
        continue;
      }
      if (hasTab(index)) {
        auto tab = tabAt(index);
        if (!tab.has_value()) {
          continue;
        }

        auto coll = GetRayCollisionBox(
            ray, (BoundingBox){
                     (Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                     (Vector3){floorf(x), 0, floorf(y)}});

        auto img = tab->GetIcon();
        if (img.has_value()) {
          if (coll.hit) {
            col = LIGHTGOLD;
            tab_title = tab->title;
          } else {
            col = WHITE;
          }
          // DrawCubeTexture(img.value(), (Vector3){x, 0, y}, 1.0, 1.0, 1.0,
          // col);
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
      y_by = 8;
    }
    auto num = std::format("{}", dimension);
    DrawText(num.c_str(), GetScreenWidth() / 2 - MeasureText(num.c_str(), 32),
             GetScreenHeight() - (y_by * 8), 32, GRAY);
  }
  EndDrawing();
}