// INCLUDES
#include <iostream>
#include <fstream>
#include <string>

#include <iomanip> // For std::setw and std::setfill
#include <sstream> // For std::stringstream

#include <cstdlib>

std::string getLastLine(const std::string& filePath); 

/* FUNCTION MAIN -----------------------------------------------------------------------------------------------------
*   
*   create a file that is used to read (readCustomerFile) and one to edit (customerOutputFile)
* 
* 
* CHECKERS:
*   1. check if IDs are 8 characters in length
*   2. check if IDs consists of positive integers only
*   3. check if ID created is unique
*/

int main() {

    // get the file path
    std::string filePath = "C:/Users/tommy/Desktop/DOMA Customer IDs/CustomerIDs.txt";

    // Create an output file stream object
    std::ofstream customerOutputFile(filePath, std::ios::app); 

    // Create an input file stream object
    std::ifstream readCustomerFile(filePath);


    // ERROR CHECKERS ------------------------------------------------------------------------------------------------------------------------------------------------
    // Check if the output file was opened successfully
    if (!customerOutputFile.is_open()) {
        std::cerr << "Error: Could not open editing file!" << std::endl;
        return -1; // Exit with an error code
    }

    // Check if the input file was opened successfully
    if (!readCustomerFile.is_open()) {
        std::cerr << "Error: Could not open reading file!" << std::endl;
        return -1; // Exit with an error code
    }
    // -------------------------------------------------------------------------------------------------------------------------------------------------------




    // Read the file line by line and compare IDs
    std::string line;

    // develop GUI that displays newID when button is pressed SOON
    // get a new ID based on last logged ID (IDs are created by incrementing last ID by 1)
    std::string newID = getLastLine(filePath);
    std::cout << newID << std::endl;




    // ERROR CHECKERS -------------------------------------------------------------------------------------------------------------------------------------------------------
    // iterate through file checking for errors
    while (std::getline(readCustomerFile, line)) {

        if (line.size() > 8 || line.size() < 8) {
            std::cout << "FILE ERROR:: ID INVALID LENGTH" << std::endl;
            return -1;
        }

        // if ID exists in file throw error
        if (line == newID) {
            std::cout << "FILE ERROR:: ENSURE FILE IS ORGANIZED FROM SMALLEST TO LARGEST" << std::endl;
            return -1;
        }

    }
    // -------------------------------------------------------------------------------------------------------------------------------------------------------


    // create a button that offically stores newID into file
    // update list of customer IDs by adding new ID to end of file
    customerOutputFile << newID << std::endl;

    // Close the files
    readCustomerFile.close();
    customerOutputFile.close();

	return 0;
}


/* FUNCTION getLastLine() ------------------------------------------------------------------------------------------------------
*   Parameters:
*       const std::string& file path
* 
*   this function simply gets the last line in the file
*   converts it to integer, develops next ID
*   formats and returns new ID
*/
std::string getLastLine(const std::string& filePath) {
    std::ifstream readFile(filePath);

    std::string getLastLine;
    std::string lastLine;


    // get last line in file
    while (std::getline(readFile, getLastLine)) { 
        if (!getLastLine.empty())
            lastLine = getLastLine;
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





// develop user interface that generates the new ID after clicking "generate", and display this new ID

// develop a button "use" that saves this ID onto file







/* NOTES:
*   1. program works on empty files
*   2. correctly throws error when IDs are incorrect length or already exists
*   3. ASSUMES FILE IS EITHER EMPTY
*   4. ONLY WORKS IF IDS ARE IN ORDER IN FILE FROM SMALLEST TO LARGEST
*/