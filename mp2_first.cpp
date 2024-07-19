#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>
#include <twolame.h>

// 1. Frame Preparation

// 1a. Read Audio File
std::vector<short> readAudioFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary); //open the file in binary mode
    if (!file) {
        throw std::runtime_error("Errore nell'apertura del file audio.");
    }
    std::vector<short> audioData((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
    return audioData;
}

// 1b. Load frame
// 1c. Save initial state of the compressor

// 2. Watermarking functions

// 3. Compressor state
// 3a. Save the state before each iteration
// 3b. Restore the state if necessary