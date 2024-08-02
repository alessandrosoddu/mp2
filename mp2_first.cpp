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
    const int N = 10;  // Number of iterations per frame
    const int frame_size = 1152;  // Frame size for MPEG layer 2
    const int buffer_size = 8192; // Buffer size for output buffer

    std::vector<unsigned char> mp2buffer(buffer_size);
    std::vector<short> audioData = readAudioFile("audio.wav");

    std::ofstream outputFile("output.mp2", std::ios::binary);
    if (!outputFile) {
        std::cerr << "Errore nell'apertura del file di output" << std::endl;
        return 1;
    }

    twolame_options *encoder_options = twolame_init();
    if (!encoder_options) {
        std::cerr << "Errore nell'inizializzazione di TwoLame encoder" << std::endl;
        return 1;
    }

    twolame_set_bitrate(encoder_options, 192);
    twolame_set_in_samplerate(encoder_options, 44100);
    twolame_set_out_samplerate(encoder_options, 44100);
    twolame_set_mode(encoder_options, TWOLAME_STEREO);
    twolame_set_num_channels(encoder_options, 2); // Stereo input

    if (twolame_init_params(encoder_options) != 0) { // Initialize with default parameters
        std::cerr << "Errore nella configurazione dell'encoder TwoLame" << std::endl;
        twolame_close(&encoder_options);
        return 1;
    }

    size_t num_frames = audioData.size() / (frame_size * 2); // Stereo, so frame_size * 2

    for (size_t frame_index = 0; frame_index < num_frames; frame_index++) {
        std::vector<short> audioFrame(frame_size * 2); // Stereo frame
        size_t start = frame_index * frame_size * 2; // Start index for the current frame

        // Copy the frame data
        std::copy(audioData.begin() + start, audioData.begin() + start + frame_size * 2, audioFrame.begin());

        // Perform N iterations on the current frame
        bool watermark_successful = false;
        for (int i = 0; i < N; ++i) {
            std::vector<short> individual = audioFrame;

            // Save the state of the encoder before processing the frame
            twolame_options *saved_opts = save_state(encoder_options);
            if (!saved_opts) {
                twolame_close(&encoder_options);
                return 1;
            }

            // Compress the frame
            int bytes_encoded = twolame_encode_buffer_interleaved(encoder_options, individual.data(), individual.size() / 2, mp2buffer.data(), buffer_size);
            if (bytes_encoded < 0) {
                std::cerr << "Errore di codifica" << std::endl;
                restore_state(encoder_options, saved_opts);
                free_twolame_options(saved_opts);
                continue;
            }

            // Placeholder for decoding and watermark validation
            std::vector<short> decoded_frame(frame_size * 2);
            // decode(mp2buffer, decoded_frame);

            // Quality check and watermark validation (simulated)
            bool compression_acceptable = true; // Simulate the watermark success check

            if (compression_acceptable) {
                outputFile.write(reinterpret_cast<char*>(mp2buffer.data()), bytes_encoded);
                watermark_successful = true;
                free_twolame_options(saved_opts);
                break; // Exit the loop if the watermark is successfully inserted
            } else {
                // If the watermark is not acceptable, restore the encoder state
                restore_state(encoder_options, saved_opts);
                free_twolame_options(saved_opts);
            }
        }

        if (!watermark_successful) {
            std::cerr << "Errore: il watermark non è stato inserito correttamente per il frame " << frame_index << std::endl;
            // Handle the case where no iteration succeeded
        }

        // Encoder state evolves naturally here for the next frame
    }

    free_twolame_options(encoder_options);
    outputFile.close();

    std::cout << "Compressione completata." << std::endl;
    return 0;
}
