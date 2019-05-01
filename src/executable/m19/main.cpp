#include <library/m19.h>
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

//#define TEST_16_BIT_SYMBOLS   // treat BWT output as 16 bit symbols


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
        std::uint32_t numThreads,
        maniscalco::parsing_method parsingMethod
    )
    {
        auto startClock = std::chrono::system_clock::now();
        maniscalco::m19_encode(begin, end, numThreads, parsingMethod);
        auto finishClock = std::chrono::system_clock::now();
        auto elapsedClock = (finishClock - startClock);
        auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedClock).count();
        std::cout << "elapsed time: " << msElapsed << " ms, " << (((long double)(end - begin) / ((long double)msElapsed / 1000)) / (1 << 20)) << " MB/sec" << std::endl;
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
        std::cout << "*** This build is pre-alpha and demonstrates both M19 and M03 context parsing only ***" << std::endl;
    }


    //==================================================================================================================
    std::int32_t print_usage
    (
    )
    {
        std::cout << "Usage: m19 [3|9] inputFile threadCount" << std::endl;
        std::cout << "\t3 = use M03 context modelling" << std::endl;
        std::cout << "\t9 = use M19 context modelling" << std::endl;
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

    if ((argCount < 3) || (strlen(argValue[1]) != 1))
        return print_usage();

    std::size_t numThreads = 0;
    if (argCount == 4)
    {
        // optional args added
        numThreads = 0;
        auto cur = argValue[3];
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
    if (numThreads == 0)
        numThreads = std::thread::hardware_concurrency();

    switch (argValue[1][0])
    {
        case '9':
        {
            auto input = load_file(argValue[2]);
            encode((std::uint8_t const *)input.data(), (std::uint8_t const *)input.data() + input.size(), 
                    numThreads, maniscalco::parsing_method::m19);
            break;
        }

        case '3':
        {
            auto input = load_file(argValue[2]);
            encode((std::uint8_t const *)input.data(), (std::uint8_t const *)input.data() + input.size(), 
                    numThreads, maniscalco::parsing_method::m03);
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

