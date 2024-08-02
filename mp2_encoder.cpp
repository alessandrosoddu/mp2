#define new new_
#include "twolame.h"
#include "common.h"
#undef new
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

#define BUFFER_SIZE 1152
#define CHANNELS 2
#define IN_SAMPLERATE 44100
#define OUT_SAMPLERATE 44100
#define BITRATE 192
#define ITERATION 10

using std::cout;
using std::vector;
using std::string;
using std::ofstream;
using std::ifstream;
using std::cerr;
using std::endl;

//Encode an input audio file in a output audio file
void encoder(const char* input_file, const char* output_file){

    //open the input file
    ifstream input(input_file, std::ios::binary);
    if(!input){
        cerr << "Error: Unable to open the input file" << endl;
    }

    //initialize the encoder
    twolame_options* options = twolame_init();

    //Set parameters
    twolame_set_num_channels(options, CHANNELS);  // stereo
    twolame_set_in_samplerate(options, IN_SAMPLERATE); // campionamento a 44.1kHz
    twolame_set_out_samplerate(options, OUT_SAMPLERATE);
    twolame_set_bitrate(options, 192);  // 192 kbps
    twolame_set_mode(options, TWOLAME_MONO);

    //initialize the encoder with parameters
    if(twolame_init_params(options) != 0){
        cerr << "Error: Unable to initialize parameters" << endl;
        twolame_close(&options);
    }

    //open the output file
    ofstream output(output_file, std::ios::binary);
    if(!output){
        cerr << "Error: Unable to open the output file" << endl;
        twolame_close(&options);
    }

    short int pcm_input_buffer[BUFFER_SIZE * 2]; //pcm samples
    unsigned char output_buffer[BUFFER_SIZE * 2];

    //read from input file, encode and write in output file
    while(input.read(reinterpret_cast<char*>(pcm_input_buffer), sizeof(pcm_input_buffer))){
        int num_samples = input.gcount() / sizeof(short int); //16 bits
        int num_bytes = twolame_encode_buffer_interleaved(
            options, 
            pcm_input_buffer, 
            num_samples / 2, 
            output_buffer, 
            sizeof(output_buffer)
            );
        
        if(num_bytes < 0){
            cerr << "Error: Encoding error occured" <<endl;
        }
        output.write(reinterpret_cast<char*>(output_buffer), num_bytes);
    }

    input.close();
    output.close();
    twolame_close(&options);
    cout << "Compression complete!" << endl;
}

//Encode ITERATION time a frame using the same state
void frame_encoder(twolame_options state /*frame*/){

}

int main(int argc, char* argv[]){
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input wav file> <output mp2 file>" << std::endl;
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    encoder(input_file, output_file);
}

