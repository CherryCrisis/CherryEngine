#include <pch.hpp>

#include <sound.hpp>
#include "resource_manager.hpp"
#include "al.h"
#include "sound_manager.hpp"

static int wavLoad(const char* filename, int* channels, int* sampleRate, unsigned int* size, int* bps, short** output)
{
    typedef struct WavRIFFHeader
    {
        char chunkId[4];
        unsigned int chunkSize;
        char format[4];
    } WavRIFFHeader;

    typedef struct WavFmtChunk
    {
        char chunkId[4];
        unsigned int chunkSize;
        short audioFormat; // PCM = 1
        short numChannels;
        int sampleRate;
        int byteRate;
        short blockAlign;
        short bitsPerSample;
    } WavFmtChunk;

    typedef struct WavDataChunk
    {
        char chunkId[4];
        unsigned int chunkSize;
        //char data[];
    } WavDataChunk;

    FILE* wav;
    fopen_s(&wav, filename, "rb");

    if (wav == NULL)
        return -1;

    WavRIFFHeader header;
    fread(&header, sizeof(WavRIFFHeader), 1, wav);
    if (strncmp(header.chunkId, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0)
    {
        fclose(wav);
        fprintf(stderr, "WAVE: Unsupported wav file\n");
        return -1;
    }

    WavFmtChunk format;
    fread(&format, sizeof(WavFmtChunk), 1, wav);
    if (format.audioFormat != 1 || format.bitsPerSample != 16)
    {
        fclose(wav);
        fprintf(stderr, "WAVE: Formats other than 16 bits PCM are not supported\n");
        return -1;
    }

    WavDataChunk data;
    fread(&data, sizeof(WavDataChunk), 1, wav);

    short* buffer = (short*)calloc(1, data.chunkSize);
    fread(buffer, sizeof(unsigned char), data.chunkSize, wav);
    fclose(wav);

    *bps = format.bitsPerSample;
    *size = data.chunkSize;
    *channels = format.numChannels;
    *sampleRate = format.sampleRate;
    *output = buffer;

    return data.chunkSize / format.numChannels / (format.bitsPerSample / 8);
}

void Sound::Load(std::shared_ptr<Sound> sound)
{
    alGenBuffers(1, &sound->m_buffer);

    short* buffer;
    int channels;
    int sampleRate;
    int bps;
    int frameCount = -1;
    unsigned int size = 0u;
    unsigned int format;

    frameCount = wavLoad(sound->m_filepath.string().c_str(), &channels, &sampleRate, &size, &bps, &buffer);

    if (frameCount == -1)
    {
        // TODO: Replace by log
        std::cout << "Error while loading sound" << std::endl;
        return;
    }

    short* monoBuffer = nullptr;
    format = bps == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;

    if (channels > 1)
    {
        monoBuffer = (short*)malloc(frameCount * sizeof(short));
        for (int i = 0; i < frameCount; i++)
            monoBuffer[i] = static_cast<short>(.5f * buffer[i * 2]) + static_cast<short>(.5f * buffer[i * 2 + 1]);
    }

    alBufferData(sound->m_buffer, format, channels == 1 ? buffer : monoBuffer, channels == 1 ? (ALsizei)size : (ALsizei)(size * .5f), (ALsizei)sampleRate);
    free(buffer);
    free(monoBuffer);

    alGenSources(1, &(sound->m_source));
    alSourcei(sound->m_source, AL_BUFFER, sound->m_buffer);
}

void Sound::SetLooping(bool loop) { alSourcei(m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE); }
void Sound::SetPosition(float x, float y, float z) { alSource3f(m_source, AL_POSITION, x, y, z); }
void Sound::SetPosition(const CCMaths::Vector3& pos) { SetPosition(pos.x, pos.y, pos.z); }
void Sound::SetDirection(const CCMaths::Vector3& fwd) { alSource3f(m_source, AL_DIRECTION, fwd.x, fwd.y, fwd.z); }
void Sound::SetPitch(float pitch) { alSourcef(m_source, AL_PITCH, pitch); }
void Sound::SetRollOff(float rolloff) { alSourcef(m_source, AL_ROLLOFF_FACTOR, rolloff); }
void Sound::SetReferenceDistance(float dst) { alSourcef(m_source, AL_REFERENCE_DISTANCE, dst); }

void Sound::Play() { alSourcePlay(m_source); };
void Sound::Pause() { alSourcePause(m_source); };
void Sound::Stop() { alSourceStop(m_source); };

void Sound::Delete() 
{
    alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);
}
