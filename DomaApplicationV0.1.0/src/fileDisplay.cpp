#include "fileDisplay.h"
#include "imgui.h"
#include "IdGenerator.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h> // For GetEnvironmentVariable
#include <GLFW/glfw3.h>

namespace fileDisplay {
    void fileUI() {
        // Get the font holder from ImGui IO
        ImGuiIO& io = ImGui::GetIO();
        FontHolder* fonts = static_cast<FontHolder*>(io.UserData);


        // Get the main viewport primary monitor
        ImGuiViewport* viewport = ImGui::GetMainViewport();


        // get main monitor res to scale ui using its dimensions
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);


        // Calculate scaling factor
        float scale_factor = mode->width / 1080.0f;
        if (scale_factor < 0.8f) scale_factor = 0.8f;
        if (scale_factor > 2.0f) scale_factor = 2.0f;

        ImFont* currentFont = fonts->Medium;
        if (scale_factor > 1.5f) {
            currentFont = fonts->Large;
        }
        else if (scale_factor < 1.0f) {
            currentFont = fonts->Small;
        }

        // Calculate window size (half of the viewport)
        ImVec2 windowSize(viewport->Size.x * 0.5f, viewport->Size.y);

        // Calculate window position - make it occupy the right half of the viewport
        ImVec2 windowPos(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y);

        // Set window size and position
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        // Set window size and position
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        // Add window flags to prevent user movement/resizing
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        // set font
        ImGui::PushFont(currentFont);

        // Create file window display
        ImGui::Begin("File", nullptr, window_flags);

        // Get OneDrive path from environment variable
        char onedrivePath[MAX_PATH];
        DWORD pathLength = GetEnvironmentVariableA("OneDrive", onedrivePath, MAX_PATH);

        if (pathLength == 0) {
            // OneDrive environment variable not found, try OneDriveConsumer (personal OneDrive)
            pathLength = GetEnvironmentVariableA("OneDriveConsumer", onedrivePath, MAX_PATH);

            if (pathLength == 0) {
                ImGui::BeginChild("FILE ERROR");
                ImGui::Text("Could not find OneDrive directory");
                ImGui::EndChild();
                ImGui::End();
                return;
            }
        }

        // Construct the file path
        std::string filePath = std::string(onedrivePath) + "/ClientFileIDs/IDs.txt";

        // Open the file
        std::ifstream readFile(filePath);

        if (!readFile.is_open()) {
            ImGui::BeginChild("FILE ERROR");
            ImGui::Text("FILE::ERROR:: COULD NOT OPEN FILE");
            ImGui::Text("Tried path: %s", filePath.c_str());
            ImGui::EndChild();
            ImGui::End();
            return;
        }

        std::string getLastLine;

        // Get and display each line from the file
        while (std::getline(readFile, getLastLine)) {
            if (!getLastLine.empty()) {
                // Display each line onto window
                for (char digit : getLastLine) {
                    ImGui::SameLine();
                    ImGui::Text("%c", digit);
                }
                ImGui::NewLine();
            }
        }

        ImGui::End();
        ImGui::PopFont();
    }
}