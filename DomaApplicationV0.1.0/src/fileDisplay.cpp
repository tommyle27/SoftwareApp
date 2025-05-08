#include "fileDisplay.h"
#include "imgui.h"
#include "IdGenerator.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h> // For GetEnvironmentVariable

namespace fileDisplay {
    void fileUI() {
        // Get the main viewport primary monitor
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Calculate scaling factor
        float scale_factor = viewport->Size.y / 1080.0f;
        if (scale_factor < 0.8f) scale_factor = 0.8f;
        if (scale_factor > 2.0f) scale_factor = 2.0f;

        // Calculate window size
        ImVec2 windowSize(viewport->Size.x * 0.4f, viewport->Size.y * 0.8f);

        // Calculate window position - place it at 50% of viewport width from the left
        ImVec2 windowPos(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + (viewport->Size.y - windowSize.y) * 0.5f);

        // Set window size and position
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
        // Set window size and position
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        // Create file window display
        ImGui::Begin("File");

        // Scale the font
        ImGui::SetWindowFontScale(scale_factor);

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
    }
}