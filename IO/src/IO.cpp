
#include "IO.h"


IO::IO()
{
    m_auBlocksFree = uNumBlocks;
}

void IO::Play()
{
    SampleType audioBuffer[uNumBlocks * uNumBlockSamples]{ 0 };
    m_EngineState.pBuffer = audioBuffer;

    m_upInputDevice->SetCallback(waveInProcWrap);
    m_upOutputDevice->SetCallback(waveOutProcWrap);

    if (!m_upInputDevice->Open(this))
        std::cout << "Failed to open input device!\n";
    if (!m_upOutputDevice->Open(this))
        std::cout << "Failed to open output device!\n";

    m_upInputDevice->Start();

    size_t inBufIdx{ m_EngineState.uCurrentInputBlock };
    size_t outBufIdx{ m_EngineState.uCurrentOutputBlock };

    auto cleanup = [this]()
    {
        while (1)
        {
            {
                std::unique_lock<std::mutex> lk{ m_mux };
                m_cvOut.wait(lk);
            }

            m_upOutputDevice->CleanUp(m_EngineState.uCurrentOutputBlock);
            m_EngineState.uCurrentOutputBlock = NextBlock(m_EngineState.uCurrentOutputBlock);
        }
    };
    //std::thread cleanupThread{ cleanup };

    while (1)
    {
        if (!m_auBlocksFree)
        {
            std::unique_lock<std::mutex> lk{ m_mux };
            m_cvIn.wait(lk);
        }
        --m_auBlocksFree;

        m_upInputDevice->Read(inBufIdx);
        m_upOutputDevice->Write(outBufIdx);

        inBufIdx = NextBlock(inBufIdx);
        outBufIdx = NextBlock(outBufIdx);
    }

}

const std::vector<std::wstring> IO::GetInputDevices() const
{
    return m_upInputDevice->GetInputDevices();
}

const std::vector<std::wstring> IO::GetOutputDevices() const
{
    return m_upOutputDevice->GetOutputDevices();
}

void IO::SetInputAudioDevice(const std::wstring& _name)
{
    m_upInputDevice = std::make_unique<AudioInputDevice>(_name, &m_EngineState);
}

void IO::SetOutputAudioDevice(const std::wstring& _name)
{
    m_upOutputDevice = std::make_unique<AudioOutputDevice>(_name, &m_EngineState);
}

// ---------------------------------------------------------------------------------------
// Handler for soundcard request for more blocks to fill.
//
// Arguments:
//     _hWaveIn - audio device handle
//     _uMsg     - user message
//     _dwParam1 - message parameter
//     _dwParam2 - message parameter
//
// Returns:
//     void.
// ---------------------------------------------------------------------------------------
void IO::waveInProc(HWAVEIN hWaveIn,
                    UINT uMsg,
                    DWORD dwParam1,
                    DWORD dwParam2)
{
    if (uMsg != WIM_DATA) return;

    ++m_auBlocksFree;
    m_cvIn.notify_one();
}

// ---------------------------------------------------------------------------------------
// Static wrapper for sound card handler.
//
// Arguments:
//     _hWaveIn   - audio device handle
//     _uMsg       - user message
//     _dwInstance - user instance data specified with waveOutOpen
//     _dwParam1   - message parameter
//     _dwParam2   - message parameter
//
// Returns:
//     void
// ---------------------------------------------------------------------------------------
void CALLBACK IO::waveInProcWrap(HWAVEIN hWaveIn,
                                 UINT uMsg,
                                 DWORD dwInstance,
                                 DWORD dwParam1,
                                 DWORD dwParam2)
{
    ((IO*)dwInstance)->waveInProc(hWaveIn,
                                  uMsg,
                                  dwParam1,
                                  dwParam2);
}

// ---------------------------------------------------------------------------------------
// Handler for soundcard request for more data.
//
// Arguments:
//     _hWaveIn - audio device handle
//     _uMsg     - user message
//     _dwParam1 - message parameter
//     _dwParam2 - message parameter
//
// Returns:
//     void.
// ---------------------------------------------------------------------------------------
void IO::waveOutProc(HWAVEOUT hWaveOut,
                     UINT uMsg,
                     DWORD dwParam1,
                     DWORD dwParam2)
{
    if (uMsg != WIM_DATA) return;

    ++m_EngineState.uCurrentOutputBlock;
    m_cvOut.notify_one();
}

// ---------------------------------------------------------------------------------------
// Static wrapper for sound card handler.
//
// Arguments:
//     _hWaveIn   - audio device handle
//     _uMsg       - user message
//     _dwInstance - user instance data specified with waveOutOpen
//     _dwParam1   - message parameter
//     _dwParam2   - message parameter
//
// Returns:
//     void
// ---------------------------------------------------------------------------------------
void CALLBACK IO::waveOutProcWrap(HWAVEOUT hWaveOut,
                                  UINT uMsg,
                                  DWORD dwInstance,
                                  DWORD dwParam1,
                                  DWORD dwParam2)
{
    ((IO*)dwInstance)->waveOutProc(hWaveOut,
                                   uMsg,
                                   dwParam1,
                                   dwParam2);
}