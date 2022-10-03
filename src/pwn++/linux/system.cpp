#include "linux/system.hpp"

namespace pwn::linux
{

u32
System::pagesize()
{
    return getpagesize();
}

    Result<std::string>
    System::ComputerName()
    {
        std::string hostname;
        hostname.resize(256);
        if( gethostname(hostname.data(), 256) < 0)
        {
            return Err(ErrorCode::ExternalApiCallFailed);
        }

        return Ok(hostname);
    }


    Result<std::string>
    System::UserName()
    {
        std::string username;
        username.resize(256);
        if( getlogin_r(username.data(), 256) < 0)
        {
            return Err(ErrorCode::ExternalApiCallFailed);
        }

        return Ok(username);
    }

}