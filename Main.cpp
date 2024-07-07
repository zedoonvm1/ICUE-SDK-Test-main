#include <iostream>
#include <thread>
#include <chrono>
#include "iCUESDK.h"

#define KEYBOARD_TYPE_MASK 0x0001


// Callback function to handle session state changes
void onSessionStateChanged(void* context, const CorsairSessionStateChanged* eventData) {
    switch (eventData->state) {
    case CSS_Connecting:
        std::cout << "Connecting to iCUE..." << std::endl;
        break;
    case CSS_Connected:
        std::cout << "Connected to iCUE." << std::endl;
        break;
    case CSS_Timeout:
        std::cout << "Connection to iCUE timed out." << std::endl;
        break;
    case CSS_ConnectionRefused:
        std::cout << "Connection to iCUE refused. Ensure third-party control is enabled in iCUE settings." << std::endl;
        break;
    case CSS_Closed:
        std::cout << "Connection to iCUE closed." << std::endl;
        break;
    case CSS_ConnectionLost:
        std::cout << "Connection to iCUE lost." << std::endl;
        break;
    default:
        std::cout << "Unknown session state." << std::endl;
    }
}

int main() {


    // Connecting the program to iCUE software
    CorsairError error = CorsairConnect(onSessionStateChanged, nullptr);
    if (error != CE_Success) {
        std::cerr << "Failed to connect to iCUE: " << error << std::endl;
        return 1;
    }

    // Wait for the connection to stabilize
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Create a filter for getting the Corsair device
    CorsairDeviceFilter myDivceFilter;
    myDivceFilter.deviceTypeMask = 0x0001;  // 0x01 is for keyboards

    // Device info array to store device IDs
    CorsairDeviceInfo myDevices[CORSAIR_DEVICE_COUNT_MAX];
    int size = 0;  // Debugging variable to output the number of devices retrieved

    // Get all keyboard devices
    CorsairError errorCode = CorsairGetDevices(&myDivceFilter, CORSAIR_DEVICE_COUNT_MAX, myDevices, &size);

    // Error handling to ensure devices were found
    if (errorCode != CE_Success) {
        std::cerr << "Failed to get devices: " << errorCode << std::endl;
        return 1;
    }

    // Error handling to ensure devices were found
    if (size == 0) {
        std::cerr << "No devices found." << std::endl;
        return 1;
    }

    const char* deviceId = myDevices[0].id;  // Getting the first device

    int size2 = 0;
    CorsairLedPosition ledPositions[CORSAIR_DEVICE_LEDCOUNT_MAX];  // Array for key positions

    // Get key IDs
    CorsairError error2 = CorsairGetLedPositions(deviceId, CORSAIR_DEVICE_LEDCOUNT_MAX, ledPositions, &size2);
    if (error2 != CE_Success) {
        std::cerr << "Error: Failed to get LED positions: " << error << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a variable to hold certain key colors
    CorsairLedColor ledColors[CORSAIR_DEVICE_LEDCOUNT_MAX];

    for (int i = 0; i < CORSAIR_DEVICE_LEDCOUNT_MAX; ++i) {
        ledColors[i].id = ledPositions[i].id;
        ledColors[i].r = 255;
        ledColors[i].g = 255;
        ledColors[i].b = 255;
    }

    // Set all key colors to white
    int error3 = CorsairSetLedColors(deviceId, CORSAIR_DEVICE_LEDCOUNT_MAX, ledColors);
    if (error3 != CE_Success) {
        std::cerr << "Error: Failed to set LED colors: " << error << std::endl;
        exit(EXIT_FAILURE);
    }

    // Keep the program running
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
