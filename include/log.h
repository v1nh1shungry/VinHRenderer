#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <windows.h>

// define VINH_NOT_TO_LOG to close the log

namespace vinh
{
#if defined(VINH_NOT_TO_LOG)

	template <typename... Args>
	void LOGGING(Args...)
	{
		;
	}

	template <typename... Args>
	void WARNING(Args...)
	{
		;
	}

	template <typename... Args>
	void ERRORING(Args...)
	{
		;
	}

#else

	template <typename... Args>
	void LOGGING(Args... args)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cerr << "LOGGING: ";
		(std::cerr << ... << args) << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	template <typename... Args>
	void WARNING(Args... args)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
		std::cerr << "WARNING: ";
		(std::cerr << ... << args) << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	template <typename... Args>
	void ERRORING(Args... args)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cerr << "ERROR: ";
		(std::cerr << ... << args) << std::endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

#endif
}

#endif
