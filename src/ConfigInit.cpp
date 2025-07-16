#include "ConfigInit.hpp"

namespace CFGI{
    IniFileCFG::IniFileCFG(){
        IniFile = std::make_unique<QSettings*[]>(INI_FILE_NUM);
        for(int i = 0; i < INI_FILE_NUM; i++)
        {
            IniFile[i] = nullptr;
        }
        QDir dir;
        if (!dir.exists(DEFAULT_CFG_PATH)){
            dir.mkdir(DEFAULT_CFG_PATH);
        }
        // CreateFile(INI_NET_CFG, INI_FILE_NET_CFG);
        // CreateFile(INI_ARG_RECORD, INI_FILE_ARG_RECORD);
        CreateFile(INI_CENTRALIZE, INI_FILE_CENTRALIZE);
        // WriteINI(INI_NET_CFG, "LocalIP", "192.168.210.11");
        // WriteINI(INI_NET_CFG, "LocalPort", "8254");
        // WriteINI(INI_NET_CFG, "DeviceIP", "192.168.210.210");
        // WriteINI(INI_NET_CFG, "DevicePort", "8204");
    }

    IniFileCFG::~IniFileCFG(){
        Clear();
        IniFile.reset();
    }

    void IniFileCFG::CreateFile(INI_TYPE type, QString fileName){
        if(type < INI_FILE_NUM && IniFile[type] == nullptr)
        {
            IniFile[type] = new QSettings(DEFAULT_CFG_PATH + fileName, QSettings::IniFormat);
        }
    }

    void IniFileCFG::WriteINI(INI_TYPE type, QString key, QString value)
    {
        if(type < INI_FILE_NUM && IniFile[type] != nullptr)
            IniFile[type]->setValue(key, value);
    }

    QVariant IniFileCFG::ReadINI(INI_TYPE type, QString key){
        if(type < INI_FILE_NUM && IniFile[type] != nullptr)
            return IniFile[type]->value(key);
        else
            return QVariant();
    }

    void IniFileCFG::ClearINI(INI_TYPE type)
    {
        IniFile[type]->clear();
    }

    void IniFileCFG::Clear()
    {
        for(int i = 0; i < INI_FILE_NUM; i++)
        {
            if(IniFile[i] != nullptr)
            {
                delete IniFile[i];
                IniFile[i] = nullptr;
            }
        }
    }

    std::unique_ptr<IniFileCFG> IniFileCFGGlobal = std::make_unique<IniFileCFG>();
};