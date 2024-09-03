#include "Instance.hpp"
#include "GUI.h"

static Window *CURRENT_INST = NULL;

Window *GetInstance() { return CURRENT_INST; }
void SetInstance(Window *win) {
  if (CURRENT_INST == NULL) {
    CURRENT_INST = win;
  } else {
    printf("WARN: Tried to set global Window instance more then once. "
           "Ignoring.\n");
  }
}