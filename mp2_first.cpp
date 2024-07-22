#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>
#include <twolame.h>   
#include "common.h"


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

// 1b. Save initial state of the compressor

// 2. Watermarking functions

// 3. Compressor state
// 3a. Save the state before each iteration
// 3b. Restore the state if necessary

int main() {
    twolame_options encoder; //twolame_options is type twolame_options_struct
    encoder.samplerate_in = 44100;
    encoder.samplerate_out = 44100;
    encoder.num_channels_in = 2;
    encoder.num_channels_out = 2;
    //encoder.subband = new subband_t;
    
    encoder.p0mem = new psycho_0_mem;
    encoder.p1mem = new psycho_1_mem;
    encoder.p2mem = new psycho_2_mem;
    encoder.p3mem = new psycho_3_mem;
    encoder.p4mem = new psycho_4_mem;

    //std::vector<uint8_t> state;
    //serialize(encoder, state);

    twolame_options newEncoder;
    newEncoder.subband = nullptr;
    newEncoder.p0mem = nullptr;
    newEncoder.p1mem = nullptr;
    newEncoder.p2mem = nullptr;
    newEncoder.p3mem = nullptr;
    newEncoder.p4mem = nullptr;
    //deserialize(state, newEncoder);

    std::cout << "samplerate_in: " << newEncoder.samplerate_in << ", samplerate_out: " << newEncoder.samplerate_out << std::endl;

    delete encoder.subband;
    delete encoder.p0mem;
    delete encoder.p1mem;
    delete encoder.p2mem;
    delete encoder.p3mem;
    delete encoder.p4mem;
    delete newEncoder.subband;
    delete newEncoder.p0mem;
    delete newEncoder.p1mem;
    delete newEncoder.p2mem;
    delete newEncoder.p3mem;
    delete newEncoder.p4mem;

    std::string filename = "audio.wav";
    std::vector<short> audioData = readAudioFile(filename);

    // Print some file audio data
    std::cout << "Il file audio contiene " << audioData.size() << " campioni." << std::endl;
    for (size_t i = 0; i < 100 && i < audioData.size(); ++i) {
        std::cout << audioData[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}