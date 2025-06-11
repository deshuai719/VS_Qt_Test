#ifndef _CONFIGINIT_HPP_
#define _CONFIGINIT_HPP_

#include <QSettings>
#include <QString>
#include <memory>
#include <QDir>

#define DEFAULT_CFG_PATH "."
#define INI_FILE_NUM 10

#define INI_FILE_NET_CFG "net.ini"
#define INI_FILE_ARG_RECORD "argRecord.ini"
#define INI_FILE_CENTRALIZE "configure.ini"
namespace CFGI{
    enum INI_TYPE{
        INI_NET_CFG = 0,
        INI_ARG_RECORD,
        INI_CENTRALIZE
    };

    class IniFileCFG{
        std::unique_ptr<QSettings*[]> IniFile;
    public:
        IniFileCFG();
        ~IniFileCFG();

        void CreateFile(INI_TYPE, QString fileName);
        void WriteINI(INI_TYPE type, QString key, QString value);
        QVariant ReadINI(INI_TYPE type, QString key);
        void ClearINI(INI_TYPE type);
        void Clear();
    };

    extern std::unique_ptr<IniFileCFG> IniFileCFGGlobal;
};

#endif //   _CONFIGINIT_HPP_