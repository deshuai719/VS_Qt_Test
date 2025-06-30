#ifndef _LOGWZ_HPP_
#define _LOGWZ_HPP_

#include <cstdio>
#include <memory>
#include <QDir>

#define DEFAULT_LOG_PATH "./LOG/"
#define PRE_FILE_DBG QString(DEFAULT_LOG_PATH)

#define LOG_NUM 50
#define LOG_DEBUG_INDEX 0
#define LOG_VERIFY_MIF_INDEX 1
#define LOG_DLG_FILE_MENU_INDEX 2
#define LOG_TASK_CONSTRUCT_INDEX 3
#define LOG_CENTRAL_WIDGET_INDEX 4
#define LOG_TASK_0X28_PARSING_INDEX 5
#define LOG_TASK_DISPATCH_INDEX 6
#define LOG_TASK_STATISTICS_INDEX 7
#define LOG_TASK_DATA_SEND_INDEX 8
#define LOG_DLG_SINADCFG_MENU_INDEX 9
#define LOG_TASK_CONSTRUCT_ARG_INDEX 10
#define LOG_TASK_DATA_SEND_INFO_INDEX 11
#define LOG_TASK_VERSION_GET_INDEX 12
#define LOG_DLG_MENU_AUDIO_ARG_INDEX 13
#define LOG_TASK_RCV_INDEX 14
#define LOG_MNIC_INDEX 15

#define LOG_UP_RECORD_INDEX 16//上报包信息记录

namespace LWZ{

    class Log{
        std::unique_ptr<FILE*[]> LogFile;
    public:
        Log(int LogNum);
        ~Log();

        void Init(int LogIndex, const char* path);
        void Write(int LogIndex, const char* format, ...);
        void Close(int LogIndex);
    public:
        static Log* LogInstance;
    };
};

#define OPEN_LOG(Index, Path)   LWZ::Log::LogInstance->Init(Index, Path)
#define WRITE_LOG(Index, Format, ...)   LWZ::Log::LogInstance->Write(Index, Format, __VA_ARGS__)
#define CLOSE_LOG(Index)   LWZ::Log::LogInstance->Close(Index)

//调试日志开关
//#define DBG_LOG
#undef DBG_LOG

#if defined DBG_LOG

#define DBGWZ_EXPORTS
#define DBG_FILE_MENU
#define TASK_CONSTRUCT_DBG
#define CENTRAL_WIDGET_DBG
#define TASK_0X28_PARSING_DBG
#define TASK_DISPATCH_DBG
#define TASK_STATISTICS_DBG
#define TASK_DATA_SEND_DBG
#define DLG_SIANDCFG_MENU_DBG
#define TASK_DATA_CONSTRUCT_ARG_DBG
#define TASK_DATA_SEND_INFO_VERIFY
#define TASK_VERSION_GET_DBG
#define DLG_MENU_AUDIO_ARG_DBG
#define TASK_RCV_DBG
#define MNIC_DBG

#else

#undef DBGWZ_EXPORTS
#undef DBG_FILE_MENU
#undef TASK_CONSTRUCT_DBG
#undef CENTRAL_WIDGET_DBG
#undef TASK_0X28_PARSING_DBG
#undef TASK_DISPATCH_DBG
#undef TASK_STATISTICS_DBG
#undef TASK_DATA_SEND_DBG
#undef DLG_SIANDCFG_MENU_DBG
#undef TASK_DATA_CONSTRUCT_ARG_DBG
#undef TASK_DATA_SEND_INFO_VERIFY
#undef TASK_VERSION_GET_DBG
#undef DLG_MENU_AUDIO_ARG_DBG
#undef TASK_RCV_DBG
#undef MNIC_DBG

#endif

#if defined DBGWZ_EXPORTS

#define OPEN_DBG(Index, Path)   LWZ::Log::LogInstance->Init(Index, Path)
#define WRITE_DBG(Index, Format, ...)   LWZ::Log::LogInstance->Write(Index, Format, __VA_ARGS__)
#define CLOSE_DBG(Index)   LWZ::Log::LogInstance->Close(Index)

#define OPEN_VERIFY_MIF(Path)   LWZ::Log::LogInstance->Init(LOG_VERIFY_MIF_INDEX, Path)
#define WRITE_VERIFY_MIF(Format, ...)   LWZ::Log::LogInstance->Write(LOG_VERIFY_MIF_INDEX, Format, __VA_ARGS__)
#define CLOSE_VERIFY_MIF()   LWZ::Log::LogInstance->Close(LOG_VERIFY_MIF_INDEX)

#else

#define OPEN_DBG(Index, Path)
#define WRITE_DBG(Index, Format, ...)
#define CLOSE_DBG(Index)

#define OPEN_VERIFY_MIF(Path)
#define WRITE_VERIFY_MIF(Format, ...)
#define CLOSE_VERIFY_MIF()

#endif // DBGWZ_EXPORTS

#if defined DBG_FILE_MENU

#define OPEN_DLG_FILE_MENU(Path)   LWZ::Log::LogInstance->Init(LOG_DLG_FILE_MENU_INDEX, QString(PRE_FILE_DBG + #Path).toStdString().c_str())
#define WRITE_DLG_FILE_MENU(Format, ...)   LWZ::Log::LogInstance->Write(LOG_DLG_FILE_MENU_INDEX, Format, __VA_ARGS__)
#define CLOSE_DLG_FILE_MENU()   LWZ::Log::LogInstance->Close(LOG_DLG_FILE_MENU_INDEX)

#else

#define OPEN_DLG_FILE_MENU(Path)
#define WRITE_DLG_FILE_MENU(Format, ...)
#define CLOSE_DLG_FILE_MENU()

#endif // DBG_FILE_MENU

#if defined TASK_CONSTRUCT_DBG

#define OPEN_TASK_CONSTRUCT_DBG(path)   LWZ::Log::LogInstance->Init(LOG_TASK_CONSTRUCT_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_CONSTRUCT_DBG(format, ...)   LWZ::Log::LogInstance->Write(LOG_TASK_CONSTRUCT_INDEX, format, __VA_ARGS__)
#define CLOSE_TASK_CONSTRUCT_DBG()  LWZ::Log::LogInstance->Close(LOG_DLG_FILE_MENU_INDEX)

#else

#define OPEN_TASK_CONSTRUCT_DBG(path)
#define WRITE_TASK_CONSTRUCT_DBG(format, ...)
#define CLOSE_TASK_CONSTRUCT_DBG()

#endif//TASK_DBG

#if defined CENTRAL_WIDGET_DBG

#define OPEN_CENTRAL_WIDGWT_DBG(path)   LWZ::Log::LogInstance->Init(LOG_CENTRAL_WIDGET_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_CENTRAL_WIDGET_DBG(format, ...)   LWZ::Log::LogInstance->Write(LOG_CENTRAL_WIDGET_INDEX, format, __VA_ARGS__)
#define CLOSE_CENTRAL_WIDGET_DBG()  LWZ::Log::LogInstance->Close(LOG_CENTRAL_WIDGET_INDEX)

#else

#define OPEN_CENTRAL_WIDGWT_DBG(path)
#define WRITE_CENTRAL_WIDGET_DBG(format, ...)
#define CLOSE_CENTRAL_WIDGET_DBG()

#endif//CENTRAL_WIDGET

#if defined TASK_0X28_PARSING_DBG

#define OPEN_TASK_0X28_PARSING(path)    LWZ::Log::LogInstance->Init(LOG_TASK_0X28_PARSING_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_0X28_PARSING(fmt, ...)   LWZ::Log::LogInstance->Write(LOG_TASK_0X28_PARSING_INDEX, fmt, __VA_ARGS__)
#define CLOSE_TASK_0X28_PARSING()   LWZ::Log::LogInstance->Close(LOG_TASK_0X28_PARSING_INDEX)

#else

#define OPEN_TASK_0X28_PARSING(path)
#define WRITE_TASK_0X28_PARSING(fmt, ...)
#define CLOSE_TASK_0X28_PARSING()

#endif  //TASK_0X28_PARSING

#if defined TASK_DISPATCH_DBG

#define OPEN_TASK_DISPATCH_DBG(path)    LWZ::Log::LogInstance->Init(LOG_TASK_DISPATCH_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_DISPATCH_DBG(fmt, ...)   LWZ::Log::LogInstance->Write(LOG_TASK_DISPATCH_INDEX, fmt, __VA_ARGS__)
#define CLOSE_TASK_DISPATCH_DBG()   LWZ::Log::LogInstance->Close(LOG_TASK_DISPATCH_INDEX)

#else

#define OPEN_TASK_DISPATCH_DBG(path)
#define WRITE_TASK_DISPATCH_DBG(fmt, ...)
#define CLOSE_TASK_DISPATCH_DBG()

#endif  //TASK_DISPATCH_DBG

#if defined TASK_STATISTICS_DBG

#define OPEN_TASK_STATISTICS_DBG(path)  LWZ::Log::LogInstance->Init(LOG_TASK_STATISTICS_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_STATISTICS_DBG(fmt, ...) LWZ::Log::LogInstance->Write(LOG_TASK_STATISTICS_INDEX, fmt, __VA_ARGS__)
#define CLOSE_TASK_STATISTICS_DBG() LWZ::Log::LogInstance->Close(LOG_TASK_STATISTICS_INDEX)

#else

#define OPEN_TASK_STATISTICS_DBG(path)
#define WRITE_TASK_STATISTICS_DBG(fmt, ...)
#define CLOSE_TASK_STATISTICS_DBG()

#endif  //TASK_STATISTICS_DBG

#if defined TASK_DATA_SEND_DBG

#define OPEN_TASK_DATA_SEND_DBG(path)   LWZ::Log::LogInstance->Init(LOG_TASK_DATA_SEND_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_DATA_SEND_DBG(fmt, ...)  LWZ::Log::LogInstance->Write(LOG_TASK_DATA_SEND_INDEX, fmt, __VA_ARGS__)
#define CLOSE_TASK_DATA_SEND_DBG()  LWZ::Log::LogInstance->Close(LOG_TASK_DATA_SEND_INDEX)

#else

#define OPEN_TASK_DATA_SEND_DBG(path)
#define WRITE_TASK_DATA_SEND_DBG(fmt, ...)
#define CLOSE_TASK_DATA_SEND_DBG()

#endif //TASK_DATA_SEND_DBG

#if defined DLG_SIANDCFG_MENU_DBG

#define OPEN_DLG_SINADCFG_MENU_DBG(path) LWZ::Log::LogInstance->Init(LOG_DLG_SINADCFG_MENU_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_DLG_SINADCFG_MENU_DBG(fmt, ...)    LWZ::Log::LogInstance->Write(LOG_DLG_SINADCFG_MENU_INDEX, fmt, __VA_ARGS__)
#define CLOSE_DLG_SINADCFG_MENU_DBG()  LWZ::Log::LogInstance->Close(LOG_DLG_SINADCFG_MENU_INDEX)

#else

#define OPEN_DLG_SINADCFG_MENU_DBG(path) 
#define WRITE_DLG_SINADCFG_MENU_DBG(fmt, ...)
#define CLOSE_DLG_SINADCFG_MENU_DBG()

#endif  //DLG_CFG_MENU_DBG

#if defined TASK_DATA_CONSTRUCT_ARG_DBG

#define OPEN_TASK_CONSTRUCT_ARG_DBG(path)   LWZ::Log::LogInstance->Init(LOG_TASK_CONSTRUCT_ARG_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_CONSTRUCT_ARG_DBG(format, ...)   LWZ::Log::LogInstance->Write(LOG_TASK_CONSTRUCT_ARG_INDEX, format, __VA_ARGS__)
#define CLOSE_TASK_CONSTRUCT_ARG_DBG()  LWZ::Log::LogInstance->Close(LOG_TASK_CONSTRUCT_ARG_INDEX)

#else

#define OPEN_TASK_CONSTRUCT_ARG_DBG(path)
#define WRITE_TASK_CONSTRUCT_ARG_DBG(format, ...)
#define CLOSE_TASK_CONSTRUCT_ARG_DBG()

#endif  //TASK_DATA_CONSTRUCT_ARG_DBG

#if defined TASK_DATA_SEND_INFO_VERIFY

#define OPEN_TASK_DATA_SEND_INFO_VERIFY(path)   LWZ::Log::LogInstance->Init(LOG_TASK_DATA_SEND_INFO_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_DATA_SEND_INFO_VERIFY(buf, len)  for(int i = 0; i < len; i+=4){\
                                                        for(int j = 0; j < 4; j++){\
                                                            LWZ::Log::LogInstance->Write(LOG_TASK_DATA_SEND_INFO_INDEX, "%02x ", buf[i + j] & 0xff);\
                                                        }\
                                                        LWZ::Log::LogInstance->Write(LOG_TASK_DATA_SEND_INFO_INDEX, "\r\n");\
                                                    }\
                                                    LWZ::Log::LogInstance->Write(LOG_TASK_DATA_SEND_INFO_INDEX, "\r\n");
#define CLOSE_TASK_DATA_SEND_INFO_VERIFY()  LWZ::Log::LogInstance->Close(LOG_TASK_DATA_SEND_INFO_INDEX);

#else

#define OPEN_TASK_DATA_SEND_INFO_VERIFY(path)
#define WRITE_TASK_DATA_SEND_INFO_VERIFY(buf, len)
#define CLOSE_TASK_DATA_SEND_INFO_VERIFY()

#endif  //TASK_DATA_SEND_INFO_VERIFY

#if defined TASK_VERSION_GET_DBG

#define OPEN_TASK_VERSION_GET_DBG(path) LWZ::Log::LogInstance->Init(LOG_TASK_VERSION_GET_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_VERSION_GET_DBG(fmt, ...)    LWZ::Log::LogInstance->Write(LOG_TASK_VERSION_GET_INDEX, fmt, __VA_ARGS__)
#define CLOSE_TASK_VERSION_GET_DBG()    LWZ::Log::LogInstance->Close(LOG_TASK_VERSION_GET_INDEX)

#else

#define OPEN_TASK_VERSION_GET_DBG(path)
#define WRITE_TASK_VERSION_GET_DBG(fmt, ...)
#define CLOSE_TASK_VERSION_GET_DBG()

#endif //TASK_VERSION_GET_DBG

#if defined DLG_MENU_AUDIO_ARG_DBG

#define OPEN_DLG_MENU_AUDIO_ARG_DBG(path)    LWZ::Log::LogInstance->Init(LOG_DLG_MENU_AUDIO_ARG_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_DLG_MENU_AUDIO_ARG_DBG(fmt, ...)   LWZ::Log::LogInstance->Write(LOG_DLG_MENU_AUDIO_ARG_INDEX, fmt, __VA_ARGS__)
#define CLOSE_DLG_MENU_AUDIO_ARG_DBG()  LWZ::Log::LogInstance->Close(LOG_DLG_MENU_AUDIO_ARG_INDEX)

#else

#define OPEN_DLG_MENU_AUDIO_ARG_DBG(path)
#define WRITE_DLG_MENU_AUDIO_ARG_DBG(fmt, ...)
#define CLOSE_DLG_MENU_AUDIO_ARG_DBG()

#endif  //DLG_MENU_AUDIO_ARG_DBG

#if defined TASK_RCV_DBG

#define OPEN_TASK_RCV_DBG(path)         LWZ::Log::LogInstance->Init(LOG_TASK_RCV_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_TASK_RCV_DBG(fmt, ...)    LWZ::Log::LogInstance->Write(LOG_TASK_RCV_INDEX, fmt, __VA_ARGS__)
#define CLOSE_TASK_RCV_DBG()            LWZ::Log::LogInstance->Close(LOG_TASK_RCV_INDEX)

#else

#define OPEN_TASK_RCV_DBG(path)
#define WRITE_TASK_RCV_DBG(fmt, ...)
#define CLOSE_TASK_RCV_DBG()

#endif  //TASK_RCV_DBG

#if defined MNIC_DBG

#define OPEN_MNIC_DBG(path)         LWZ::Log::LogInstance->Init(LOG_MNIC_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_MNIC_DBG(fmt, ...)    LWZ::Log::LogInstance->Write(LOG_MNIC_INDEX, fmt, __VA_ARGS__)
#define CLOSE_MNIC_DBG()            LWZ::Log::LogInstance->Close(LOG_MNIC_INDEX)

#else

#define OPEN_MNIC_DBG(path)
#define WRITE_MNIC_DBG(fmt, ...)
#define CLOSE_MNIC_DBG()

#endif // MNIC_DBG

#define LOG_EXPORTS_WZ//日志输出
// #undef LOG_EXPORTS_WZ

#if defined LOG_EXPORTS_WZ

#define LOG_UP_RECORD

#else

#undef LOG_UP_RECORD

#endif // LOG_EXPORTS_WZ

#if defined LOG_UP_RECORD

#define OPEN_LOG_UP_RECORD(path)   LWZ::Log::LogInstance->Init(LOG_UP_RECORD_INDEX, QString(PRE_FILE_DBG + #path).toStdString().c_str())
#define WRITE_LOG_UP_RECORD(fmt, ...)   LWZ::Log::LogInstance->Write(LOG_UP_RECORD_INDEX, fmt, __VA_ARGS__)
#define WRITE_LOG_UP_PARAM_RECORD(...) LWZ::Log::LogInstance->Write(LOG_UP_RECORD_INDEX, "  参数: dB=%.2f, Freq=%llu, Dur=%u, Digital=%d, PGA=%d, Playback=%d, Headset=%d\n", __VA_ARGS__)//新增：日志写参数
#define WRITE_LOG_UP_CODEC_COND_RECORD(...)  LWZ::Log::LogInstance->Write(LOG_UP_RECORD_INDEX, "  Codec SINAD[%d,%d], VppPTP[%d,%d], VppRMS[%d,%d]\n", __VA_ARGS__)//新增：日志写判定条件
#define WRITE_LOG_UP_ADPOW_COND_RECORD(...)  LWZ::Log::LogInstance->Write(LOG_UP_RECORD_INDEX, "  Adpow SINAD[%d,%d], VppPTP[%d,%d], VppRMS[%d,%d]\n", __VA_ARGS__)//新增：日志写判定条件
#define WRITE_LOG_UP_PACK_RECORD(...)   LWZ::Log::LogInstance->Write(LOG_UP_RECORD_INDEX, "\n测试次数：%4d, 时标:%5d\n", __VA_ARGS__)
#define WRITE_LOG_UP_CHIP_RECORD(...)   LWZ::Log::LogInstance->Write(LOG_UP_RECORD_INDEX, "  芯片编号：J[%d]S[%d]\n    Codec Left [Sinad:%4d, Vpp:%5d, Rms:%5d, Res:%s],\n    Codec Right[Sinad:%4d, Vpp:%5d, Rms:%5d, Res:%s],\n    AdPow      [Sinad:%4d, Vpp:%5d, Rms:%5d, Res:%s]\n", __VA_ARGS__)
#define CLOSE_LOG_UP_RECORD()   LWZ::Log::LogInstance->Close(LOG_UP_RECORD_INDEX)

#else

#endif // LOG_UP_RECORD

#endif  // _LOGWZ_HPP_