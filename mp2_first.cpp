#define new new_
#include "twolame.h"
#include "common.h"
#undef new

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

void options_copy(twolame_options *dest, const twolame_options *src) {
    std::memcpy(dest, src, sizeof(twolame_options));

    if (src->subband) {
        dest->subband = static_cast<subband_t*>(malloc(sizeof(subband_t)));
        std::memcpy(dest->subband, src->subband, sizeof(subband_t));
    }
    if (src->j_sample) {
        dest->j_sample = static_cast<jsb_sample_t*>(malloc(sizeof(jsb_sample_t)));
        std::memcpy(dest->j_sample, src->j_sample, sizeof(jsb_sample_t));
    }
    if (src->sb_sample) {
        dest->sb_sample = static_cast<sb_sample_t*>(malloc(sizeof(sb_sample_t)));
        std::memcpy(dest->sb_sample, src->sb_sample, sizeof(sb_sample_t));
    }
    if (src->p0mem) {
        dest->p0mem = static_cast<psycho_0_mem*>(malloc(sizeof(psycho_0_mem)));
        std::memcpy(dest->p0mem, src->p0mem, sizeof(psycho_0_mem));
    }
    if (src->p1mem) {
        dest->p1mem = static_cast<psycho_1_mem*>(malloc(sizeof(psycho_1_mem)));
        std::memcpy(dest->p1mem, src->p1mem, sizeof(psycho_1_mem));
    }
    if (src->p2mem) {
        dest->p2mem = static_cast<psycho_2_mem*>(malloc(sizeof(psycho_2_mem)));
        std::memcpy(dest->p2mem, src->p2mem, sizeof(psycho_2_mem));
    }
    if (src->p3mem) {
        dest->p3mem = static_cast<psycho_3_mem*>(malloc(sizeof(psycho_3_mem)));
        std::memcpy(dest->p3mem, src->p3mem, sizeof(psycho_3_mem));
    }
    if (src->p4mem) {
        dest->p4mem = static_cast<psycho_4_mem*>(malloc(sizeof(psycho_4_mem)));
        std::memcpy(dest->p4mem, src->p4mem, sizeof(psycho_4_mem));
    }
}

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

twolame_options* save_state(const twolame_options *opts) {
    twolame_options *saved_opts = twolame_init();
    if (!saved_opts) {
        std::cerr << "Errore di inizializzazione di TwoLame" << std::endl;
        return nullptr;
    }
    options_copy(saved_opts, opts);
    return saved_opts;
}

void restore_state(twolame_options *opts, const twolame_options *saved_opts) {
    options_copy(opts, saved_opts);
}

std::vector<short> readAudioFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Errore nell'apertura del file audio.");
    }
    std::vector<short> audioData((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
    return audioData;
}

int main() {
    const int N = 10;  //Number of iteration
    const int frame_size = 1152;  // Frame size for MPEG layer 2
    const int buffer_size = 8192; // Frame size for output buffer

    // Buffer audio
    //std::vector<short> audio_buffer(frame_size);
    //std::vector<unsigned char> mp2buffer(buffer_size);

    std::vector<unsigned char> mp2buffer(buffer_size);
    std::vector<short> audioData = readAudioFile("audio.wav");

    twolame_options *encoder_options = twolame_init();
    if (!encoder_options) {
        std::cerr << "Errore nell'inizializzazione di TwoLame encoder" << std::endl;
        return 1;
    }

    twolame_set_bitrate(encoder_options, 128);
    twolame_set_in_samplerate(encoder_options, 44100);
    twolame_set_out_samplerate(encoder_options, 44100);
    twolame_set_mode(encoder_options, TWOLAME_MONO);
    twolame_set_num_channels(encoder_options, 1); // Specifica il numero di canali di input

    if (twolame_init_params(encoder_options) != 0) { // Inizializza i parametri con valori di default
        std::cerr << "Errore nella configurazione dell'encoder TwoLame" << std::endl;
        twolame_close(&encoder_options);
        return 1;
    }

    std::vector<short> audioFrame(frame_size);
    std::copy(audioData.begin(), audioData.begin() + frame_size, audioFrame.begin());

    //Read audio
    //std::string filename = "audio.wav";
    //std::vector<short> audioData = readAudioFile(filename);

    //Print some file audio data
    std::cout << "Il file audio contiene " << audioData.size() << " campioni." << std::endl;
    for (size_t i = 0; i < 100 && i < audioData.size(); ++i) {
        std::cout << audioData[i] << " ";
    }
    std::cout << std::endl;

    //Genetic Algorithm here..

    // Iteration
    for (int i = 0; i < N; ++i) {
        std::vector<short> individual = audioFrame;

        if (audioData.size() == 0) {
            break; // Fine del file
        }

        // 3a. Save the state before each iteration
        twolame_options *saved_opts = save_state(encoder_options);
        if (!saved_opts) {
            twolame_close(&encoder_options);
            return 1;
        }

        // Compress frame
        int bytes_encoded = twolame_encode_buffer(encoder_options, individual.data(), nullptr, individual.size(), mp2buffer.data(), buffer_size);
        if (bytes_encoded < 0) {
            std::cerr << "Errore di codifica" << std::endl;
            restore_state(encoder_options, saved_opts);
            free_twolame_options(saved_opts);
            continue;
        }

        std::vector<short> decoded_frame(frame_size);
        // Placeholder per la funzione di decodifica
        // decode(mp2buffer, decoded_frame);

        //quality check
        bool compression_acceptable = true;

        if (!compression_acceptable) {
            restore_state(encoder_options, saved_opts);
        }
        free_twolame_options(saved_opts);
    }
    // Libera le opzioni dell'encoder principale
    free_twolame_options(encoder_options);

    return 0;
}
