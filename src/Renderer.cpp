#include "Renderer.hpp"
#include "Instance.hpp"
#include "glib-object.h"
#include "gtk-raylib.h"
#include "raylib.h"
#include <vector>

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

  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawRectangleGradientV(0, 0, 800, 600, WHITE, RAYWHITE);

  BeginMode3D(*renderer->camera);

  float mul = renderer->camera->position.x;
  for (float y = mul * -10; y < mul * 10; y += 2) {
    for (float x = mul * -10; x < mul * 10; x += 2) {
      Color col = ColorAlpha(LIGHTGRAY,
                             1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25))));
      DrawCube((Vector3){x, 0, y}, 1.0, 1.0, 1.0, col);
    }
  }
  float x = 0;
  float y = 0;
  for (int i = 0; i < size; i++) {
    auto img = icons.at(i);
    auto col = 1.0 - (abs(y / (mul + 25)) + abs(x / (mul + 25)));
    if (img.favicon.has_value()) {
      auto tex = LoadTextureFromImage(img.favicon.value());
      DrawCubeTexture(tex, (Vector3){x, 0, y}, 1.0, 1.0, 1.0,
                      ColorAlpha(WHITE, col));
    } else {
      DrawCube((Vector3){x, 0, y}, 1.0, 1.0, 1.0, ColorAlpha(GRAY, col));
    }
    x += 2;
  }

  EndMode3D();
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
  g_signal_connect_swapped(embed, "render", G_CALLBACK(render), NULL);
  g_signal_connect_swapped(embed, "realize", G_CALLBACK(start), NULL);
}