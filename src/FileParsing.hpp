#ifndef _FILEPARSING_HPP_
#define _FILEPARSING_HPP_

#include <QString>
#include <memory>
#include <fstream>
#include "ToolWZ.hpp"
#include "LogWZ.hpp"

namespace FPWZ{

    enum FileDataType{
        HEX,
        DEC,
        BIN,
        OCT,
    };

    class AddrMif{
        long long AddrBeg; //地址
        long long AddrEnd; //地址
    public:
        AddrMif();
        AddrMif(long long beg, long long end);
        AddrMif(const AddrMif& addr);
        ~AddrMif();

        void operator=(const AddrMif& addr);
        void SetAddr(long long beg, long long end);
        long long GetAddrBeg() const;
        long long GetAddrEnd() const;
    };

    class FileParsingMif{
        QString FilePath;
        std::ifstream File;//文件流
        std::unique_ptr<char[]> Buffer;//存储解析后的有效数据
        unsigned long long FileDataSize;//文件有效数据字节长度
        long long Depth;//mif文件深度
        long long Width;//mif文件宽度,bit宽度
        int ByteWidth;//mif文件宽度字节数
        FileDataType DataType;//mif文件数据类型
        FileDataType AddrType;//mif文件地址类型
    public:
        FileParsingMif();
        ~FileParsingMif();

        void Init(QString path);
        void Clear();
        void Parse();
    private:
        long long ReadFile(std::unique_ptr<char[]>& buffer);
        long long LineParsing(std::unique_ptr<char[]>& buffer, long long Beg, long long End);
        long long DataParsing(std::unique_ptr<char[]>& buffer, long long Beg);
        long long DataLineParsing(std::unique_ptr<char[]>& buffer, long long Beg, long long End);
        AddrMif GetAddr(std::unique_ptr<char[]>& buffer, long long Beg);
        long long GetData(std::unique_ptr<char[]>& buffer, long long Beg, AddrMif addr);
        long long GetDepth(std::unique_ptr<char[]>& buffer, long long Beg, long long& Dep);
        long long GetWidth(std::unique_ptr<char[]>& buffer, long long Beg, long long& Wid);
        long long GetDataType(std::unique_ptr<char[]>& buffer, long long Beg, FileDataType& Typ);
    public:
        inline QString GetFilePath() const{
            return FilePath;
        }

        inline const std::unique_ptr<char[]>& GetBuffer() const{
            return Buffer;
        }

        inline unsigned long long GetFileDataSize() const{
            return FileDataSize;
        }

        inline long long GetDepth() const{
            return Depth;
        }

        inline long long GetWidth() const{
            return Width;
        }

        inline int GetByteWidth() const{
            return ByteWidth;
        }
    };

    class ArgDM{
        QString Path;
        int Duration;//s
    public:
        ArgDM();
        ArgDM(QString p, int d);
        ArgDM(const ArgDM& a);

        void operator=(const ArgDM& a);
        bool operator==(const ArgDM& a) const;

        QString GetPath() const;
        int GetDuration() const;
    };
};

#endif // _FILEPARSING_HPP_