#pragma once

#include <AccCtrl.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include "common.hpp"
#include "handle.hpp"
#include "win32/nt.hpp"

namespace fs = std::filesystem;

namespace pwn::windows::process
{

class Process
{
    class Memory
    {
    public:
        Memory();
        Memory(SharedHandle& h);

        auto
        Read(uptr const Address, usize Length) -> Result<std::vector<u8>>;

        auto
        Write(uptr const Address, std::vector<u8> data) -> Result<usize>;

        auto
        Memset(uptr const address, const size_t size, const u8 val = 0x00) -> Result<uptr>;

        auto
        allocate(
            const size_t Size,
            const wchar_t Permission[3]     = L"rwx",
            const uptr ForcedMappingAddress = 0,
            bool wipe                       = true) -> Result<uptr>;

        auto
        free(const uptr Address) -> bool;

    private:
        SharedHandle m_process_handle;
    };

public:
    using Privilege  = std::wstring;
    using Privileges = std::vector<Privilege>;

    enum class Integrity : int
    {
        Unknown,
        Low,
        Medium,
        High,
        System
    };

    Process();

    Process(u32, bool = false);

    ~Process();

    fs::path const
    Path() const;

    u32 const
    ParentProcessId() const;

    u32 const
    ProcessId() const;

    auto
    operator<=>(Process const&) const = default;

    PPEB
    peb();

    PTEB
    teb();

    const HANDLE
    handle() const;

    auto
    is_elevated() -> bool;

    auto
    enumerate_privileges() -> bool;

    friend std::wostream&
    operator<<(std::wostream& os, const Process& p)
    {
        os << L"Process(Pid=" << p.ProcessId() << L")";
        return os;
    }

    ///
    /// @brief Kill the process
    ///
    /// @return Result<bool>
    ///
    Result<bool>
    Kill();

    ///
    /// @brief Retrieve the process integrity level
    ///
    /// @return Result<Process::Integrity>
    ///
    Result<Process::Integrity>
    IntegrityLevel();

    ///
    /// @brief Add a privilege to the process (if possible)
    ///
    /// @param PrivilegeName
    /// @return Result<bool> true if the privilege was added (false, not added). ErrorCode otherwise
    ///
    Result<bool>
    AddPrivilege(std::wstring const& PrivilegeName);

    ///
    /// @brief  a privilege to the process (if possible)
    ///
    /// @param PrivilegeName
    /// @return Result<bool> true if the privilege is acquired (false if not).  ErrorCode otherwise
    ///
    Result<bool>
    HasPrivilege(std::wstring const& PrivilegeName);

    // TODO:
    // - modules

    Memory Memory;

private:
    u32 m_Pid;
    u32 m_Ppid;
    std::wstring m_Path;
    Integrity m_IntegrityLevel;
    SharedHandle m_ProcessHandle;
    Privileges m_Privileges;
    bool m_KillOnClose;
    bool m_IsSelf;
    PPEB m_Peb;
    PTEB m_Teb;
};


///
/// @brief Returns a basic list of processes, in a vector of tuple <ProcessName, PID>
///
/// @return std::vector<std::tuple<std::wstring, u32>>
///
PWNAPI auto
Processes() -> std::vector<std::tuple<std::wstring, u32>>;

PWNAPI auto
execv(const std::wstring_view& CommandLine, const u32 ParentPid = 0) -> Result<std::tuple<HANDLE, HANDLE>>;

_Success_(return )
PWNAPI auto
system(_In_ const std::wstring& lpCommandLine, _In_ const std::wstring& operation = L"open") -> bool;

_Success_(return != nullptr)
PWNAPI auto
cmd() -> HANDLE;


namespace appcontainer
{
class AppContainer
{
public:
    AppContainer(
        std::wstring_view const& container_name,
        std::wstring_view const& executable_path,
        std::vector<WELL_KNOWN_SID_TYPE> const& DesiredCapabilities = {});

    ~AppContainer();

    _Success_(return )
    auto
    allow_file_or_directory(_In_ const std::wstring& file_or_directory_name) -> bool;

    _Success_(return )
    auto
    allow_registry_key(_In_ const std::wstring& regkey) -> bool;

    _Success_(return )
    auto
    spawn() -> bool;

    _Success_(return )
    auto
    restore_acls() -> bool;

    _Success_(return )
    auto
    join(_In_ u32 dwTimeout = INFINITE) -> bool;

private:
    auto
    set_named_object_access(
        const std::wstring& ObjectName,
        const SE_OBJECT_TYPE ObjectType,
        const ACCESS_MODE AccessMode,
        const ACCESS_MASK AccessMask) -> bool;

    std::wstring m_ContainerName;
    std::wstring m_ExecutablePath;
    std::vector<WELL_KNOWN_SID_TYPE> m_Capabilities;
    std::vector<std::tuple<std::wstring, SE_OBJECT_TYPE, PACL>> m_OriginalAcls;
    std::wstring m_SidAsString;
    std::wstring m_FolderPath;

    PSID m_AppContainerSid                       = nullptr;
    STARTUPINFOEX m_StartupInfo                  = {{0}};
    PROCESS_INFORMATION m_ProcessInfo            = {nullptr};
    SECURITY_CAPABILITIES m_SecurityCapabilities = {nullptr};
};
} // namespace appcontainer
} // namespace pwn::windows::process


std::wostream&
operator<<(std::wostream& wos, const pwn::windows::process::Process::Integrity i);

template<>
struct std::formatter<pwn::windows::process::Process::Integrity, wchar_t> : std::formatter<std::wstring, wchar_t>
{
    auto
    format(pwn::windows::process::Process::Integrity i, wformat_context& ctx)
    {
        std::wstring wstr;
        switch ( i )
        {
        case pwn::windows::process::Process::Integrity::Low:
            wstr = std::format(L"INTEGRITY_LOW");
            break;

        case pwn::windows::process::Process::Integrity::Medium:
            wstr = std::format(L"INTEGRITY_MEDIUM");
            break;

        case pwn::windows::process::Process::Integrity::High:
            wstr = std::format(L"INTEGRITY_HIGH");
            break;

        case pwn::windows::process::Process::Integrity::System:
            wstr = std::format(L"INTEGRITY_SYSTEM");
            break;

        default:
            wstr = std::format(L"INTEGRITY_UNKNOWN");
            break;
        }
        return std::formatter<wstring, wchar_t>::format(wstr, ctx);
    }
};
