#include <cstdlib>    // for EXIT_SUCCESS, EXIT_FAILURE
#include <exception>  // for std::exception
#include <fstream>    // for std::ifstream
#include <ios>        // for std::ios_base, etc.
#include <iostream>   // for std::cerr, std::cout
#include <ostream>    // for std::endl
#include <map>

#include "Shlwapi.h"

namespace FileUtils
{
	inline DWORD CalcCrcForFile(const TCHAR*sName)
	{
		return 0;
		//std::streamsize const  buffer_size = 1024;
		//boost::crc_32_type  result;

		//std::ifstream  ifs(sName, std::ios_base::binary);

		//if (ifs)
		//{
		//	do
		//	{
		//		char  buffer[buffer_size];

		//		ifs.read(buffer, buffer_size);
		//		result.process_bytes(buffer, ifs.gcount());
		//	} while (ifs);
		//}

		//return result.checksum();

	}
	inline bool IsFileExists(const wchar_t* sPath)
	{
		return PathFileExists(sPath) ? true : false;
	}

	inline bool RenameFile(const wchar_t* old, const wchar_t* newf)
	{
		return MoveFile(old, newf);
	}

}

