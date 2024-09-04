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

// !! Backported from Raylib 5.0

// RayCollision, ray hit information
typedef struct RayCollision {
  bool hit;       // Did the ray hit something?
  float distance; // Distance to the nearest hit
  Vector3 point;  // Point of the nearest hit
  Vector3 normal; // Surface normal of hit
} RayCollision;

RayCollision GetRayCollisionBox(Ray ray, BoundingBox box);

#endif