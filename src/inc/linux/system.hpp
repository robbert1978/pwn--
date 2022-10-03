#pragma once

#include "common.hpp"

#include <string>
#include <optional>


namespace pwn::linux
{
	class System
	{
	public:
	static u32
	PageSize();

    static Result<std::string>
    ComputerName();

    static Result<std::string>
    UserName();

	private:
	}
}