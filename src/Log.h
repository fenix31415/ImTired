#pragma once

#include "SKSE/Impl/PCH.h"
//#define DEBUG_ON
//#define DEBUG_INFO
//#define DEBUG_ERRORS

template <class... Args>
struct info
{
	info() = delete;

	template <class T>
	info(T&& a_fmt, Args&&... a_args, SKSE::stl::source_location a_loc = SKSE::stl::source_location::current())
	{
#ifdef DEBUG_ON
#ifdef DEBUG_INFO
		spdlog::log(spdlog::source_loc{ a_loc.file_name(), static_cast<int>(a_loc.line()), a_loc.function_name() },
			spdlog::level::info, std::forward<T>(a_fmt), std::forward<Args>(a_args)...);
#endif	// DEBUG_INFO
#endif	// DEBUG_ON
	}
};
template <class T, class... Args>
info(T&&, Args&&...) -> info<Args...>;


template <class... Args>
struct error
{
	error() = delete;

	template <class T>
	error(T&& a_fmt, Args&&... a_args, SKSE::stl::source_location a_loc = SKSE::stl::source_location::current())
	{
#ifdef DEBUG_ON
#ifdef DEBUG_ERRORS
		spdlog::log(spdlog::source_loc{ a_loc.file_name(), static_cast<int>(a_loc.line()), a_loc.function_name() },
			spdlog::level::err, std::forward<T>(a_fmt), std::forward<Args>(a_args)...);
#endif	// DEBUG_ERRORS
#endif	// DEBUG_ON
	}
};
template <class T, class... Args>
error(T&&, Args&&...) -> error<Args...>;


template <typename T>
bool check_null(T ptr, const char* message)
{
	if (!ptr) {
		error(message, 1);
		return true;
	}
	return false;
}
