//##################################################################################################
//
//   Caffa
//   Copyright (C) 2021- 3D-Radar AS
//
//   GNU Lesser General Public License Usage
//   This library is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Lesser General Public License as published by
//   the Free Software Foundation; either version 2.1 of the License, or
//   (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or
//   FITNESS FOR A PARTICULAR PURPOSE.
//
//   See the GNU Lesser General Public License at <<http://www.gnu.org/licenses/lgpl-2.1.html>>
//   for more details.
//
//##################################################################################################
#include "cafLogger.h"

#include "cafStringTools.h"

#include <assert.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <thread>

using namespace caffa;

Logger::Level                          Logger::s_applicationLogLevel = Logger::Level::WARNING;
std::unique_ptr<std::ostream>          Logger::s_stream = std::make_unique<std::ostream>( std::cout.rdbuf() );
std::mutex                             Logger::s_mutex;
std::map<std::thread::id, std::string> Logger::s_threadNames;
std::chrono::system_clock::time_point  Logger::s_startTime       = std::chrono::system_clock::now();
Logger::TimeGranularity                Logger::s_timeGranularity = Logger::TimeGranularity::MILLISECONDS;

void Logger::log( Level level, const std::string& message, char const* function, char const* file, int line )
{
    std::scoped_lock lock( s_mutex );

    auto now              = std::chrono::system_clock::now();
    auto s_since_startup  = std::chrono::duration_cast<std::chrono::seconds>( now - s_startTime );
    auto ms_since_startup = std::chrono::duration_cast<std::chrono::milliseconds>( now - s_startTime );

    std::chrono::seconds      secs( s_since_startup.count() );
    std::chrono::milliseconds ms              = std::chrono::duration_cast<std::chrono::milliseconds>( secs );
    auto                      ms_since_last_s = ms_since_startup - ms;

    auto threadId     = std::this_thread::get_id();
    auto threadNameIt = s_threadNames.find( threadId );
    auto threadName   = threadNameIt != s_threadNames.end() ? threadNameIt->second : "UNKNOWN_THREAD";

    if ( level <= s_applicationLogLevel )
    {
        // TODO: should provide platform specific path delimiter
        auto filePath       = caffa::StringTools::split( file, "/" );
        auto fileName       = !filePath.empty() ? filePath.back() : file;
        auto fileComponents = caffa::StringTools::split( fileName, "." );
        fileName            = !fileComponents.empty() ? fileComponents.front() : fileName;
        if ( s_timeGranularity != TimeGranularity::NONE )
        {
            *s_stream << "[" << std::setfill( '0' ) << std::setw( 3 ) << s_since_startup.count();
            if ( s_timeGranularity == TimeGranularity::MILLISECONDS )
            {
                *s_stream << "." << std::setfill( '0' ) << std::setw( 3 ) << ms_since_last_s.count();
            }
            *s_stream << "] ";
        }
        *s_stream << "[" << logLevelLabel( level ) << "] " << fileName << "::" << function << "[" << line << "]";
        if ( !threadName.empty() )
        {
            *s_stream << "{" << threadName << "}";
        }

        *s_stream << ": " << message << std::endl;
    }
}

Logger::Level Logger::applicationLogLevel()
{
    return s_applicationLogLevel;
}
void Logger::setApplicationLogLevel( Level applicationLogLevel )
{
    s_applicationLogLevel = applicationLogLevel;
}

std::string Logger::logLevelLabel( Level level )
{
    return Logger::logLevels()[level];
}

Logger::Level Logger::logLevelFromLabel( const std::string& label )
{
    for ( auto [level, levelLabel] : logLevels() )
    {
        if ( levelLabel == label ) return level;
    }
    assert( false );
    return Logger::Level::CRITICAL;
}

void Logger::setLogFile( const std::string& logFile )
{
    s_stream = std::make_unique<std::ofstream>( logFile );
}

std::map<Logger::Level, std::string> Logger::logLevels()
{
    return { { Level::TRACE, "trace" },
             { Level::DEBUG, "debug" },
             { Level::INFO, "info" },
             { Level::WARNING, "warning" },
             { Level::ERROR, "error" },
             { Level::CRITICAL, "critical" } };
}

void Logger::setTimeGranularity( TimeGranularity granularity )
{
    s_timeGranularity = granularity;
}

void Logger::registerThreadName( const std::string& name )
{
    std::scoped_lock lock( s_mutex );
    s_threadNames[std::this_thread::get_id()] = name;
}
