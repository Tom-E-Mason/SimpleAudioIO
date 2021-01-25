
#include "IO.h"

int main()
{
    IO io;

    auto devs{ io.GetInputDevices() };
    std::cout << "Available input devices:\n\n";
    for (size_t i{ 0 }; i < devs.size(); ++i)
        std::wcout << "    " << i + 1 << ": " << devs.at(i) << "\n";
    
    std::cout << "\nSelect by number.\n\n";
    
    size_t uDeviceId{ 0 };
    std::cin >> uDeviceId;
    --uDeviceId;

    io.SetInputAudioDevice(devs.at(uDeviceId));
    
    devs = io.GetOutputDevices();
    std::cout << "\nAvailable output devices:\n\n";
    for (size_t i{ 0 }; i < devs.size(); ++i)
        std::wcout << "    " << i + 1 << ": " << devs.at(i) << "\n";
    
    std::cout << "\nSelect by number.\n\n";
    
    std::cin >> uDeviceId;
    --uDeviceId;

    io.SetOutputAudioDevice(devs.at(uDeviceId));
    
    io.Play();
}