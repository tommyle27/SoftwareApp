#include "IdGenerator.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <algorithm> // For std::clamp
#include <windows.h> // For GetEnvironmentVariable
#include <GLFW/glfw3.h>


#include "imgui.h"
#include "fileDisplay.h"

namespace DomaApp {

    std::string getLastLine(const std::string& filePath) {

        //get file path
        std::ifstream readFile(filePath);
        if (!readFile.is_open()) {
            return "00000000"; // Or throw an exception
        }

        std::string getLastLine;
        std::string lastLine;


        // get last line in file
        while (std::getline(readFile, getLastLine)) {

            if (!getLastLine.empty()) {
                lastLine = getLastLine;
            }

        }

        // if file is empty
        if (lastLine.empty()) {
            return "00000000";
        }

        // convert string into an integer increment is by one
        // convert back into a string and format it into 8 character string of integers following naming convention
        try {
            unsigned int lastLineInt = std::stoi(lastLine);
            lastLineInt++;
            std::string newLine = std::to_string(lastLineInt);
            size_t numZeros = 8 - newLine.size();
            return std::string(numZeros, '0') + newLine;
        }
        catch (const std::exception& e) {
            return "00000000"; // Or handle error
        }

    }


    void RenderIdGeneratorUI() {
        // Get the font holder from ImGui IO
        ImGuiIO& io = ImGui::GetIO();
        FontHolder* fonts = static_cast<FontHolder*>(io.UserData);

        // get monitor res to scale font/ui using its dimensions
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        // Calculate a scaling factor based on display height (you could use width instead)
        // We'll use 1080p as our reference resolution
        // Calculate scaling factor with manual clamping
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


        // get the main viewport primary monitor
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        // calculate new size of window (half of the viewport)
        ImVec2 windowSize(viewport->Size.x * 0.5f, viewport->Size.y);
        // Calculate window position - place it to the left half of the viewport
        ImVec2 windowPos(viewport->Pos.x, viewport->Pos.y);


        // Set window size and position
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        // set the next window size and position
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        // Add window flags to prevent user movement/resizing
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushFont(currentFont);
        ImGui::Begin("GenerateID", nullptr, window_flags);

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


        std::unordered_set<std::string> idSet;
        char stringBuffer[1024] = { 0 };

        try {
            // get new ID that can be used
            std::string newID = getLastLine(filePath);
            strncpy_s(stringBuffer, newID.c_str(), sizeof(stringBuffer) - 1);

            // Calculate sizes dynamically
            float textWidth = ImGui::CalcTextSize("New ID: ").x;
            float inputWidth = ImGui::GetWindowWidth() * 0.15f * scale_factor; // 15% of window width
            float totalWidth = textWidth + inputWidth + ImGui::GetStyle().ItemSpacing.x * scale_factor;

            // Center ID display
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - totalWidth) * 0.5f);
            ImGui::Text("New ID: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputText("##label", stringBuffer, sizeof(stringBuffer), ImGuiInputTextFlags_ReadOnly);

            // ERROR CHECKERS -------------------------------------------------------------------------------------------------------------------------------------------------------
            std::ifstream readCustomerFile(filePath);
            if (readCustomerFile.is_open()) {
                // iterate through file checking for errors
                std::string line;
                while (std::getline(readCustomerFile, line)) {

                    if (line.size() != 8 || line.find_first_not_of("0123456789") != std::string::npos) {
                        ImGui::BeginChild("FILE ERROR");
                        ImGui::Text("ID::ERROR:: ID INVALID FORMAT");
                        ImGui::SameLine();
                        ImGui::Text(line.c_str());
                        ImGui::EndChild();
                    }

                    // if ID exists in file throw error
                    if (line == newID) {
                        ImGui::BeginChild("FILE ERROR");
                        ImGui::Text("FILE::ERROR:: ENSURE FILE IS ORGANIZED FROM SMALLEST TO LARGEST");
                        ImGui::EndChild();

                    }

                    // Check if the ID is already in the set
                    if (idSet.find(line) != idSet.end()) {
                        ImGui::BeginChild("FILE ERROR");

                        ImGui::Text("ID::ERROR:: DUPLICATE ID FOUND");
                        ImGui::SameLine();
                        ImGui::Text(line.c_str());

                        ImGui::EndChild();

                    }
                    else {
                        idSet.insert(line);
                    }

                }
                // -------------------------------------------------------------------------------------------------------------------------------------------------------
            }

            // Scale button padding and size
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x * scale_factor, ImGui::GetStyle().FramePadding.y * scale_factor));

            // Center and scale button
            float buttonWidth = (ImGui::CalcTextSize("Save/Use ID").x + (ImGui::GetStyle().FramePadding.x * 2.0f * scale_factor)) * scale_factor;

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.6f);
            if (ImGui::Button("Save/Use ID")) {
                std::ofstream writeToFile(filePath, std::ios::app);
                if (writeToFile.is_open()) {
                    writeToFile << newID << std::endl;
                }
            }
            ImGui::PopStyleVar();

        }
        catch (const std::exception& e) {
            ImGui::Text("Error: %s", e.what());
        }

        ImGui::End();
        ImGui::PopFont();
    }



}
