#include "fileDisplay.h"

#include "imgui.h"
#include "IdGenerator.h"

#include "string"
#include <iostream>
#include <fstream>

namespace fileDisplay {
    void fileUI() {

        // create file window display
        ImGui::Begin("File");

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
