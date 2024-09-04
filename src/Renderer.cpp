#include "Renderer.hpp"
#include "GUI.h"
#include "Instance.hpp"
#include "glib-object.h"
#include "gtk-raylib.h"
#include "raylib.h"
#include "rlgl.h"
#include <cmath>
#include <format>
#define LIGHTGOLD                                                              \
  (Color) { 255, 247, 130, 255 }

static float dimension;
static int time_since_scroll = 0;

bool render(GtkWidget *btn, GdkEventButton *event, gpointer userdata) {
  auto renderer = GetInstance()->renderer();
  if (!GetInstance()->renderer_active) {
    return false;
  }

  auto v = -GetMouseWheelMove();
  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    dimension -= v;
    if (time_since_scroll <= 32) {
      time_since_scroll += 1;
    }
  } else {
    if (renderer->camera->position.x + v >= 1) {
      renderer->camera->position.x += v;
      renderer->camera->position.y += v;
      renderer->camera->position.z += v;
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

  BeginMode3D(*renderer->camera);

  Ray ray = GetMouseRay(GetMousePosition(), *renderer->camera);

  float mul = renderer->camera->position.x;
  for (float y = mul * -10; y < mul * 10; y += 2) {
    for (float x = mul * -10; x < mul * 10; x += 2) {
      Color col;

      auto coll = GetRayCollisionBox(
          ray,
          (BoundingBox){(Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                        (Vector3){floorf(x), 0, floorf(y)}});
      // If it's an open tab

      auto index = TabPosition(x, y, dimension);
      if (GetInstance()->has_tab(index)) {

        auto tab = GetInstance()->tab_at(index);
        if (!tab.has_value()) {
          continue;
        }
        auto alpha = 1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25)));
        auto coll = GetRayCollisionBox(
            ray, (BoundingBox){
                     (Vector3){floorf(x) - 0.75f, -0.75, floorf(y) - 0.75f},
                     (Vector3){floorf(x), 0, floorf(y)}});

        auto img = tab.value()->GetIcon();
        if (img.has_value()) {
          if (img->favicon.has_value()) {
            if (coll.hit) {
              col = LIGHTGOLD;
              tab_title = img->title;
            } else {
              col = WHITE;
            }
            DrawCubeTexture(img->favicon.value(), (Vector3){x, 0, y}, 1.0, 1.0,
                            1.0, col);
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
              GetInstance()->set_tab(index);
              GetInstance()->toggle_view();
              GetInstance()->update_buttons();
            }
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
              GetInstance()->remove_tab(index);
              if (GetInstance()->is_current(index)) {
                GetInstance()->disable_gui();
              }
            }
          }
        }
      } else {
        if (coll.hit) {
          col = ColorAlpha(LIGHTGOLD,
                           1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25))));
          if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            GetInstance()->insert_tab(index);
            GetInstance()->set_tab(index);
            GetInstance()->toggle_view();
            GetInstance()->update_buttons();
          }
        } else {
          col = ColorAlpha(LIGHTGRAY,
                           1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25))));
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
  SetTraceLogLevel(LOG_WARNING);
  g_signal_connect_swapped(embed, "render", G_CALLBACK(render), this);
  g_signal_connect_swapped(embed, "realize", G_CALLBACK(start), this);
}

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