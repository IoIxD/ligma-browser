// This code is a modification of the original "cefsimple" example that can be
// found at https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage

#ifndef BROWSERVIEW_HPP
#define BROWSERVIEW_HPP

#include <iostream>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Matrices manipulation for OpenGL
#include <glm/ext.hpp>
#include <glm/glm.hpp>

// Chromium Embedded Framework
#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "cef_display_handler.h"
#include "internal/cef_ptr.h"
#include "raylib.h"

// ****************************************************************************
//! \brief Interface class rendering a single web page.
// ****************************************************************************
class BrowserView {
 public:
  //! \brief Default Constructor using a given URL.
  BrowserView(const std::string& url);

  //! \brief
  ~BrowserView();

  //! \brief Load the given web page.
  void load(const std::string& url);

  //! \brief Render the web page.
  void draw();

  //! \brief Set the windows size.
  void reshape(int w, int h);

  //! \brief Set the viewport: the rectangle on the window where to display
  //! the web document.
  //! \return false if arguments are incorrect.
  bool viewport(float x, float y, float w, float h);

  //! \brief Get the viewport.
  inline glm::vec4 const& viewport() const { return m_viewport; }

  //! \brief TODO
  // void executeJS(const std::string &cmd);

  //! \brief Set the new mouse position
  void mouseMove(int x, int y);

  //! \brief Set the new mouse state (clicked ...)
  void mouseClick(CefBrowserHost::MouseButtonType btn, bool mouse_up);

  //! \brief Set the new keyboard state (char typed ...)
  void keyPress(int key, bool pressed);

  Texture2D GetTexture() {
    return (Texture2D){.id = m_render_handler->texture(),
                       .width = m_render_handler->width(),
                       .height = m_render_handler->height(),
                       .mipmaps = 2,
                       .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
  };

  double Progress() { return this->m_display_handler->progress; }

 private:
  // *************************************************************************
  //! \brief Private implementation to handle CEF events to draw the web page.
  // *************************************************************************
  class RenderHandler : public CefRenderHandler {
   public:
    RenderHandler(glm::vec4 const& viewport);

    //! \brief
    ~RenderHandler();

    //! \brief Compile OpenGL shaders and create OpenGL objects (VAO,
    //! VBO, texture, locations ...)
    bool init();

    //! \brief Render OpenGL VAO (rotating a textured square)
    void draw(glm::vec4 const& viewport, bool fixed);

    //! \brief Resize the view
    void reshape(int w, int h);

    //! \brief Return the OpenGL texture handle
    GLuint texture() const { return m_tex; }

    //! \brief Return the OpenGL texture's width
    int width() const { return m_width; }

    //! \brief Return the OpenGL texture's height
    int height() const { return m_height; }

    //! \brief CefRenderHandler interface
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser,
                             CefRect& rect) override;

    //! \brief CefRenderHandler interface
    //! Update the OpenGL texture.
    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                         PaintElementType type,
                         const RectList& dirtyRects,
                         const void* buffer,
                         int width,
                         int height) override;

    //! \brief CefBase interface
    IMPLEMENT_REFCOUNTING(RenderHandler);

   private:
    //! \brief Dimension
    int m_width;
    int m_height;

    //! \brief Where to draw on the OpenGL window
    glm::vec4 const& m_viewport;

    //! \brief OpenGL shader program handle
    GLuint m_prog = 0;
    //! \brief OpenGL texture handle
    GLuint m_tex = 0;
    //! \brief OpenGL vertex array object handle
    GLuint m_vao = 0;
    //! \brief OpenGL vertex buffer obejct handle
    GLuint m_vbo = 0;

    //! \brief OpenGL shader variable locations for vertices of the
    //! rectangle
    GLint m_pos_loc = -1;
    //! \brief OpenGL shader variable locations for the texture
    GLint m_tex_loc = -1;
    //! \brief OpenGL shader variable locations for the Model View
    //! Projection matrix.
    GLint m_mvp_loc = -1;
  };

  class DisplayHandler : public CefDisplayHandler {
    double progress;
    bool cursorLocked = false;

   public:
    DisplayHandler() {};
    ~DisplayHandler() {};
    //! \brief CefDisplayHandler interface
    //! Update the cursor.
    virtual bool OnCursorChange(
        CefRefPtr<CefBrowser> browser,
        CefCursorHandle cursor,
        cef_cursor_type_t type,
        const CefCursorInfo& custom_cursor_info) override;
    virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser,
                                         double progress) override;
    IMPLEMENT_REFCOUNTING(DisplayHandler);

    friend class BrowserView;
  };
  // *************************************************************************
  //! \brief Provide access to browser-instance-specific callbacks. A single
  //! CefClient instance can be shared among any number of browsers.
  // *************************************************************************
  class BrowserClient : public CefClient {
   public:
    BrowserClient(CefRefPtr<CefRenderHandler> ptr,
                  CefRefPtr<CefDisplayHandler> ptr2)
        : m_renderHandler(ptr), m_displayHandler(ptr2) {}

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override {
      return m_renderHandler;
    }

    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
      return m_displayHandler;
    }

    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<CefDisplayHandler> m_displayHandler;

    IMPLEMENT_REFCOUNTING(BrowserClient);
  };

 public:
  //! \brief Mouse cursor position on the OpenGL window
  int m_mouse_x;
  int m_mouse_y;

  //! \brief Where to draw on the OpenGL window
  glm::vec4 m_viewport;

  //! \brief Chromium Embedded framework elements
  CefRefPtr<CefBrowser> m_browser;
  CefRefPtr<BrowserClient> m_client;
  RenderHandler* m_render_handler = nullptr;
  DisplayHandler* m_display_handler = nullptr;

  //! \brief OpenGL has created GPU elements with success
  bool m_initialized = false;

 public:
  //! \brief If set to false then the web page is turning.
  bool m_fixed = true;
};

#endif  // BROWSERVIEW_HPP
