
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include <Windows.h>
#include <mmreg.h>

#define IOEXP __declspec(dllexport)

class IOEXP IO;

constexpr size_t uSampleRate = 44100;
constexpr size_t uBitDepth = 32;
constexpr size_t uNumBlocks = 16;
constexpr size_t uNumBlockSamples = 256;

typedef float SampleType;

struct EngineState
{
    size_t uCurrentInputBlock = 1;
    std::atomic<size_t> uCurrentOutputBlock = 0;
    SampleType* pBuffer;
};
