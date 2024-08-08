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
using std::memcpy;

//Save the encoder state options
void options_copy(twolame_options *dest, const twolame_options *src) {
    memcpy(dest, src, sizeof(twolame_options));

    if (src->subband) {
        dest->subband = static_cast<subband_t*>(malloc(sizeof(subband_t)));
        memcpy(dest->subband, src->subband, sizeof(subband_t));
    }
    if (src->j_sample) {
        dest->j_sample = static_cast<jsb_sample_t*>(malloc(sizeof(jsb_sample_t)));
        memcpy(dest->j_sample, src->j_sample, sizeof(jsb_sample_t));
    }
    if (src->sb_sample) {
        dest->sb_sample = static_cast<sb_sample_t*>(malloc(sizeof(sb_sample_t)));
        memcpy(dest->sb_sample, src->sb_sample, sizeof(sb_sample_t));
    }
    if (src->p0mem) {
        dest->p0mem = static_cast<psycho_0_mem*>(malloc(sizeof(psycho_0_mem)));
        memcpy(dest->p0mem, src->p0mem, sizeof(psycho_0_mem));
    }
    if (src->p1mem) {
        dest->p1mem = static_cast<psycho_1_mem*>(malloc(sizeof(psycho_1_mem)));
        memcpy(dest->p1mem, src->p1mem, sizeof(psycho_1_mem));
    }
    if (src->p2mem) {
        dest->p2mem = static_cast<psycho_2_mem*>(malloc(sizeof(psycho_2_mem)));
        memcpy(dest->p2mem, src->p2mem, sizeof(psycho_2_mem));
    }
    if (src->p3mem) {
        dest->p3mem = static_cast<psycho_3_mem*>(malloc(sizeof(psycho_3_mem)));
        memcpy(dest->p3mem, src->p3mem, sizeof(psycho_3_mem));
    }
    if (src->p4mem) {
        dest->p4mem = static_cast<psycho_4_mem*>(malloc(sizeof(psycho_4_mem)));
        memcpy(dest->p4mem, src->p4mem, sizeof(psycho_4_mem));
    }
}

//Free the memory and the encoder state options
void free_twolame_options(twolame_options *opts) {
    if (opts->subband) {
        free(opts->subband);
        opts->subband = nullptr;
    }
    if (opts->j_sample) {
        free(opts->j_sample);
        opts->j_sample = nullptr;
    }
    if (opts->sb_sample) {
        free(opts->sb_sample);
        opts->sb_sample = nullptr;
    }
    if (opts->p0mem) {
        free(opts->p0mem);
        opts->p0mem = nullptr;
    }
    if (opts->p1mem) {
        free(opts->p1mem);
        opts->p1mem = nullptr;
    }
    if (opts->p2mem) {
        free(opts->p2mem);
        opts->p2mem = nullptr;
    }
    if (opts->p3mem) {
        free(opts->p3mem);
        opts->p3mem = nullptr;
    }
    if (opts->p4mem) {
        free(opts->p4mem);
        opts->p4mem = nullptr;
    }
    twolame_close(&opts);
}

//Save the encoder state option
twolame_options* save_state(const twolame_options *opts) {
    twolame_options *saved_opts = twolame_init();
    if (!saved_opts) {
        cerr << "Errore di inizializzazione di TwoLame" << endl;
        return nullptr;
    }
    options_copy(saved_opts, opts);
    return saved_opts;
}

void restore_state(twolame_options *opts, const twolame_options *saved_opts) {
    options_copy(opts, saved_opts);
}

int watermark_embedding(){
    return rand() % ITERATION;
}

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
    twolame_set_in_samplerate(options, IN_SAMPLERATE); // sampling at 44.1kHz
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
void frame_encoder(const char* input_file, const char* output_file){

    // Open the input file
    ifstream input(input_file, std::ios::binary);
    if (!input) {
        cerr << "Error: Unable to open the input file" << endl;
        return;
    }

    // Initialize the encoder
    twolame_options* options = twolame_init();

    // Set standard parameters
    twolame_set_num_channels(options, CHANNELS);  // stereo
    twolame_set_in_samplerate(options, IN_SAMPLERATE); // sampling at 44.1kHz
    twolame_set_out_samplerate(options, OUT_SAMPLERATE);
    twolame_set_bitrate(options, BITRATE);  // 192 kbps
    twolame_set_mode(options, TWOLAME_MONO);

    // Initialize the encoder with all the parameters
    if (twolame_init_params(options) != 0) {
        cerr << "Error: Unable to initialize parameters" << endl;
        twolame_close(&options);
        return;
    }

    // Open the output file
    ofstream output(output_file, std::ios::binary);
    if (!output) {
        cerr << "Error: Unable to open the output file" << endl;
        twolame_close(&options);
        return;
    }

    // Read and encode only one frame
    short int pcm_input_buffer[BUFFER_SIZE * CHANNELS]; // PCM samples (interleaved for stereo)
    unsigned char output_buffer[BUFFER_SIZE * CHANNELS];

    // Read one frame from input file
    while (input.read(reinterpret_cast<char*>(pcm_input_buffer), sizeof(pcm_input_buffer))) {

        int watermark = watermark_embedding();
        for(int i = 0; i < ITERATION; i++){

            // Save the state of the encoder before processing the frame
            twolame_options *saved_opts = save_state(options);
            if (!saved_opts) {
                twolame_close(&options);
            }

            /*if(ITERATION == 0){
                twolame_options *saved_opts = save_state(options);
            }*/

            int num_samples = input.gcount() / sizeof(short int); // number of samples in the buffer
            int num_bytes = twolame_encode_buffer_interleaved(
                options, 
                pcm_input_buffer, 
                num_samples / CHANNELS, 
                output_buffer, 
                sizeof(output_buffer)
            );

            if (num_bytes < 0) {
             cerr << "Error: Encoding error occurred" << endl;
            } 
            // Write to output file only when watermark is successfully embedded
            if (i == watermark) {
                output.write(reinterpret_cast<char*>(output_buffer), num_bytes);
                //cout << "Watermarked in: " << watermark << " iteration. Frame compressed and written to output file!" << endl;
                free_twolame_options(saved_opts);
                break;
            }
            else {
                restore_state(options, saved_opts);
                free_twolame_options(saved_opts);
                //cout << "Iteration.." << endl;
            }
        }
    } 
    // Close the files and clean up
    //free_twolame_options(options);
    input.close();
    output.close();
    twolame_close(&options);
}

int main(int argc, char* argv[]){
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input wav file> <output mp2 file>" << std::endl;
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    //encoder(input_file, output_file);
    frame_encoder(input_file, output_file);
}

