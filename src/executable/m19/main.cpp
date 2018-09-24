#include <library/m19.h>
#include <library/msufsort.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstring>
#include <string>

//#define VERBOSE_MAIN


namespace
{

    //==================================================================================================================
    std::vector<char> load_file
    (
        char const * path
    )
    {
        // read data from file
        std::vector<char> input;
        std::ifstream inputStream(path, std::ios_base::in | std::ios_base::binary);
        if (!inputStream.is_open())
        {
            std::cout << "failed to open file \"" << path << "\"" << std::endl;
            return std::vector<char>();
        }

        inputStream.seekg(0, std::ios_base::end);
        std::size_t size = inputStream.tellg();
        input.resize(size);
        inputStream.seekg(0, std::ios_base::beg);
        inputStream.read(input.data(), input.size());
        inputStream.close();
        return input;
    }


    //==================================================================================================================
    void encode
    (
        std::uint8_t const * begin,
        std::uint8_t const * end,
        char const * outputPath,
        std::uint32_t numThreads
    )
    {
        auto startClock = std::chrono::system_clock::now();
        auto sentinelIndex = maniscalco::forward_burrows_wheeler_transform(begin, end, numThreads);
        auto finishClock = std::chrono::system_clock::now();
        auto elapsedClock = (finishClock - startClock);
        auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedClock).count();
        std::cout << "BWT elapsed time: " << msElapsed << " ms" << std::endl;

        startClock = std::chrono::system_clock::now();
        maniscalco::m19_encode(begin, end, sentinelIndex);
        finishClock = std::chrono::system_clock::now();
        elapsedClock = (finishClock - startClock);
        msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedClock).count();
        std::cout << "Total m19 - modelling elapsed time: " << msElapsed << " ms" << std::endl;
    }


    //==================================================================================================================
    void decode
    (
        std::uint8_t const *,
        char const *,
        std::uint32_t
    )
    {
    }


    //==================================================================================================================
    void print_about
    (
    )
    {
        std::cout << "m19 - next generation BWT compressor.  Author: M.A. Maniscalco (2018 - 2019)" << std::endl;
    }


    //==================================================================================================================
    std::int32_t print_usage
    (
    )
    {
        std::cout << "Usage: m19 [e|d] inputFile outputFile threadCount" << std::endl;
        return 0;
    }
}


//======================================================================================================================
std::int32_t main
(
    std::int32_t argCount,
    char const * argValue[]
)
{
    print_about();

    if ((argCount < 4) || (strlen(argValue[1]) != 1))
        return print_usage();

    std::size_t numThreads = 0;
    if (argCount == 5)
    {
        // optional args added
        numThreads = 0;
        auto cur = argValue[4];
        while (*cur != 0)
        {
            if ((*cur < '0') || (*cur > '9'))
            {
                std::cout << "invalid thread count" << std::endl;
                print_usage();
                return -1;
            }
            numThreads *= 10;
            numThreads += (*cur - '0');
            ++cur;
        }
    }
    if ((numThreads == 0))// || (numThreads > std::thread::hardware_concurrency()))
        numThreads = std::thread::hardware_concurrency();

    switch (argValue[1][0])
    {
        case 'e':
        {
            auto input = load_file(argValue[2]);
            encode((std::uint8_t const *)input.data(), (std::uint8_t const *)input.data() + input.size(), argValue[3], numThreads);
            break;
        }

        case 'd':
        {
            auto input = load_file(argValue[2]);
            decode((std::uint8_t const *)input.data(), argValue[3], numThreads);
            break;
        }

        default:
        {
            print_usage();
            break;
        }
    }

    return 0;
}

