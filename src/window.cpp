#include "window.h"
#include "logger.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string_view>

Window::Window() noexcept
{
    Window("Image App", 1280, 720);
}

Window::Window(const char *name, int width, int height, bool resizable) noexcept
    //: _context{GLFWContext::retain()},
    :_resizable{resizable}
{
	// Setup window
	if (!glfwInit())
	{
		logger("Err glfw init");
		return;
	}

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);

    // Create window with graphics context
    _handle = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (_handle == nullptr) {
        const char *error = nullptr;
        glfwGetError(&error);
        logger("Failed to create GLFW window.");
		exit(-1);
    }
    glfwMakeContextCurrent(_handle);
    glfwSwapInterval(0);// disable vsync

    int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		logger("Failed to initialize OpenGL context");
		exit(-1);
	}

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
    ImGui_ImplGlfw_InitForOpenGL(_handle, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glfwSetWindowUserPointer(_handle, this);
    glfwSetMouseButtonCallback(_handle, [](GLFWwindow *window, int button, int action, int mods) noexcept {
        if (ImGui::GetIO().WantCaptureMouse) {// ImGui is handling the mouse
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        } else {
            auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
            auto x = 0.0;
            auto y = 0.0;
            glfwGetCursorPos(self->handle(), &x, &y);
            if (auto &&cb = self->_mouse_button_callback) {
                cb(button, action, static_cast<float>(x), static_cast<float>(y));
            }
        }
    });
    // glfwSetCursorPosCallback(_handle, [](GLFWwindow *window, double x, double y) noexcept {
	// 	if (ImGui::GetIO().WantCaptureMouse) {// ImGui is handling the keyboard
    //         ImGui_ImplGlfw_CursorPosCallback(window, x, y);
    //     } else {
	// 		auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
	// 		if (auto &&cb = self->_cursor_position_callback) { cb(static_cast<float>(x), static_cast<float>(y)); }
	// 	}
    // });
    glfwSetWindowSizeCallback(_handle, [](GLFWwindow *window, int width, int height) noexcept {
        auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (auto &&cb = self->_window_size_callback) { cb(width, height); }
    });
    glfwSetKeyCallback(_handle, [](GLFWwindow *window, int key, int scancode, int action, int mods) noexcept {
        if (ImGui::GetIO().WantCaptureKeyboard) {// ImGui is handling the keyboard
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        } else {
            auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (auto &&cb = self->_key_callback) { cb(key, action); }
        }
    });
    glfwSetScrollCallback(_handle, [](GLFWwindow *window, double dx, double dy) noexcept {
        if (ImGui::GetIO().WantCaptureMouse) {// ImGui is handling the mouse
            ImGui_ImplGlfw_ScrollCallback(window, dx, dy);
        } else {
            auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
            if (auto &&cb = self->_scroll_callback) {
                cb(static_cast<int>(dx), static_cast<int>(dy));
            }
        }
    });
    glfwSetCharCallback(_handle, ImGui_ImplGlfw_CharCallback);
}

Window::~Window() noexcept {
    glfwMakeContextCurrent(_handle);
    //_texture.reset();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(_handle);
}

int Window::width() const noexcept {
    auto width = 0;
    auto height = 0;
    glfwGetWindowSize(_handle, &width, &height);
    return static_cast<int>(width);
}

int Window::height() const noexcept {
    auto width = 0;
    auto height = 0;
    glfwGetWindowSize(_handle, &width, &height);
    return static_cast<int>(height);
}

bool Window::should_close() const noexcept {
    return glfwWindowShouldClose(_handle);
}

Window &Window::set_mouse_callback(Window::MouseButtonCallback cb) noexcept {
    _mouse_button_callback = std::move(cb);
    return *this;
}

Window &Window::set_cursor_position_callback(Window::CursorPositionCallback cb) noexcept {
    _cursor_position_callback = std::move(cb);
    return *this;
}

Window &Window::set_window_size_callback(Window::WindowSizeCallback cb) noexcept {
    _window_size_callback = std::move(cb);
    return *this;
}

Window &Window::set_key_callback(Window::KeyCallback cb) noexcept {
    _key_callback = std::move(cb);
    return *this;
}

Window &Window::set_scroll_callback(Window::ScrollCallback cb) noexcept {
    _scroll_callback = std::move(cb);
    return *this;
}

void Window::set_background(const std::array<uint8_t, 4u> *pixels, int width, int height, bool bilerp) noexcept {
    // if (_texture == nullptr) { _texture = luisa::make_unique<GLTexture>(); }
    // _texture->load(pixels, size, bilerp);
}

void Window::set_should_close() noexcept {
    glfwSetWindowShouldClose(_handle, true);
}

void Window::_imgui_dock() noexcept {
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.1f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();
    if (opt_fullscreen) {
        ImGui::PopStyleVar(2);
    }
    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = minWinSizeX;
    ImGui::End();
}

void Window::_begin_frame() noexcept {
    if (!should_close()) {
        glfwMakeContextCurrent(_handle);
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        _imgui_dock();
    }
}

void Window::_end_frame() noexcept {
    if (!should_close()) {
        // background
        // if (_texture != nullptr) {
        //     auto s = make_float2(size());
        //     auto t = make_float2(_texture->size());
        //     // aspect fit
        //     auto scale = std::min(s.x / t.x, s.y / t.y);
        //     auto tl = (s - t * scale) * 0.5f;
        //     auto br = tl + t * scale;
        //     ImGui::GetBackgroundDrawList()->AddImage(
        //         _texture->handle(), {tl.x, tl.y}, {br.x, br.y});
        // }

        // rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(_handle, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(.15f, .15f, .15f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

        glfwSwapBuffers(_handle);
    }
}

void Window::set_size(int _width, int _height) noexcept {
    if (_resizable) {
        glfwSetWindowSize(_handle, static_cast<int>(_width), static_cast<int>(_height));
    } else {
        logger("Ignoring resize on non-resizable window.");
    }
}