#include <raylib.h>
#include "CEFGLWindow.hpp"
#include "GLFW/glfw3.h"
#include "JSKeyCodes.hpp"
#include "Window.hpp"
#include "internal/cef_types.h"
#include "internal/cef_types_wrappers.h"
#include "rlgl.h"

int main(int argc, char* argv[]) {
  Window* window = new Window(argc, argv);

  while (!WindowShouldClose()) {
    window->resizeTranslation();
    window->keyTranslation();
    window->mouseTranslation();
    window->resizeTranslation();
    window->render();
  }
  return 0;
}
