#ifndef __RENDER_HPP
#define __RENDER_HPP

#include "gtk/gtk.h"
#include "raylib.h"
class Renderer {

public:
  Camera3D *camera;
  GtkWidget *embed;
  Renderer();
};

#endif