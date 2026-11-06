#pragma once
#include <API/eLogLevel.h>

struct iLoggerSetup
{
	virtual void logMessage( AudioCodecs::eLogLevel lvl, const char8_t* message ) const noexcept = 0;

	virtual AudioCodecs::eLogLevel maxLogLevel() const noexcept = 0;
};

struct LoggerSetupRaii
{
public:
	LoggerSetupRaii( iLoggerSetup* setup ) noexcept;
	LoggerSetupRaii( const LoggerSetupRaii& ) = delete;
	LoggerSetupRaii& operator=( const LoggerSetupRaii& ) = delete;
	~LoggerSetupRaii() noexcept;
};