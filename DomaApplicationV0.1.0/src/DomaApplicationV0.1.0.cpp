#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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

struct GLVersion {
    int major;
    int minor;
    const char* glsl_version;
    bool try_compat;
    bool try_forward_compat;
};

static bool is_context_valid() {
    // Check if we have a valid OpenGL context by querying a simple value
    return glGetString(GL_VERSION) != nullptr;
}

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Define version fallback chain
    std::vector<GLVersion> versions_to_try = {
        {3, 3, "#version 330", false, true},  // Core profile
        {3, 2, "#version 150", false, true},
        {3, 1, "#version 140", true, false},  // Compatibility profile
        {3, 0, "#version 130", true, false},
        {2, 1, "#version 120", true, false},
        {2, 0, "#version 110", true, false}
    };

    GLFWwindow* window = nullptr;
    const char* glsl_version = nullptr;

    // Get the primary monitor
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    /*
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    */

    // Try creating context with different versions
    for (const auto& version : versions_to_try) {
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Test with invisible window first
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.minor);

        if (!version.try_compat) {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, version.try_forward_compat ? GL_TRUE : GL_FALSE);

        window = glfwCreateWindow(1, 1, "Context Test", nullptr, nullptr);
        if (window) {
            glfwMakeContextCurrent(window);

            // Verify the context is actually valid
            if (is_context_valid()) {
                // Success! Now create the real window
                glfwDestroyWindow(window);

                // Recreate with proper settings
                glfwDefaultWindowHints();
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.major);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.minor);

                if (!version.try_compat) {
                    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                }

                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, version.try_forward_compat ? GL_TRUE : GL_FALSE);

                // Get monitor for centering
                GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
                window = glfwCreateWindow(
                    static_cast<int>(mode->width * 0.8),
                    static_cast<int>(mode->height * 0.8),
                    "Doma ID Generator",
                    nullptr, nullptr);

                if (window) {
                    glfwMakeContextCurrent(window);
                    if (is_context_valid()) {
                        glsl_version = version.glsl_version;
                        std::cout << "Success with OpenGL " << version.major << "." << version.minor
                            << " (" << (version.try_compat ? "compatibility" : "core")
                            << " profile)\n";
                        break;
                    }
                }
            }
            if (window) {
                glfwDestroyWindow(window);
                window = nullptr;
            }
        }
    }
    
    // Final fallback - no version hints
    if (!window) {
        glfwDefaultWindowHints();
        window = glfwCreateWindow(mode->width * 0.8, mode->height * 0.8, "Doma ID Generator Version 0.1.0", nullptr, nullptr);
        if (window) {
            glfwMakeContextCurrent(window);
            if (is_context_valid()) {
                const char* version = (const char*)glGetString(GL_VERSION);
                if (version) {
                    std::cout << "Using default OpenGL context: " << version << "\n";
                    if (strstr(version, "OpenGL ES")) {
                        glsl_version = "#version 300 es";
                    }
                    else if (atof(version) >= 3.3) {
                        glsl_version = "#version 330";
                    }
                    else if (atof(version) >= 3.0) {
                        glsl_version = "#version 130";
                    }
                    else {
                        glsl_version = "#version 120";
                    }
                }
            }
            else {
                glfwDestroyWindow(window);
                window = nullptr;
            }
        }
    }

    if (!window) {
        std::cerr << "Failed to create OpenGL context\n";
        glfwTerminate();
        return 1;
    }


    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 18.0f);
    


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

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

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
