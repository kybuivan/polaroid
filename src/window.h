#ifndef _WINDOW_H_
#define _WINDOW_H_
#include <functional>
#include <array>
#define GL_SILENCE_DEPRECATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

class GLFWContext;
//class GLFWwindow;

class Window {
public:
    using MouseButtonCallback = std::function<void(int /* button */, int /* action */, float /* x */, float /* y */)>;
    using CursorPositionCallback = std::function<void(int /* x*/, int /* y */)>;
    using WindowSizeCallback = std::function<void(int /* width*/, int /* height */)>;
    using KeyCallback = std::function<void(int /* key */, int /* action */)>;
    using ScrollCallback = std::function<void(int /* dx */, int /* dy */)>;
public:
    Window() noexcept;
    Window(const char *name, int width, int height, bool resizable = false) noexcept;
    Window(Window &&) noexcept = delete;
    Window(const Window &) noexcept = delete;
    Window &operator=(Window &&) noexcept = delete;
    Window &operator=(const Window &) noexcept = delete;
    ~Window() noexcept;

    [[nodiscard]] int width() const noexcept;
    [[nodiscard]] int height() const noexcept;
    [[nodiscard]] bool should_close() const noexcept;
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] explicit operator bool() const noexcept { return !should_close(); }

    Window &set_mouse_callback(MouseButtonCallback cb) noexcept;
    Window &set_cursor_position_callback(CursorPositionCallback cb) noexcept;
    Window &set_window_size_callback(WindowSizeCallback cb) noexcept;
    Window &set_key_callback(KeyCallback cb) noexcept;
    Window &set_scroll_callback(ScrollCallback cb) noexcept;

    void set_background(const std::array<uint8_t, 4u> *pixels, int width, int height, bool bilerp = true) noexcept;

    void set_should_close() noexcept;
    void set_size(int _width, int _height) noexcept;

    template<typename F>
    void run(F &&draw) noexcept {
        while (!should_close()) {
            run_one_frame(draw);
        }
    }

    template<typename F>
    void run_one_frame(F &&draw) noexcept {
        _begin_frame();
        std::invoke(std::forward<F>(draw));
        _end_frame();
    }

private:
    void _begin_frame() noexcept;
    void _end_frame() noexcept;
    void _imgui_dock() noexcept;
private:
    //std::shared_ptr<GLFWContext> _context;
    GLFWwindow *_handle{nullptr};
    //mutable std::unique_ptr<GLTexture> _texture;
    MouseButtonCallback _mouse_button_callback;
    CursorPositionCallback _cursor_position_callback;
    WindowSizeCallback _window_size_callback;
    KeyCallback _key_callback;
    ScrollCallback _scroll_callback;
    bool _resizable;
};
#endif