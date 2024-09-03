#include "GUI.h"
#include "rlgl.h"
#include <gobject/gsignal.h>

int main(int argc, char *argv[]) {
  auto gui = new Window();

  gui->main_thread();
  return 0;
}
