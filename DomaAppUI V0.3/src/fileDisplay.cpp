#include "fileDisplay.h"

#include "imgui.h"
#include "IdGenerator.h"

#include "string"
#include <iostream>
#include <fstream>

namespace fileDisplay {
    void fileUI() {
        // get the main viewport primary monitor
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Calculate a scaling factor based on display height (you could use width instead)
        // We'll use 1080p as our reference resolution
        // Calculate scaling factor with manual clamping
        float scale_factor = viewport->Size.y / 1080.0f;
        if (scale_factor < 0.8f) scale_factor = 0.8f;
        if (scale_factor > 2.0f) scale_factor = 2.0f;


        // calculate new size of window (90% of display size)
        ImVec2 windowSize(viewport->Size.x * 0.45f, viewport->Size.y * 0.9f);

        // set the next window size and position
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        // create file window display
        ImGui::Begin("File");

        // Scale the font
        ImGui::SetWindowFontScale(scale_factor);

        // get each line from file and display it as text onto window
        std::string filePath = "C:/Users/tommy/Desktop/DOMA Customer IDs/CustomerIDs.txt";
        std::ifstream readFile(filePath);

        if (!readFile.is_open()) {
            ImGui::BeginChild("FILE ERROR");
            ImGui::Text("FILE::ERROR:: COULD NOT OPEN FILE");

            ImGui::EndChild();
        }

        std::string getLastLine;
        std::string lastLine;


        // get last line in file
        while (std::getline(readFile, getLastLine)) {

            if (!getLastLine.empty()) {
                lastLine = getLastLine;

                // dislpay each line onto window
                for (char digit : lastLine) {
                    ImGui::SameLine();
                    ImGui::Text("%c", digit);
                }
                ImGui::NewLine();

            }

        }

        ImGui::End();
    }
}
