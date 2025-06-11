#include "FileParsing.hpp"

FPWZ::AddrMif::AddrMif()
    : AddrBeg(0), AddrEnd(0)
{}

FPWZ::AddrMif::AddrMif(long long beg, long long end)
    : AddrBeg(beg), AddrEnd(end)
{}

FPWZ::AddrMif::AddrMif(const AddrMif& addr)
    : AddrBeg(addr.GetAddrBeg()), AddrEnd(addr.GetAddrEnd())
{}

FPWZ::AddrMif::~AddrMif()
{}

void FPWZ::AddrMif::operator=(const AddrMif& addr)
{
    AddrBeg = addr.GetAddrBeg();
    AddrEnd = addr.GetAddrEnd();
}

void FPWZ::AddrMif::SetAddr(long long beg, long long end)
{
    AddrBeg = beg;
    AddrEnd = end;
}

long long FPWZ::AddrMif::GetAddrBeg() const
{
    return AddrBeg;
}

long long FPWZ::AddrMif::GetAddrEnd() const
{
    return AddrEnd;
}

//

FPWZ::FileParsingMif::FileParsingMif()
    : FilePath(""), File(), Buffer(nullptr), FileDataSize(0), Depth(0), Width(0), ByteWidth(0), DataType(HEX), AddrType(HEX)
{}

FPWZ::FileParsingMif::~FileParsingMif()
{
    Clear();
}

void FPWZ::FileParsingMif::Init(QString path)
{
    Clear();
    FilePath = path;
}

void FPWZ::FileParsingMif::Clear()
{
    FilePath.clear();
    if(File.is_open())
    {
        File.close();
    }
    if(Buffer != nullptr)
    {
        Buffer.reset();
        Buffer = nullptr;
    }
    FileDataSize = 0;
    Depth = 0;
    Width = 0;
    ByteWidth = 0;
    DataType = HEX;
    AddrType = HEX;
}

void FPWZ::FileParsingMif::Parse()
{
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::Parse() ENTER\n");
    std::unique_ptr<char[]> FileBuffer(nullptr);
    long long FileSize = ReadFile(FileBuffer);
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::Parse() FileSize: %lld\n", FileSize);
    if(FileBuffer == nullptr)
    {
        return;
    }

    long long LineBeg = 0;
    for(int i = 0; i < FileSize;)
    {
        switch (FileBuffer[i])
        {
        case '\n':
            i = LineParsing(FileBuffer, LineBeg, i);
            LineBeg = ++i;
            break;
        case '\r\n':
            i = LineParsing(FileBuffer, LineBeg, i);
            LineBeg = ++i;
            break;
        default:
            i++;
            break;
        }
    }
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::Parse() EXIT\n");
}

long long FPWZ::FileParsingMif::ReadFile(std::unique_ptr<char[]>& buffer)
{
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::ReadFile() ENTER\n");
    File.open(FilePath.toStdString(), std::ios::in | std::ios::binary);
    if(!File.is_open())
    {
        WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::ReadFile() EXIT, File is not open\n");
        return 0;
    }
    File.seekg(0, std::ios::end);
    long long FileSize = File.tellg();
    File.seekg(0, std::ios::beg);
    buffer = std::make_unique<char[]>(FileSize);
    File.read(buffer.get(), FileSize);
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::ReadFile() EXIT, FileSize: %lld\n", FileSize);
    return FileSize;
}

long long FPWZ::FileParsingMif::LineParsing(std::unique_ptr<char[]>& buffer, long long Beg, long long End)
{
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() ENTER, Beg = %lld, End = %lld\n", Beg, End);
    long long i = Beg;
    for(; i < End;)
    {
        switch (buffer[i])
        {
        case '\n':
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() EXIT, \'\\n\'\n");
            i = End;
            break;
        case '\r\n':
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() EXIT, \'\\r\\n\'\n");
            i = End;
            break;
        case '-':
            if(buffer[i + 1] == '-')
            {
                WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() EXIT, \'-\'\n");
                i = End;
            }
            else
            {
                i++;
            }
            break;
        case 'A':
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() ENTER \'A\'\n");
            i = GetDataType(buffer, i + 13, AddrType);
            WRITE_DBG(LOG_DEBUG_INDEX, "AddrType: %d\n", AddrType);
            break;
        case 'B'://
            {                
                WRITE_DBG(LOG_DEBUG_INDEX, "DataParing Before: %lld\n", i);
                long long j = i + 5;
                for(; buffer[j] != '\n' && buffer[j] != '\r\n'; j++);
                i = DataParsing(buffer, j + 1);
                // WRITE_DBG(LOG_DEBUG_INDEX, "DataParing After: %lld\n", i);
            }
            break;
        case 'C':
            i+=7;
            break;
        case 'D':
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() ENTER \'D\'\n");
            switch (buffer[i + 1])
            {
            case 'A':
                i = GetDataType(buffer, i + 11, DataType);
                WRITE_DBG(LOG_DEBUG_INDEX, "DataType: %d\n", DataType);
                break;
            case 'E':
                i = GetDepth(buffer, i + 6, Depth);
                WRITE_DBG(LOG_DEBUG_INDEX, "Depth: %lld\n", Depth);
                break;
            default:
                break;
            }
            break;
        case 'E':
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() EXIT \'E\', i = %lld, buffer[i + 1] = %c, buffer[i + 2] = %c, buffer[i + 3] = %c\n", 
                i, buffer[i + 1], buffer[i + 2], buffer[i + 3]);
            i = End;
            break;
        case 'W':
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() ENTER \'W\'\n");
            i = GetWidth(buffer, i + 6, Width);
            WRITE_DBG(LOG_DEBUG_INDEX, "Width: %lld\n", Width);
            break;
        default:
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing(), default\n");
            i++;
            break;
        }
    }
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::LineParsing() EXIT i = %lld\n", i);
    return i;
}

long long FPWZ::FileParsingMif::DataParsing(std::unique_ptr<char[]>& buffer, long long Beg)
{
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::DataParsing() ENTER\n");
    if(Depth == 0 || Width == 0)
    {
        return 0;
    }
    if(Width % 8)
    {
        ByteWidth = Width / 8 + 1;        
        FileDataSize = Depth * ByteWidth;
    }
    else
    {
        ByteWidth = Width / 8;
        FileDataSize = Depth * ByteWidth;
    }
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::DataParsing() FileDataSize: %lld\n", FileDataSize);
    Buffer = std::make_unique<char[]>(FileDataSize);
    for(long long i = 0; i < FileDataSize; i++)
    {
        Buffer[i] = 0;
    }
    long long LineBeg = Beg;
    long long i = Beg;
    for(; ; )
    {
        switch (buffer[i])
        {
        case '\n':
            i = DataLineParsing(buffer, LineBeg, i);
            LineBeg = ++i;
            break;
        case '\r\n':
            i = DataLineParsing(buffer, LineBeg, i);
            LineBeg = ++i;
            break;
        case 'E':
            if(buffer[i + 1] == 'N' && buffer[i + 2] == 'D')
            {
                WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::DataParsing() EXIT: %lld\n", i);
                return i - 1;
            }
            else
            {
                i++;
            }
        default:
            i++;
            break;
        }
    }
}

long long FPWZ::FileParsingMif::DataLineParsing(std::unique_ptr<char[]>& buffer, long long Beg, long long End)
{
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::DataLineParsing() ENTER\n");
    long long i = Beg;
    for(; i < End;)
    {
        switch (buffer[i])
        {
        case '\n':
            i = End;
            break;
        case '\r\n':
            i = End;
            break;
        case '-':
            if(buffer[i + 1] == '-')
            {
                for(; buffer[i] != '\n' && buffer[i] != '\r\n'; i++);
            }
            else
            {
                i++;
            }
            break;
        case ':':
        {
            AddrMif addr = GetAddr(buffer, Beg);
            i = GetData(buffer, i + 1, addr);
            if(addr.GetAddrBeg() == addr.GetAddrEnd())
            {
                WRITE_VERIFY_MIF("%04X :", addr.GetAddrBeg());
                for(int i = 0; i < ByteWidth; i++)
                {
                    WRITE_VERIFY_MIF("%02X", (unsigned char)Buffer[addr.GetAddrBeg() * ByteWidth + (ByteWidth - 1 - i)]);
                }
                WRITE_VERIFY_MIF("\n");
            }
            else
            {
                WRITE_VERIFY_MIF("[%04X ... %04X] :", addr.GetAddrBeg(), addr.GetAddrEnd());
                for(int i = 0; i < ByteWidth; i++)
                {
                    WRITE_VERIFY_MIF("%02X", (unsigned char)Buffer[addr.GetAddrBeg() * ByteWidth + (ByteWidth - 1 - i)]);
                }
                WRITE_VERIFY_MIF("\n");
            }
        }
            break;
        default:
            i++;
            break;
        }
    }
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::DataLineParsing() EXIT\n");
    return i;
}

FPWZ::AddrMif FPWZ::FileParsingMif::GetAddr(std::unique_ptr<char[]>& buffer, long long Beg)
{
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetAddr() ENTER\n");
    long long AddrBeg = 0;
    long long AddrEnd = 0;
    long long AddrBegL = 0;//左地址起始位置
    long long AddrEndL = 0;//左地址结束位置 + 1
    long long AddrBegR = 0;//右地址起始位置
    long long AddrEndR = 0;//右地址结束位置 + 1
    for(long long i = Beg; buffer[i] != ':'; i++)
    {
        if(buffer[i] == '[')
        {
            for(AddrBegL = i + 1; buffer[AddrBegL] == ' ' || buffer[AddrBegL] == '\t'; AddrBegL++);
            for(AddrEndL = AddrBegL + 1; buffer[AddrEndL] != '.' && buffer[AddrEndL] != ' ' && buffer[AddrEndL] != '\t'; AddrEndL++);
            for(AddrBegR = AddrEndL + 1; buffer[AddrBegR] == '.' || buffer[AddrBegR] == ' ' || buffer[AddrBegR] == '\t'; AddrBegR++);
            for(AddrEndR = AddrBegR + 1; buffer[AddrEndR] != ']' && buffer[AddrEndR] != ' ' && buffer[AddrEndR] != '\t'; AddrEndR++);//找到左右范围地址的位置
            WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetAddr() AddrBegL = %lld, AddrEndL = %lld, AddrBegR = %lld, AddrEndR = %lld\n", AddrBegL, AddrEndL, AddrBegR, AddrEndR);
            switch (AddrType)
            {
            case HEX:
                AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL), nullptr, 16);
                AddrEnd = std::stoll(std::string(buffer.get() + AddrBegR, AddrEndR - AddrBegR), nullptr, 16);
                WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetAddr() EXIT\n");
                return AddrMif(AddrBeg, AddrEnd);
            case DEC:
                AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL));
                AddrEnd = std::stoll(std::string(buffer.get() + AddrBegR, AddrEndR - AddrBegR));
                return AddrMif(AddrBeg, AddrEnd);
            case BIN:
                AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL), nullptr, 2);
                AddrEnd = std::stoll(std::string(buffer.get() + AddrBegR, AddrEndR - AddrBegR), nullptr, 2);
                return AddrMif(AddrBeg, AddrEnd);
            case OCT:
                AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL), nullptr, 8);
                AddrEnd = std::stoll(std::string(buffer.get() + AddrBegR, AddrEndR - AddrBegR), nullptr, 8);
                return AddrMif(AddrBeg, AddrEnd);
            default:
                break;
            }

        }
    }
    for(AddrBegL = Beg; buffer[AddrBegL] == ' ' || buffer[AddrBegL] == '\t'; AddrBegL++);
    for(AddrEndL = AddrBegL + 1; buffer[AddrEndL] != ':' && buffer[AddrEndL] != ' ' && buffer[AddrEndL] != '\t'; AddrEndL++);
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetAddr() AddrBegL = %lld, AddrEndL = %lld\n", AddrBegL, AddrEndL);
    switch (AddrType)
    {
    case HEX:
        AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL), nullptr, 16);
        WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetAddr() EXIT\n");
        return AddrMif(AddrBeg, AddrBeg);
    case DEC:
        AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL));
        return AddrMif(AddrBeg, AddrBeg);
    case BIN:
        AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL), nullptr, 2);
        return AddrMif(AddrBeg, AddrBeg);
    case OCT:
        AddrBeg = std::stoll(std::string(buffer.get() + AddrBegL, AddrEndL - AddrBegL), nullptr, 8);
        return AddrMif(AddrBeg, AddrBeg);
    default:
        break;
    }
    // WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetAddr() EXIT\n");
}

long long FPWZ::FileParsingMif::GetData(std::unique_ptr<char[]>& buffer, long long Beg, AddrMif addr)
{
    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetData() ENTER\n");
    long long i = Beg;
    long long DataBeg = Beg;//数据起始位置
    long long DataEnd = Beg + 1;//数据结束位置 + 1
    long long AddrInc = 0;//地址增量
    for(; ; )
    {
        switch (buffer[i])
        {   
        case ';':
        {
            for(; buffer[DataBeg] == ' ' || buffer[DataBeg] == '\t' || buffer[DataBeg] == ','; DataBeg++);//找到数据起始位置
            for(DataEnd = DataBeg + 1; buffer[DataEnd] != ' ' && buffer[DataEnd] != '\t' && buffer[DataEnd] != ';'; DataEnd++);//找到数据结束位置
            switch (DataType)
            {
            case HEX:
            {//十六进制存储到Buffer中
                WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetData(), DataBeg = %lld, DataEnd = %lld\n", DataBeg, DataEnd);
                for(long long j = DataBeg; j < DataEnd; j++)
                {//将数据存储到Buffer中
                    WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetData(), addr.GetAddrBeg() = %lld, addr.GetAddrEnd() = %lld, AddrInc = %lld, ByteWidth = %d, j = %d\n", 
                        addr.GetAddrBeg(), addr.GetAddrEnd(), AddrInc, ByteWidth, j);
                    for(long long k = addr.GetAddrBeg() + AddrInc; k < addr.GetAddrEnd() + 1; k++)
                    {//将数据存储到Buffer中
                        Buffer[k * ByteWidth + (j - DataBeg) / 2] |= TOOLWZ::CharToHex(buffer[DataEnd - (j - DataBeg) - 1]) << (((j - DataBeg) % 2) * 4);
                        WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetData(), k = %lld, Buffer[%lld] = buffer[%lld] = %d\n", 
                            k, k * ByteWidth + (j - DataBeg) / 2, DataEnd - (j - DataBeg) - 1, Buffer[k * ByteWidth + (j - DataBeg) / 2]);
                    }
                }
            }
                break;
            case DEC:
            {//十进制转十六进制，存储到Buffer中
                std::unique_ptr<char[]> BuffDec(new char[DataEnd - DataBeg]);
                for(long long j = DataBeg; j < DataEnd; j++)
                {//将数据存储到BuffDec中
                    BuffDec[j - DataBeg] = TOOLWZ::CharToHex(buffer[DataBeg]);
                }
                std::unique_ptr<char[]> BuffHex(new char[DataEnd - DataBeg]);
                //将BuffDec中的数据转为十六进制存储到BuffHex中
                unsigned long long HexLen = TOOLWZ::DecToHexBigInteger((unsigned char*)BuffDec.get(), DataEnd - DataBeg, (unsigned char*)BuffHex.get());  
                for(long long j = 0; j < HexLen; j++)
                {//将BuffHex中的数据存储到Buffer中
                    for(long long k = addr.GetAddrBeg() + AddrInc; k < addr.GetAddrEnd() + 1; k++)
                    {//将数据存储到Buffer中
                        Buffer[k * ByteWidth + j / 2] |= BuffHex[j] << ((j % 2) * 4);
                    }
                }                   
            }
                break;
            case BIN:
                /* code */
                break;
            case OCT:
                /* code */
                break;
            default:
                break;
            }
        }
            // WRITE_DBG(LOG_DEBUG_INDEX, "FileParsingMif::GetData() EXIT, ADDR = %lld, Ret i = %lld\n", addr.GetAddrBeg(), i);
            return i;
        case ',':
            for (; buffer[DataBeg] == ' ' || buffer[DataBeg] == '\t' || buffer[DataBeg] == ','; DataBeg++);
            for (DataEnd = DataBeg; buffer[DataEnd] != ' ' && buffer[DataEnd] != '\t' && buffer[DataEnd] != ','; DataEnd++);
            switch (DataType)
            {
            case HEX:
                for(long long j = DataBeg; j < DataEnd; j++)
                {//将数据存储到Buffer中
                    Buffer[(addr.GetAddrBeg() + AddrInc) * ByteWidth + (j - DataBeg) / 2] |= TOOLWZ::CharToHex(buffer[DataEnd - (j - DataBeg) - 1]) << (((j - DataBeg) % 2) * 4);
                }
                break;
            case DEC:
            {
                std::unique_ptr<char[]> BuffDec(new char[DataEnd - DataBeg]);
                for(long long j = DataBeg; j < DataEnd; j++)
                {//将数据存储到BuffDec中
                    BuffDec[j - DataBeg] = TOOLWZ::CharToHex(buffer[DataBeg]);
                }
                std::unique_ptr<char[]> BuffHex(new char[DataEnd - DataBeg]);
                //将BuffDec中的数据转为十六进制存储到BuffHex中
                unsigned long long HexLen = TOOLWZ::DecToHexBigInteger((unsigned char*)BuffDec.get(), DataEnd - DataBeg, (unsigned char*)BuffHex.get());  
                for(long long j = 0; j < HexLen; j++)
                {//将BuffHex中的数据存储到Buffer中
                    Buffer[(addr.GetAddrBeg() + AddrInc) * ByteWidth + j / 2] |= BuffHex[j] << ((j % 2) * 4);
                }
            }
                break;
            case BIN:
                /* code */
                break;
            case OCT:
                /* code */
                break;
            default:
                break;
            }
            DataBeg = DataEnd + 1;
            DataEnd = DataBeg + 1;
            AddrInc++;
            i++;
            break;
        default:    
            i++;
            break;
        }
    }
}

long long FPWZ::FileParsingMif::GetDepth(std::unique_ptr<char[]>& buffer, long long Beg, long long& Dep)
{
    long long DataBeg = Beg;
    long long DataEnd = Beg + 1;
    
    for(; buffer[DataBeg] == ' ' || buffer[DataBeg] == '\t' || buffer[DataBeg] == '='; DataBeg++);
    for(DataEnd = DataBeg; buffer[DataEnd] != ' ' && buffer[DataEnd] != '\t' && buffer[DataEnd] != ';'; DataEnd++);
    Dep = std::stoll(std::string(buffer.get() + DataBeg, DataEnd - DataBeg));
    return DataEnd;
}

long long FPWZ::FileParsingMif::GetWidth(std::unique_ptr<char[]>& buffer, long long Beg, long long& Wid)
{
    return GetDepth(buffer, Beg, Wid);
}

long long FPWZ::FileParsingMif::GetDataType(std::unique_ptr<char[]>& buffer, long long Beg, FileDataType& Typ)
{
    long long DataBeg = Beg;
    long long DataEnd = Beg + 1;
    for(; buffer[DataBeg] == ' ' || buffer[DataBeg] == '\t' || buffer[DataBeg] == '='; DataBeg++);
    for(DataEnd = DataBeg; buffer[DataEnd] != ' ' && buffer[DataEnd] != '\t' && buffer[DataEnd] != ';'; DataEnd++);
    switch (buffer[DataBeg])
    {
    case 'H':
        Typ = HEX;
        break;
    case 'D':
        Typ = DEC;
        break;
    case 'B':
        Typ = BIN;
        break;
    case 'O':
        Typ = OCT;
        break;
    default:
        break;
    }
    return DataEnd;
}

FPWZ::ArgDM::ArgDM(){}

FPWZ::ArgDM::ArgDM(QString p, int d):Path(p), Duration(d){}

FPWZ::ArgDM::ArgDM(const ArgDM& a):Path(a.GetPath()), Duration(a.GetDuration()){}

void FPWZ::ArgDM::operator=(const ArgDM& a)
{
    Path = a.GetPath();
    Duration = a.GetDuration();
}

bool FPWZ::ArgDM::operator==(const ArgDM& a) const
{
    return Path == a.GetPath() && Duration == a.GetDuration();
}

QString FPWZ::ArgDM::GetPath() const
{
    return Path;
}

int FPWZ::ArgDM::GetDuration() const
{
    return Duration;
}