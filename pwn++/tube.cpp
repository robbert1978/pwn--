#include "tube.h"


size_t Tube::send(_In_ std::vector<BYTE> const& data)
{
	return __send_internal(data);
}


size_t Tube::send(_In_ std::string const& str)
{
	return __send_internal(pwn::utils::string_to_bytes(str));
}


std::vector<BYTE> Tube::recv(_In_ size_t size)
{
	return __recv_internal(size);
}


size_t Tube::sendline(_In_ std::vector<BYTE> const& data)
{
	auto send_data(data);
	send_data.push_back(PWN_LINESEP);
	return send(send_data);
}


size_t Tube::sendline(_In_ std::string const& str)
{
	return sendline(pwn::utils::string_to_bytes(str));
}


std::vector<BYTE> Tube::recvuntil(_In_ std::vector<BYTE> const& pattern)
{
	size_t idx = 0;
	std::vector<BYTE> in;

	while (true)
	{
		// append new data received from the pipe
		{
			auto in2 = recv(PWN_TUBE_PIPE_DEFAULT_SIZE);
			if (in2.empty()) 
				return {};
			in.insert(in.begin(), in2.begin(), in2.end());
		}

		// look for the pattern
		if (std::find_if(
			in.begin(),
			in.end(),
			[&idx,&pattern, &in](BYTE const& x) { 			
				idx++;
				auto i = idx;
				auto sz = pattern.size();

				if (i < sz)
					return false;

				for (auto j = 0; j < sz; j++)
				{
					auto c1 = pattern.at(j);
					auto c2 = in.at( (i - sz) + j);
					dbg(L"[%d] c1=%x == c2=%x\n", idx, c1, c2);
					if (pattern.at(j) != in.at(i-sz))
						return false;
				}

				return true; 
			}
		) != in.end())
		{
			// line separator found, copy the rest of the buffer to the queue
			std::copy(
				in.begin() + idx,
				in.end(),
				std::back_inserter(m_receive_buffer)
			);

			in.erase(
				in.begin() + idx,
				in.end()
			);

			return in;
		}
	}
}


std::vector<BYTE> Tube::recvline()
{
	return recvuntil({ PWN_LINESEP });
}


size_t Tube::peek()
{
	return __peek_internal();
}


static bool __bReplLoop = false;


_Success_(return)
static BOOL WINAPI __pwn_interactive_repl_sighandler(_In_ DWORD signum)
{
	switch (signum)
	{
		case CTRL_C_EVENT:
			dbg(L"Stopping interactive mode...\n");
			__bReplLoop = false;
			::ExitProcess(0);
			break;

		default:
			break;
	}
		
	return TRUE;
}


void Tube::interactive()
{
	__bReplLoop = true;
 
	::SetConsoleCtrlHandler(__pwn_interactive_repl_sighandler, true);

	ok(L"Entering interactive mode...\n");

	while (__bReplLoop)
	{
		std::string cmd;
		std::cout << ">>> ";
		std::cin >> cmd;

		if (cmd == "quit")
			break;

		sendline(cmd);
		
		while (true)
		{
			try
			{		
				auto in = recv(PWN_TUBE_PIPE_DEFAULT_SIZE);
				::printf("%s\n", &in[0]);

				if (in.size() < PWN_TUBE_PIPE_DEFAULT_SIZE)
					break;
			}
			catch (...)
			{
				break;
			}
		}
	}

	::SetConsoleCtrlHandler(nullptr, true);

	ok(L"Leaving interactive mode...\n");
}