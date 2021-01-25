
#pragma once

#include "Core.h"
#include "AudioInputDevice.h"
#include "AudioOutputDevice.h"

class IOEXP IO
{
public:
    IO();
    ~IO() = default;

public:
    void Play();
    const std::vector<std::wstring> GetInputDevices() const;
    const std::vector<std::wstring> GetOutputDevices() const;
    void SetInputAudioDevice(const std::wstring& _name);
    void SetOutputAudioDevice(const std::wstring& _name);

    size_t NextBlock(size_t _uBlock) const 
    {
        size_t newBlock = _uBlock + 1;

        if (newBlock == uNumBlocks)
            return 0;
        else
            return newBlock;
    };

private:
    std::mutex m_mux;
    std::condition_variable m_cvIn;
    std::condition_variable m_cvOut;
    EngineState m_EngineState;
    std::unique_ptr<AudioInputDevice> m_upInputDevice;
    std::unique_ptr<AudioOutputDevice> m_upOutputDevice;
    std::atomic<size_t> m_auBlocksFree;

private:
    // Handler for soundcard request for more data
    void waveInProc(HWAVEIN hWaveIn,
                    UINT uMsg,
                    DWORD dwParam1,
                    DWORD dwParam2);
                                         
    // Static wrapper for sound card handler
    static void CALLBACK waveInProcWrap(HWAVEIN hWaveIn,
                                         UINT uMsg,
                                         DWORD dwInstance,
                                         DWORD dwParam1,
                                         DWORD dwParam2);
                                         
    // Handler for soundcard request for more data
    void waveOutProc(HWAVEOUT hWaveOut,
                                         UINT uMsg,
                                         DWORD dwParam1,
                                         DWORD dwParam2);
                                         
    // Static wrapper for sound card handler
    static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut,
                                         UINT uMsg,
                                         DWORD dwInstance,
                                         DWORD dwParam1,
                                         DWORD dwParam2);
};

