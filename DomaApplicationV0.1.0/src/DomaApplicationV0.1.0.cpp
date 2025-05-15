#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>

#define GL_SILENCE_DEPRECATION

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "string"
#include "IdGenerator.h"
#include "fileDisplay.h"

#include <iostream>
#include <vector>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static bool is_context_valid() {
    // Check if we have a valid OpenGL context by querying a simple value
    return glGetString(GL_VERSION) != nullptr;
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);

    // ===== 1. FIRST TRY ANGLE (D3D11 BACKEND) =====
    glfwInitHint(GLFW_ANGLE_PLATFORM_TYPE, GLFW_ANGLE_PLATFORM_TYPE_D3D11);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW with ANGLE (D3D11). Falling back to default.\n";
        glfwInitHint(GLFW_ANGLE_PLATFORM_TYPE, GLFW_ANGLE_PLATFORM_TYPE_NONE);
        if (!glfwInit()) {
            std::cerr << "FATAL: GLFW initialization failed completely.\n";
            return 1;
        }
    }

    // ===== 2. GET MONITOR DIMENSIONS =====
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    // make window 80% size of primary monitor
    int windowWidth = static_cast<int>(mode->width * 0.8);
    int windowHeight = static_cast<int>(mode->height * 0.8);

    // ===== 3. TRY DIFFERENT CONTEXT CREATION METHODS =====
    GLFWwindow* window = nullptr;
    const char* glsl_version = nullptr;

    // Attempt 1: ANGLE (D3D11)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_ANGLE_PLATFORM_TYPE_D3D11);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    window = glfwCreateWindow(windowWidth, windowHeight, "Doma ID Generator", nullptr, nullptr);

    if (!window) {
        std::cerr << "ANGLE (D3D11) failed. Trying OpenGL ES...\n";
        // Attempt 2: OpenGL ES
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        window = glfwCreateWindow(windowWidth, windowHeight, "Doma ID Generator", nullptr, nullptr);
    }

    if (!window) {
        std::cerr << "OpenGL ES failed. Trying standard OpenGL...\n";
        // Attempt 3: Standard OpenGL (Core + Compatibility)
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(windowWidth, windowHeight, "Doma ID Generator", nullptr, nullptr);

        if (!window) {
            // ==== TRY OPENGL ES 2.0 (BEST FOR SURFACE INTEL GPUs) ====
            // Attempt 4:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            window = glfwCreateWindow(windowWidth, windowHeight, "Doma ID Generator", nullptr, nullptr);
        }

        if (!window) {
            // Attempt 5: Standard OpenGL 2.1
            std::cerr << "OpenGL ES 2.0 failed. Trying OpenGL 2.1...\n";
            glfwDefaultWindowHints();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            window = glfwCreateWindow(windowWidth, windowHeight, "Doma ID Generator", nullptr, nullptr);
        }

        if (!window) {
            // Last attempt: No version hints (let GLFW decide)
            glfwDefaultWindowHints();
            window = glfwCreateWindow(windowWidth, windowHeight, "Doma ID Generator", nullptr, nullptr);
        }
    }

    if (!window) {
        const char* description;
        int code = glfwGetError(&description);
        std::cerr << "FATAL: Failed to create GLFW window. Error " << code << ": " << description << "\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // ===== 4. DETECT ACTUAL GL VERSION =====
    const char* gl_version = (const char*)glGetString(GL_VERSION);
    if (gl_version) {
        std::cout << "Running with: " << gl_version << "\n";
        if (strstr(gl_version, "OpenGL ES")) {
            glsl_version = "#version 300 es";
        }
        else if (strstr(gl_version, "3.3") || strstr(gl_version, "3.2")) {
            glsl_version = "#version 330";
        }
        else if (strstr(gl_version, "3.1") || strstr(gl_version, "3.0")) {
            glsl_version = "#version 130";
        }
        else {
            glsl_version = "#version 120";
        }
    }
    else {
        std::cerr << "WARNING: Could not detect OpenGL version. Using fallback GLSL.\n";
        glsl_version = "#version 120";
    }

    // ===== 5. CENTER WINDOW =====
    // Center the window if not fullscreen
    if (primaryMonitor && window) {
        int monitorX, monitorY;
        glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        glfwSetWindowPos(window,
            monitorX + (mode->width - windowWidth) / 2,
            monitorY + (mode->height - windowHeight) / 2);
    }

    
    // ===== 6. SETUP IMGUI =====
    // use this number to scale the base value of the font sizes
    float fontScale = mode->width / 1440.0f;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // DockingEnable
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // ViewportsEnable
    ImFont* font_small = io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 8.0f * fontScale);
    ImFont* font_medium = io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 12.0f * fontScale);
    ImFont* font_large = io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 18.0f * fontScale);

    static struct FontHolder {
        ImFont* small;
        ImFont* medium;
        ImFont* large;
    } fonts;

    fonts.small = font_small;
    fonts.medium = font_medium;
    fonts.large = font_large;
    io.UserData = &fonts;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);



    // ===== 7. MAIN LOOP =====

    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render ID generator UI
        DomaApp::RenderIdGeneratorUI();
        // render file ui
        fileDisplay::fileUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

       glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
