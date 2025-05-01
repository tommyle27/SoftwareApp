#include "IdGenerator.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>


#include "imgui.h"
#include "fileDisplay.h"

namespace DomaApp {

    std::string getLastLine(const std::string filePath) {

        //get file path
        std::ifstream readFile(filePath);

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
        unsigned int lastLineInt = stoi(lastLine);
        lastLineInt++;

        std::string newLine = std::to_string(lastLineInt);

        unsigned int numZeros = 8 - newLine.size();
        std::string finalID;

        for (int i = 0; i < numZeros; i++) {
            finalID += '0';
        }

        return finalID + newLine;
    }


    void RenderIdGeneratorUI() {
        ImGui::Begin("GenerateID");

        std::string filePath = "C:/Users/tommy/Desktop/DOMA Customer IDs/CustomerIDs.txt";
        std::unordered_set<std::string> idSet;

        std::ofstream writeToFile(filePath, std::ios::app);
        std::ifstream readCustomerFile(filePath);

        // get new ID that can be used
        std::string newID = getLastLine(filePath);

        //create a temp buffer to store the string into char
        static char stringBuffer[1024];
        strncpy_s(stringBuffer, newID.c_str(), sizeof(stringBuffer));
        stringBuffer[sizeof(stringBuffer) - 1] = '\0'; // ensure null termination

        ImGui::Text("New ID: ");
        ImGui::SameLine();

        ImGui::InputText("##label", stringBuffer, sizeof(stringBuffer), ImGuiInputTextFlags_ReadOnly);



        // ERROR CHECKERS -------------------------------------------------------------------------------------------------------------------------------------------------------
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


        if (ImGui::Button("Save/Use ID")) {
            writeToFile << newID << std::endl;
        }



        ImGui::End();
    }



}
