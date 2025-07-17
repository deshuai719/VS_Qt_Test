#include "LogWZ.hpp"
#include <cstdarg>

LWZ::Log::Log(int LogNum)
{
    QDir dir;
    if (!dir.exists(DEFAULT_LOG_PATH)){
        dir.mkdir(DEFAULT_LOG_PATH);
    }
    LogFile = std::make_unique<FILE*[]>(LogNum);
    for(int i = 0; i < LogNum; ++i)
    {
        LogFile[i] = nullptr;
    }
}

LWZ::Log::~Log()
{
    for(int i = 0; i < LOG_NUM; ++i)
    {
        if(LogFile[i] != nullptr)
        {
            fclose(LogFile[i]);
            LogFile[i] = nullptr;
        }
    }
}

void LWZ::Log::Init(int LogIndex, const char* path)
{
	if (LogIndex < 0 || LogIndex >= LOG_NUM)
	{
		return;
	}
	if (LogFile[LogIndex] != nullptr)
	{
		return;
	}
	LogFile[LogIndex] = fopen(path, "w");
}

void LWZ::Log::Write(int LogIndex, const char* format, ...)
{
    if(LogIndex < 0 || LogIndex >= LOG_NUM)
    {
        return;
    }
    if(LogFile[LogIndex] == nullptr)
    {
        return;
    }
    va_list args;
    va_start(args, format);
    vfprintf(LogFile[LogIndex], format, args);
    fflush(LogFile[LogIndex]);
    va_end(args);
}

void LWZ::Log::Close(int LogIndex)
{
    if(LogIndex < 0 || LogIndex >= LOG_NUM)
    {
        return;
    }
    if(LogFile[LogIndex] == nullptr)
    {
        return;
    }
    fclose(LogFile[LogIndex]);
    LogFile[LogIndex] = nullptr;
}

LWZ::Log* LWZ::Log::LogInstance = new Log(LOG_NUM);
bool g_logOn = true;