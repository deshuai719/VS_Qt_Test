#ifndef _DATA_CONSTRUCT_HPP_
#define _DATA_CONSTRUCT_HPP_

#include "DataFrameWz.hpp"
#include "FileParsing.hpp"
#include "SamplingDataGenerate.hpp"

/*
    下发数据构造
*/

#define PACK_DATA_LEN 1024

namespace DCWZ{

    struct SegInfo{
        long long Start;
        long long Len;
    public:
        SegInfo();
        SegInfo(long long Start, long long Len);
        SegInfo(const SegInfo& Seg);
        ~SegInfo();

        void operator=(const SegInfo& Seg);
        void SetSeg(long long Start, long long Len);
        long long GetStart() const;
        long long GetLen() const;
    };

    class PackInfo{
        std::unique_ptr<char[]> PackData;
        SegInfo SegAll;
        SegInfo SegHead;
        SegInfo SegFrame;
        SegInfo SegData;
    public:
        PackInfo();
        ~PackInfo();

        void Init(long long Len);
        void Clear();
        void SetSegAll(SegInfo Seg);
        void SetSegHead(SegInfo Seg);
        void SetSegFrame(SegInfo Seg);
        void SetSegData(SegInfo Seg);
        SegInfo GetSegAll() const;
        SegInfo GetSegHead() const;
        SegInfo GetSegFrame() const;
        SegInfo GetSegData() const;
        char* GetPackData() const;
    };

    //寄存器配置参数
    template<typename T = unsigned char>
    class ArgRegCFG{
        T DigitalLeft;
        T DigitalRight;
        T AnalogLeft;
        T AnalogRight;
    public:
        ArgRegCFG();
        ArgRegCFG(T dl, T dr, T al, T ar);
        ArgRegCFG(const ArgRegCFG<T>& a);
        ~ArgRegCFG();

        void operator=(const ArgRegCFG<T>& a);
        bool operator==(const ArgRegCFG<T>& a) const;
        inline void SetDL(T dl);
        inline void SetDR(T dr);
        inline void SetAL(T al);
        inline void SetAR(T ar);
        inline T GetDL() const;
        inline T GetDR() const;
        inline T GetAL() const;
        inline T GetAR() const;
    };

    template<typename T>
    ArgRegCFG<T>::ArgRegCFG()
        :DigitalLeft(0), DigitalRight(0), AnalogLeft(0), AnalogRight(0)
    {}

    template<typename T>
    ArgRegCFG<T>::ArgRegCFG(T dl, T dr, T al, T ar)
        :DigitalLeft(dl), DigitalRight(dr), AnalogLeft(al), AnalogRight(ar)
    {}

    template<typename T>
    ArgRegCFG<T>::ArgRegCFG(const ArgRegCFG<T>& a)
        :DigitalLeft(a.DigitalLeft), DigitalRight(a.DigitalRight), AnalogLeft(a.AnalogLeft), AnalogRight(a.AnalogRight)
    {}

    template<typename T>
    ArgRegCFG<T>::~ArgRegCFG()
    {}

    template<typename T>
    void ArgRegCFG<T>::operator=(const ArgRegCFG<T>& a)
    {
        DigitalLeft = a.GetDL();
        DigitalRight = a.GetDR();
        AnalogLeft = a.GetAL();
        AnalogRight = a.GetAR();
    }

    template<typename T>
    bool ArgRegCFG<T>::operator==(const ArgRegCFG<T>& a) const
    {
        return DigitalLeft == a.GetDL() && DigitalRight == a.GetDR() && AnalogLeft == a.GetAL() && AnalogRight == a.GetAR();
    }

    template<typename T>
    void ArgRegCFG<T>::SetDL(T dl)
    {
        DigitalLeft = dl;
    }

    template<typename T>
    void ArgRegCFG<T>::SetDR(T dr)
    {
        DigitalRight = dr;
    }

    template<typename T>
    void ArgRegCFG<T>::SetAL(T al)
    {
        AnalogLeft = al;
    }

    template<typename T>
    void ArgRegCFG<T>::SetAR(T ar)
    {
        AnalogRight = ar;
    }

    template<typename T>
    T ArgRegCFG<T>::GetDL() const
    {
        return DigitalLeft;
    }

    template<typename T>
    T ArgRegCFG<T>::GetDR() const
    {
        return DigitalRight;
    }

    template<typename T>
    T ArgRegCFG<T>::GetAL() const
    {
        return AnalogLeft;
    }

    template<typename T>
    T ArgRegCFG<T>::GetAR() const
    {
        return AnalogRight;
    }

//数据构造基类

    class DataConstruct{
        ArgRegCFG<unsigned char> RegCFG;//寄存器配置参数，用于配置在音频数据之前下发的a0配置命令
    public:
        DataConstruct(ArgRegCFG<unsigned char> a = ArgRegCFG<unsigned char>());
        ~DataConstruct();

        virtual void CLear() = 0;
        virtual void Construct() = 0;
        virtual void Construct(int start) = 0;
        virtual const FPWZ::FileParsingMif& GetFP() const = 0;
        virtual const SDG::SamplingDataGenerate& GetSDG() const = 0;
        virtual PackInfo& GetPackInfo(long long Index) = 0;
        virtual long long GetPackNum() const = 0;//数据包数
        virtual long long GetSendNum() const = 0;//发送包数
        virtual int GetDuration() const = 0;
        const ArgRegCFG<unsigned char>& GetRegCFG() const;
        void UpdateCfgCMD();
        static const char const* GetRegCfgCMD();
        static void InitCmdRegCFG();

        static unsigned int CmdRegCFG[0x52];
    };

    class DC_DN_MNIC_64K : public DataConstruct{
    private:
        FPWZ::FileParsingMif FP;
        std::unique_ptr<PackInfo[]> PackInfoArr;
        long long PackNum;
        FPWZ::ArgDM Arg;
        unsigned SendCnt;
    public:
        DC_DN_MNIC_64K();
        DC_DN_MNIC_64K(FPWZ::ArgDM a);
        ~DC_DN_MNIC_64K();

        virtual void CLear() override;
        virtual void Construct() override;
        virtual void Construct(int start) override;
        virtual const FPWZ::FileParsingMif& GetFP() const override;
        virtual const SDG::SamplingDataGenerate& GetSDG() const;
        virtual PackInfo& GetPackInfo(long long Index) override;
        virtual long long GetPackNum() const override;
        virtual long long GetSendNum() const override;
        virtual int GetDuration() const override;//s
    };

    class ARG_RTC_GENERATE{
    private:
        SDG::ARG ArgAudio;//音频参数
        ArgRegCFG<unsigned char> RegCFG;//寄存器配置参数
    public:
        ARG_RTC_GENERATE();
        ARG_RTC_GENERATE(SDG::ARG a, ArgRegCFG<unsigned char> regCfg);
        ARG_RTC_GENERATE(const ARG_RTC_GENERATE& cp);
        ~ARG_RTC_GENERATE();

        void operator=(const ARG_RTC_GENERATE& as);
        bool operator==(const ARG_RTC_GENERATE& as) const;
        void SetAudioARG(SDG::ARG a);
        SDG::ARG GetAudioARG() const;
        void SetRegcfgARG(ArgRegCFG<unsigned char> regCfg);
        ArgRegCFG<unsigned char> GetRegCfgARG() const;
    };

    class DC_DN_MNIC_64K_RCT_GENERATE: public DataConstruct{
    private:
        SDG::SamplingDataGenerate SampDataGenerator;
        std::unique_ptr<PackInfo> PacksBuffer;
        double Amp;//振幅
        long long StepLen;//步长
        int SendPacksNum;//发送包数目
        int PackCnt;//当前正在构造的包计数
    public:
        DC_DN_MNIC_64K_RCT_GENERATE();
        DC_DN_MNIC_64K_RCT_GENERATE(ARG_RTC_GENERATE a);
        ~DC_DN_MNIC_64K_RCT_GENERATE();

        virtual void CLear() override;
        virtual void Construct() override;
        virtual void Construct(int start) override;
        virtual const FPWZ::FileParsingMif& GetFP() const override;
        virtual const SDG::SamplingDataGenerate& GetSDG() const;
        virtual PackInfo& GetPackInfo(long long Index) override;
        virtual long long GetPackNum() const override;
        virtual long long GetSendNum() const override;
        virtual int GetDuration() const override;
    };

    //

    class DataNode{
    private:
        std::unique_ptr<DataConstruct> DC;
        std::shared_ptr<DataNode> Next;
    public:
        DataNode();
        ~DataNode();

        void Clear();
        void ClearDATA();
        void SetData(std::unique_ptr<DataConstruct>& DC);
        void SetNext(std::shared_ptr<DataNode>& Node);
        void SetNext(const std::shared_ptr<DataNode>& Node);
        const std::unique_ptr<DataConstruct>& GetData() const;
        const std::shared_ptr<DataNode>& GetNext() const;
    };

    class DataList{
    private:
        std::shared_ptr<DataNode> Head;
        std::shared_ptr<DataNode> Tail;
    public:
        DataList();
        ~DataList();

        void AddNode(std::unique_ptr<DataConstruct>& DC);
        void AddNode(FPWZ::ArgDM a);
        void DelNode(FPWZ::ArgDM a);
        void AddNode(ARG_RTC_GENERATE a);
        void DelNode(ARG_RTC_GENERATE a);
        void Exchange(ARG_RTC_GENERATE a, ARG_RTC_GENERATE exchg);
        void ToBottom(ARG_RTC_GENERATE a);
        const std::shared_ptr<DataNode>& GetHead() const;
        void Clear();
    };

    class DataMana{
    public:
        static DataList DataListGlobal;
    };
};

#endif // _DATA_CONSTRUCT_HPP_