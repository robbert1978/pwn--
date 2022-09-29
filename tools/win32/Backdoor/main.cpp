///
/// @file Backdoor
///
/// @author hugsy (hugsy [AT] blah [DOT] cat)
///
/// @brief
///

#include <pwn.hpp>

using namespace std::chrono_literals;


auto
wmain(const int argc, const wchar_t** argv) -> int
{
    pwn::Context.set(pwn::log::log_level_t::LOG_DEBUG);

    pwn::utils::random::seed();

    //
    // Start the backdoor thread
    //
    info(L"Starting backdoor thread");
    {
        auto res = pwn::backdoor::start();
        if ( Failed(res) )
        {
            err(L"Couldn't start backdoor thread");
            exit(EXIT_FAILURE);
        }
    }

    //
    // Enter to exit
    //
    pwn::utils::pause();

    //
    // Stop the thread
    //
    pwn::backdoor::stop();


    return EXIT_SUCCESS;
}
