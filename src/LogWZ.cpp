//日志读写操作的封装与实现
//

#include "LogWZ.hpp"
#include <cstdarg>

// 新增：温度等级判断和后缀生成函数实现
namespace LWZ {
    static QString g_seLogTimestamp;
    static double g_seLogEnvTemp = 0.0;

    QString GenerateTemperatureSuffix(double temperature) {
        QString tempLevel;
        if (temperature < 0) {
            tempLevel = "低温";  // 小于0为低温
        } else if (temperature >= 0 && temperature <= 40) {
            tempLevel = "常温";  // 0-40为常温
        } else {
            tempLevel = "高温";  // 大于40为高温
        }
        // 修改：正确格式化温度后缀，格式：_低温_-5.2℃，_常温_25.0℃，_高温_55.8℃
        return QString("_%1_%2℃").arg(tempLevel).arg(temperature, 0, 'f', 1);  // 修改：使用正确的QString格式化方法
    }

    // 记录/获取LogSeRecord的时间戳，供最终结果命名使用
    void SetSeLogTimestamp(const QString& ts) { g_seLogTimestamp = ts; }
    QString GetSeLogTimestamp() { return g_seLogTimestamp; }
    void SetSeLogEnvTemp(double t) { g_seLogEnvTemp = t; }
    double GetSeLogEnvTemp() { return g_seLogEnvTemp; }
}

LWZ::Log::Log(int LogNum)
{
    QDir dir;
    // 修改：确保使用UTF-8编码创建中文文件夹
    QString logPath = QString::fromUtf8(DEFAULT_LOG_PATH);
    if (!dir.exists(logPath)){
        bool success = dir.mkpath(logPath); // 修改：使用mkpath确保创建完整路径
        if (!success) {
            // 新增：如果创建失败，尝试创建英文备用文件夹
            logPath = "./LogFiles/";
            dir.mkpath(logPath);
        }
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
	// 修改：支持中文路径的文件打开，Qt6中直接使用QString转换
	QString qPath = QString::fromUtf8(path);
	QByteArray localPath = qPath.toLocal8Bit(); // 新增：转换为本地编码
	LogFile[LogIndex] = fopen(localPath.constData(), "w");
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
bool g_logOn = false;//默认开启详细日志记录bool g_logOn = false;//默认开启详细日志记录