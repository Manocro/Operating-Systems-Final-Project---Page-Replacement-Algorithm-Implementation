// Name: Manuel Edwardo De La Rosa M- Nhan Bui.
// Course: CSE 4300/5300
// Final Project Operating Systems Page Replacement Algorithms

#include <iostream>
#include <fstream>  
#include <string>    

using namespace std; 

int main() {
    string filename;
    //ask for the input file. 
    cout << "Enter the input file name: ";
    cin >> filename;

    // Create an input file stream object and open the specified file
    ifstream file(filename);

    // Check if the file failed to open 
    if (!file.is_open()) {
        // Print an error message if the file could not be opened and exit.
        cout << "Error: Could not open file " << filename << endl;
        return 1;
    }

    // Declare a variable to store the algorithm choice ('O' for OPT, 'F' for FIFO)
    char algo;
    file >> algo;

    // Declare a temporary character variable to catch and discard the commas in the file
    char comma;
    file >> comma;

    // Declare an integer to store the number of memory frames dynamically
    int num_frames;
    // Read the number of memory frames from the file
    file >> num_frames;

    // Read and discard the second comma after the number of frames
    file >> comma;

    // Save the current position in the file so we can rewind later
    streampos pos = file.tellg();

    // Declare a counter to figure out how long the reference string is
    int ref_count = 0;
    // Declare a temporary integer to read the reference numbers one by one
    int temp_val;

    // Loop through the rest of the file to count the reference integers
    while (file >> temp_val) {
        // Increment the count for every number successfully read
        ref_count++;
        //now the comma.
        file >> comma;
    }
    //Clear the file stream.
    file.clear();

    // Seek back to the saved position right before the reference string started
    file.seekg(pos);

    // Dynamically allocate a basic array to hold the reference string based on our count
    int* ref_string = new int[ref_count];

    // Loop through the exact number of elements we just counted
    for (int i = 0; i < ref_count; i++) {
        // Read the actual integer value into our newly allocated array
        file >> ref_string[i];
        //then the comma.
        file >> comma;
    }
    //When done reading, close the file.
    file.close();

    // Dynamically allocate a 1D array to represent the current memory frames
    int* frames = new int[num_frames];

    // Initialize all memory frames to -1 to represent that they are currently empty
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }

	// Dynamically allocate a 2D array to record the history of memory states for printing at the end
    int** history = new int* [num_frames];

    // Loop through each frame row to allocate the columns (the length of the reference string)
    for (int i = 0; i < num_frames; i++) {
        history[i] = new int[ref_count];
    }

    // Declare an integer to track the total number of page faults
    int faults = 0;

    // Declare a pointer variable used strictly for the FIFO algorithm to track the oldest page
    int fifo_pointer = 0;

    // Start iterating through the entire reference string one by one
    for (int i = 0; i < ref_count; i++) {
        // Extract the current page requested from the reference string
        int current_page = ref_string[i];

		// track if the page is already in memory or not
        bool is_hit = false;

        // Loop through all current memory frames to check for the requested page
        for (int j = 0; j < num_frames; j++) {
			// If the frame holds the requested page we set the flag to true and break out of the loop early since we found a hit
            if (frames[j] == current_page) {
                is_hit = true;
                break;
            }
        }

        // Check if the page request resulted in a hit
        if (is_hit) {
            // If it's a hit, record -1 in the history array for this specific column
            // -1 will tell our printing logic to leave the column completely blank
            for (int j = 0; j < num_frames; j++) {
                history[j][i] = -1;
            }
        }
        // If the page request resulted in a fault (not found in memory)
        else {
            // Increment the page fault counter since we missed
            faults++;

            // Declare a variable to store the index of the frame we need to replace
            int replace_idx = -1;

            // Loop through the frames to see if there is an empty slot (-1) available
            for (int j = 0; j < num_frames; j++) {
                // If we find an empty frame, use it immediately
                if (frames[j] == -1) {
                    // Record the index of the empty frame
                    replace_idx = j;
                    break;
                }
            }

            // If there were no empty frames available, we must use our chosen replacement algorithm
            if (replace_idx == -1) {
                // Check if the user selected the FIFO algorithm ('F')
                if (algo == 'F') {
                    // Set the replacement index to the current FIFO pointer (oldest page)
                    replace_idx = fifo_pointer;
                    // Move the FIFO pointer forward, wrapping around using modulo arithmetic
                    fifo_pointer = (fifo_pointer + 1) % num_frames;
                }
                // Check if the user selected the Optimal algorithm ('O' or 'OPT')
                else if (algo == 'O') {
                    // Declare a variable to keep track of the page that is used furthest in the future
                    int max_next_use = -1;

                    // Loop through all current frames to evaluate which one is the best to replace
                    for (int j = 0; j < num_frames; j++) {
                        // Assume the page is never used again (sets the distance to beyond reference length)
                        int next_use = ref_count;

                        // Look forward in the reference string starting from the very next request
                        for (int k = i + 1; k < ref_count; k++) {
                            // If we find the frame's page used in the future
                            if (ref_string[k] == frames[j]) {
                                // Record the index of its next occurrence
                                next_use = k;
                                // Stop looking forward for this specific frame
                                break;
                            }
                        }

                        // If this frame's next usage is further away than our current maximum
                        if (next_use > max_next_use) {
                            // Update the maximum furthest distance found
                            max_next_use = next_use;
                            // Update the replacement index to point to this frame
                            replace_idx = j;
                        }
                    }
                }
                // Check if the user selected the LRU Algorithm( 'L' )
                else if (algo == 'L') {
                    int least_recent = ref_count;

                    for (int j = 0; j < num_frames; j++) {
                        int last_use = -1;

                        for (int k = i - 1; k >= 0; k--) {
                            if (ref_string[k] == frames[j]) {
                                last_use = k;
                                break;
            }
        }

        if (last_use < least_recent) {
            least_recent = last_use;
            replace_idx = j;
        }
    }
}
            }

            // Replace the chosen frame with the newly requested page
            frames[replace_idx] = current_page;

            // Copy the new state of the frames into the history matrix for this specific column
            for (int j = 0; j < num_frames; j++) {
                history[j][i] = frames[j];
            }
        }
    }


    // --- Output Formatting ---

     // 1. Top row: Print the reference string with spacing
    for (int i = 0; i < ref_count; i++) {
        // Print the number followed by 3 spaces to create a clean column
        cout << ref_string[i] << "   ";
    }
    cout << endl;

    // 2. Dashed line: Print a separator that matches the width of the columns
    for (int i = 0; i < ref_count; i++) {
        // Print 4 dashes per column to match the 4-character width of the numbers + spaces
        cout << "----";
    }
    cout << endl;

    // 3. Table rows: Print frame history, leaving hits completely blank
    for (int i = 0; i < num_frames; i++) {
        for (int j = 0; j < ref_count; j++) {
            // If it's a page fault (a number is stored), print the number and spaces
            if (history[i][j] != -1) {
                cout << history[i][j] << "   ";
            }
            // If it's a page hit (-1 is stored), print exactly 4 spaces to leave it blank
            else {
                cout << "    ";
            }
        }
        cout << endl;
    }

    // Print a blank line, then the total faults
    cout << endl << "Total Page Faults: " << faults << endl;


    /*
    // --- CSV FORMAT - Output Formatting Section ---

    // Print the top row consisting of the reference string values
    for (int i = 0; i < ref_count; i++) {
        cout << "\"" << ref_string[i] << " \"";
        if (i < ref_count - 1) {
            cout << ",";
        }
    }

    cout << endl;

    // Loop through each frame to print the history table row by row
    for (int i = 0; i < num_frames; i++) {
        // Loop through each step (column) in the reference string
        for (int j = 0; j < ref_count; j++) {
            // Check if the history recorded a frame state (not -1 for a hit)
            if (history[i][j] != -1) {
                // If it was a fault, print the value stored in the frame enclosed in quotes
                cout << "\"" << history[i][j] << " \"";
            }
            cout << ",";
        }
        cout << endl;
    }
    cout << endl;
    // Output the total calculated number of page faults
    cout << "Total Page Faults: " << faults << endl;

    delete[] ref_string;
    delete[] frames;

    // Loop through each row of the 2D history array to delete the columns
    for (int i = 0; i < num_frames; i++) {
        delete[] history[i];
    }
    delete[] history;

    return 0;



    */
}