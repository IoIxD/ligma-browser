#include "Renderer.hpp"
#include "Instance.hpp"
#include "glib-object.h"
#include "gtk-raylib.h"
#include "raylib.h"
#include "rlgl.h"
#include <cmath>
#include <vector>

#define LIGHTGOLD                                                              \
  (Color) { 255, 247, 130, 255 }

// RayCollision, ray hit information
typedef struct RayCollision {
  bool hit;       // Did the ray hit something?
  float distance; // Distance to the nearest hit
  Vector3 point;  // Point of the nearest hit
  Vector3 normal; // Surface normal of hit
} RayCollision;

// Get collision info between ray and box
RayCollision GetRayCollisionBox(Ray ray, BoundingBox box) {
  RayCollision collision = {0};

  // Note: If ray.position is inside the box, the distance is negative (as if
  // the ray was reversed) Reversing ray.direction will give use the correct
  // result
  bool insideBox =
      (ray.position.x > box.min.x) && (ray.position.x < box.max.x) &&
      (ray.position.y > box.min.y) && (ray.position.y < box.max.y) &&
      (ray.position.z > box.min.z) && (ray.position.z < box.max.z);

  if (insideBox)
    ray.direction = Vector3Negate(ray.direction);

  float t[11] = {0};

  t[8] = 1.0f / ray.direction.x;
  t[9] = 1.0f / ray.direction.y;
  t[10] = 1.0f / ray.direction.z;

  t[0] = (box.min.x - ray.position.x) * t[8];
  t[1] = (box.max.x - ray.position.x) * t[8];
  t[2] = (box.min.y - ray.position.y) * t[9];
  t[3] = (box.max.y - ray.position.y) * t[9];
  t[4] = (box.min.z - ray.position.z) * t[10];
  t[5] = (box.max.z - ray.position.z) * t[10];
  t[6] =
      (float)fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
  t[7] =
      (float)fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

  collision.hit = !((t[7] < 0) || (t[6] > t[7]));
  collision.distance = t[6];
  collision.point =
      Vector3Add(ray.position, Vector3Scale(ray.direction, collision.distance));

  // Get box center point
  collision.normal = Vector3Lerp(box.min, box.max, 0.5f);
  // Get vector center point->hit point
  collision.normal = Vector3Subtract(collision.point, collision.normal);
  // Scale vector to unit cube
  // NOTE: We use an additional .01 to fix numerical errors
  collision.normal = Vector3Scale(collision.normal, 2.01f);
  collision.normal =
      Vector3Divide(collision.normal, Vector3Subtract(box.max, box.min));
  // The relevant elements of the vector are now slightly larger than 1.0f (or
  // smaller than -1.0f) and the others are somewhere between -1.0 and 1.0
  // casting to int is exactly our wanted normal!
  collision.normal.x = (float)((int)collision.normal.x);
  collision.normal.y = (float)((int)collision.normal.y);
  collision.normal.z = (float)((int)collision.normal.z);

  collision.normal = Vector3Normalize(collision.normal);

  if (insideBox) {
    // Reset ray.direction
    ray.direction = Vector3Negate(ray.direction);
    // Fix result
    collision.distance *= -1.0f;
    collision.normal = Vector3Negate(collision.normal);
  }

  return collision;
}

static bool image_filled = false;
static std::vector<SiteInfo> icons;
bool render() {
  if (!image_filled) {
    image_filled = GetInstance()->GetIcons(&icons);
  }

  auto size = icons.size();

  auto hello = "Congrats! You created your first window!";

  int w = GetScreenWidth();
  int h = GetScreenHeight();
  int w_text = MeasureText(hello, 20); // width text

  auto renderer = GetInstance()->renderer();
  auto v = -GetMouseWheelMove();
  if (renderer->camera->position.x + v >= 1) {
    renderer->camera->position.x += v;
    renderer->camera->position.y += v;
    renderer->camera->position.z += v;
  }

  std::string tab_title = "";

  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawRectangleGradientV(0, 0, 800, 600, WHITE, RAYWHITE);

  BeginMode3D(*renderer->camera);

  Ray ray = GetMouseRay(GetMousePosition(), *renderer->camera);

  float mul = renderer->camera->position.x;
  for (float y = mul * -10; y < mul * 10; y += 2) {
    for (float x = mul * -10; x < mul * 10; x += 2) {
      if (x == 0 && y == 0) {
        continue;
      }
      Color col;

      auto coll = GetRayCollisionBox(
          ray,
          (BoundingBox){(Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                        (Vector3){floorf(x), 0, floorf(y)}});
      if (coll.hit) {
        col = ColorAlpha(LIGHTGOLD,
                         1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25))));
      } else {
        col = ColorAlpha(LIGHTGRAY,
                         1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25))));
      }
      DrawCube((Vector3){x, 0, y}, 1.0, 1.0, 1.0, col);
    }
  }
  float x = 0;
  float y = 0;
  for (int i = 0; i < size; i++) {
    auto img = icons.at(i);
    auto col = 1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25)));
    auto coll = GetRayCollisionBox(
        ray,
        (BoundingBox){(Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                      (Vector3){floorf(x), 0, floorf(y)}});
    if (img.favicon.has_value()) {

      if (coll.hit) {
        DrawCubeTexture(img.favicon.value(), (Vector3){x, 0, y}, 1.0, 1.0, 1.0,
                        LIGHTGOLD);
        tab_title = img.title;
      } else {
        DrawCubeTexture(img.favicon.value(), (Vector3){x, 0, y}, 1.0, 1.0, 1.0,
                        WHITE);
      }
    } else {
      if (coll.hit) {
        DrawCube((Vector3){x, 0, y}, 1.0, 1.0, 1.0, ColorAlpha(GOLD, col));
      } else {
        DrawCube((Vector3){x, 0, y}, 1.0, 1.0, 1.0, ColorAlpha(GRAY, col));
      }
    }
    x += 2;
  }

  EndMode3D();

  if (tab_title != "") {
    auto mousePos = GetMousePosition();
    auto dim = MeasureText(tab_title.c_str(), 20);
    DrawRectangle(mousePos.x - 2, mousePos.y - 2, dim + 30, 20 + 2, LIGHTGOLD);
    DrawText(tab_title.c_str(), mousePos.x + 19, mousePos.y - 1, 20, BLACK);
  }
  EndDrawing();
  return true;
}

void start() { SetTargetFPS(60); }

Renderer::Renderer() {
  *camera = (Camera3D){
      .position = (Vector3){3, 3, 3},
      .target = (Vector3){0, 0, 0},
      .up = (Vector3){0, 1, 0},
      .fovy = 90,
      .projection = CAMERA_PERSPECTIVE,
  };

  printf("Renderer\n");
  embed = gtk_raylib_embed_new();
  TraceLogLevel(LOG_WARNING);
  g_signal_connect_swapped(embed, "render", G_CALLBACK(render), NULL);
  g_signal_connect_swapped(embed, "realize", G_CALLBACK(start), NULL);
}