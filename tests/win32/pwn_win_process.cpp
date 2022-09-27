#include <chrono>
#include <pwn.hpp>

#include "../catch.hpp"
#define NS "pwn::windows"

using namespace std::chrono_literals;

TEST_CASE("Process", "[" NS "]")
{
    SECTION("Local process tests")
    {
        pwn::windows::Process Local;
        REQUIRE(Local.IsValid() == false);

        auto res = pwn::windows::Process::Current();
        REQUIRE(Success(res));
        Local = Value(res);
        REQUIRE(Local.IsValid() == true);
        CHECK(Local.ProcessId() == ::GetCurrentProcessId());
        CHECK(Local.ProcessEnvironmentBlock() == (PPEB)::NtCurrentTeb()->ProcessEnvironmentBlock);
        CHECK(((uptr)Local.ProcessEnvironmentBlock() & 0xfff) == 0);
    }

    SECTION("Remote process tests")
    {
        const std::wstring TargetProcess = L"explorer.exe";
        u32 TargetPid                    = 0;
        {
            auto res = pwn::windows::system::PidOf(TargetProcess);
            REQUIRE(Success(res));
            REQUIRE(Value(res).size() > 0);
            TargetPid = Value(res).at(0);
            INFO("PID Found = " << TargetPid);
            REQUIRE(TargetPid > 0);
        }

        pwn::windows::Process Remote {TargetPid};
        REQUIRE(Remote.IsValid());
        CHECK(Remote.ProcessId() == TargetPid);
        PPEB RemotePeb = Remote.ProcessEnvironmentBlock();
        CHECK(RemotePeb != nullptr);
        CHECK(((uptr)RemotePeb & 0xfff) == 0);
    }

    SECTION("Process property - threads")
    {
        auto CurrentProcess = Value(pwn::windows::Process::Current());
        REQUIRE(CurrentProcess.IsValid() == true);

        auto res = CurrentProcess.Threads.List();
        REQUIRE(Success(res));
        auto tids = Value(res);
        REQUIRE(tids.size() > 0);
        CHECK(CurrentProcess.Threads[tids[0]].IsValid());
    }
}
