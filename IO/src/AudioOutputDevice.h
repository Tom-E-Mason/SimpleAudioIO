
#pragma once

#include "Core.h"

class AudioOutputDevice
{
public:
    AudioOutputDevice() = delete;
    AudioOutputDevice(std::wstring _wstrName, const EngineState* _state);
    ~AudioOutputDevice() = default;

public:
    const std::vector<std::wstring> GetOutputDevices() const;
    bool Open(const IO* _instance);
    void Write(size_t _uCurrentBlock);
    void CleanUp(const size_t _uCurrentBlock);
    void SetCallback(void(CALLBACK* _func)(HWAVEOUT,
                                           UINT,
                                           DWORD,
                                           DWORD,
                                           DWORD)) {
        m_CallbackFunction = _func;
    };
    
private:
    std::wstring m_wstrName;
    HWAVEOUT m_hwDevice = 0;
    const EngineState* m_State = nullptr;

    SampleType* m_pBuffer = nullptr;
    std::array<WAVEHDR, uNumBlocks> m_arrBufferHeaders = { 0 };

    void(CALLBACK* m_CallbackFunction)(HWAVEOUT,
                                       UINT,
                                       DWORD,
                                       DWORD,
                                       DWORD) = nullptr;
};

