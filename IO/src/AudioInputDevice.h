
#pragma once

#include "Core.h"

class AudioInputDevice
{
public:
    AudioInputDevice() = delete;
    AudioInputDevice(std::wstring _wstrName, const EngineState* _state);
    ~AudioInputDevice() = default;

public:
    const std::vector<std::wstring> GetInputDevices() const;
    bool Open(const IO* _instance);
    void Start() { waveInStart(m_hwDevice); };
    void Read(size_t _uCurrentBlock);
    void SetCallback(void(CALLBACK* _func)(HWAVEIN,
                                           UINT,
                                           DWORD,
                                           DWORD,
                                           DWORD)) {
        m_CallbackFunction = _func;
    };
private:
    std::wstring m_wstrName;
    HWAVEIN m_hwDevice = 0;
    const EngineState* m_State = nullptr;

    SampleType* m_pBuffer = nullptr;
    std::array<WAVEHDR, uNumBlocks> m_arrBufferHeaders = { 0 };

    void(CALLBACK* m_CallbackFunction)(HWAVEIN,
                                       UINT,
                                       DWORD,
                                       DWORD,
                                       DWORD) = nullptr;
};

