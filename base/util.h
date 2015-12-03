#pragma once
#include <string>
#include <map>
#include <unistd.h>
#include <boost/lexical_cast.hpp>

namespace loki
{
	class Global
	{
	public:
		static std::map<std::string, std::string> Value;

		template<class T>
			static T Get(const std::string& key) {
				return boost::lexical_cast<T>(Value[key]);
			}

		static void ParseCommand(const int argc, char** argv)
		{
			Value["Daemon"] = "0";
			Value["ThreadNum"] = "1";

			int ch = 0;
			while ((ch = getopt(argc, argv, "t:df:"))!= -1)
			{
				switch (ch)
				{
					case 't':
						{
							Value["ThreadNum"] = optarg;
						}
						break;
					case 'd':
						{
							Value["Daemon"] = "1";
						}
						break;
					case 'f':
						{
							Value["Script"] = optarg;
						}
						break;
				}
			}
		}
	};

};

namespace Cmd
{

	inline bool isset_state(const unsigned char* state, const int teststate)
	{
		return 0!=(state[teststate/8]&(0xff&(1<<(teststate%8))));
	}

	inline void set_state(unsigned char* state, const int teststate)
	{
		state[teststate/8] |= (0xff & (1<<(teststate%8)));
	}

	inline void clear_state(unsigned char* state, const int teststate)
	{
		state[teststate/8] &= (0xff & (~(1<<(teststate %8))));
	}

};
