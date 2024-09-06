#include <raylib.h>
#include "CEFGLWindow.hpp"
#include "GLFW/glfw3.h"
#include "internal/cef_types.h"
#include "internal/cef_types_wrappers.h"
#include "rlgl.h"

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

int main(int argc, char* argv[]) {
  InitBrowser(argc, argv);

  CEFGLWindow win(800, 600, "CEF OpenGL");
  win.init();

  InitWindow(800, 600, "Ligma Browser");

  auto tab = win.createBrowser("https://google.com").lock();
  tab->reshape(GetScreenWidth(), GetScreenHeight());

  auto v = GetMousePosition();
  while (!WindowShouldClose()) {
    auto nv = GetMousePosition();

    for (auto win : win.browsers()) {
      for (int i = 39; i < 336; i++) {
        if (IsKeyDown(i)) {
          win->keyPress(glfwGetKeyScancode(i), true);
        }
        if (IsKeyReleased(i)) {
          win->keyPress(glfwGetKeyScancode(i), false);
        }
      }

      if (v.x != nv.x || nv.y != nv.y) {
        v = nv;
        win->m_mouse_x = v.x;
        win->m_mouse_y = v.y;
      }
      CefMouseEvent evt;
      evt.x = v.x;
      evt.y = v.y;
      auto left_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
      auto host = tab->m_browser->GetHost();
      if (left_down) {
        host->SendMouseClickEvent(
            evt, CefBrowserHost::MouseButtonType::MBT_LEFT, left_down, 1);
      }
    }
    BeginDrawing();
    ClearBackground(RED);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto win : win.browsers()) {
      DrawTexture(win->GetTexture(), 0, 0, WHITE);
    }

    CefDoMessageLoopWork();
    EndDrawing();
  }
  return 0;
}
