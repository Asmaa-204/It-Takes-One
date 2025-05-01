#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace our {
    class SoundSystem {
    private:
        ALCdevice* device;
        ALCcontext* context;
        std::map<std::string, ALuint> soundBuffers;
        std::vector<ALuint> sources;

    public:
        SoundSystem() {
            // Initialize OpenAL
            device = alcOpenDevice(nullptr);
            if (!device) {
                std::cerr << "Failed to open OpenAL device!" << std::endl;
                return;
            }
            
            context = alcCreateContext(device, nullptr);
            if (!context) {
                std::cerr << "Failed to create OpenAL context!" << std::endl;
                return;
            }
            
            alcMakeContextCurrent(context);
        }

        ~SoundSystem() {
            // Cleanup sources and buffers
            for (ALuint source : sources) {
                alDeleteSources(1, &source);
            }
            for (auto& buffer : soundBuffers) {
                alDeleteBuffers(1, &buffer.second);
            }
            
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
            alcCloseDevice(device);
        }

        void loadSound(const std::string& name, const std::string& filepath) {
            // Create OpenAL buffer
            ALuint buffer;
            alGenBuffers(1, &buffer);

            // Open audio file
            SF_INFO sfinfo;
            SNDFILE* sndfile = sf_open(filepath.c_str(), SFM_READ, &sfinfo);
            if (!sndfile) {
                std::cerr << "Could not open audio file: " << filepath << std::endl;
                return;
            }

            // Read the audio data
            std::vector<short> samples(sfinfo.frames * sfinfo.channels);
            sf_read_short(sndfile, samples.data(), samples.size());
            sf_close(sndfile);

            // Figure out the format
            ALenum format;
            if (sfinfo.channels == 1)
                format = AL_FORMAT_MONO16;
            else if (sfinfo.channels == 2)
                format = AL_FORMAT_STEREO16;
            else {
                std::cerr << "Unsupported channel count: " << sfinfo.channels << std::endl;
                return;
            }

            // Upload audio data to OpenAL buffer
            alBufferData(buffer, format, samples.data(), 
                        samples.size() * sizeof(short), sfinfo.samplerate);

            // Store the buffer
            soundBuffers[name] = buffer;
        }

        void playSound(const std::string& name) {
            auto it = soundBuffers.find(name);
            if (it == soundBuffers.end()) {
                std::cerr << "Sound not found: " << name << std::endl;
                return;
            }

            // Create and configure sound source
            ALuint source;
            alGenSources(1, &source);
            sources.push_back(source);

            // Set source properties
            alSourcef(source, AL_PITCH, 1.0f);
            alSourcef(source, AL_GAIN, 1.0f);
            alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
            alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            alSourcei(source, AL_LOOPING, AL_FALSE);

            // Attach buffer and play
            alSourcei(source, AL_BUFFER, it->second);
            alSourcePlay(source);
        }
    };
}