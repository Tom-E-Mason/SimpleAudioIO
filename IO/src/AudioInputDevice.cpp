
#include "AudioInputDevice.h"

AudioInputDevice::AudioInputDevice(std::wstring _wstrName,
                                   const EngineState* _state)
    : m_wstrName(_wstrName), m_State(_state)
{
}

const std::vector<std::wstring> AudioInputDevice::GetInputDevices() const
{
    UINT uNumDevs{ waveInGetNumDevs() };
    std::vector<std::wstring> vDevices;
    vDevices.reserve(uNumDevs);
    WAVEINCAPS deviceInfo;

    for (UINT i{ 0 }; i < uNumDevs; ++i)
        if (waveInGetDevCaps(i, &deviceInfo, sizeof(WAVEINCAPS)) == MMSYSERR_NOERROR)
            vDevices.push_back(deviceInfo.szPname);

    return vDevices;
}

bool AudioInputDevice::Open(const IO* _instance)
{
    auto devs{ GetInputDevices() };
    auto dev{ std::find(devs.begin(), devs.end(), m_wstrName) };
    if (dev == devs.end())
        return false;

    UINT uDeviceId{ (UINT)std::distance(devs.begin(), dev) };

    WAVEINCAPS deviceInfo;
    if (waveInGetDevCaps(uDeviceId, &deviceInfo, sizeof(WAVEINCAPS)))
        return false;

    WORD wBlockAlign{ (WORD)(uBitDepth *
                             deviceInfo.wChannels / 8) };
    WORD cbSize{ (WORD)(sizeof(WAVEFORMATEXTENSIBLE) -
                         sizeof(WAVEFORMATEX)) };
    DWORD dwChannelMask{ 0 };
    for (WORD i{ 0 }; i < deviceInfo.wChannels; ++i)
        dwChannelMask |= 1 << i;

    WAVEFORMATEXTENSIBLE format
    {
        {
            WAVE_FORMAT_EXTENSIBLE,             // extensible tag
            deviceInfo.wChannels,               // channel count
            uSampleRate,                        // sample rate
            uSampleRate * wBlockAlign,          // average bytes/second
            wBlockAlign,                        // bytes/sample all channels
            sizeof(SampleType) * 8,             // 32 bit float
            cbSize                              // extensible size
        },
        sizeof(SampleType) * 8,                 // 32 bit float
        dwChannelMask,                          // speaker bit mask
        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT         // PCM format tag
    };

    if (waveInOpen(&m_hwDevice,
                   uDeviceId,
                   &format.Format,
                   (DWORD_PTR)m_CallbackFunction,
                   (DWORD_PTR)_instance,
                   CALLBACK_FUNCTION))
        return false;

    m_pBuffer = m_State->pBuffer;

    // links headers to buffers
    for (size_t i{ 0 }; i < m_arrBufferHeaders.size(); ++i)
    {
        m_arrBufferHeaders[i].dwBufferLength = uNumBlockSamples *
                                               sizeof(SampleType);
        m_arrBufferHeaders[i].lpData = (LPSTR)(m_pBuffer + (i * uNumBlockSamples));
    }

    return true;
}

void AudioInputDevice::Read(size_t _uCurrentBlock)
{
    if (m_arrBufferHeaders.at(_uCurrentBlock).dwFlags & WHDR_PREPARED)
        waveInUnprepareHeader(m_hwDevice, &m_arrBufferHeaders.at(_uCurrentBlock), sizeof(WAVEHDR));

    waveInPrepareHeader(m_hwDevice, &m_arrBufferHeaders.at(_uCurrentBlock), sizeof(WAVEHDR));

    waveInAddBuffer(m_hwDevice, &m_arrBufferHeaders.at(_uCurrentBlock), sizeof(WAVEHDR));
}
