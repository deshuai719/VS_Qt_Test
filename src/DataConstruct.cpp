#include "DataConstruct.hpp"
#include <cstdio>

namespace DCWZ{

    //数据段信息类

    SegInfo::SegInfo()
        : Start(0), Len(0)
    {}

    SegInfo::SegInfo(long long Start, long long Len)
        : Start(Start), Len(Len)
    {}

    SegInfo::SegInfo(const SegInfo& Seg)
        : Start(Seg.GetStart()), Len(Seg.GetLen())
    {}

    SegInfo::~SegInfo()
    {}

    void SegInfo::operator=(const SegInfo& Seg)
    {
        Start = Seg.GetStart();
        Len = Seg.GetLen();
    }

    void SegInfo::SetSeg(long long Start, long long Len)
    {
        this->Start = Start;
        this->Len = Len;
    }

    long long SegInfo::GetStart() const
    {
        return Start;
    }

    long long SegInfo::GetLen() const
    {
        return Len;
    }

    //数据包信息类

    PackInfo::PackInfo()
        : PackData(nullptr), SegAll(), SegHead(), SegFrame(), SegData()
    {}

    PackInfo::~PackInfo()
    {
        Clear();
    }

    void PackInfo::Init(long long Len)
    {
        Clear();
        PackData = std::make_unique<char[]>(Len);
    }

    void PackInfo::Clear()
    {
        if(PackData != nullptr)
        {
            PackData.reset();
            PackData = nullptr;
        }
    }

    void PackInfo::SetSegAll(SegInfo Seg)
    {
        SegAll = Seg;
    }

    void PackInfo::SetSegHead(SegInfo Seg)
    {
        SegHead = Seg;
    }

    void PackInfo::SetSegFrame(SegInfo Seg)
    {
        SegFrame = Seg;
    }

    void PackInfo::SetSegData(SegInfo Seg)
    {
        SegData = Seg;
    }

    SegInfo PackInfo::GetSegAll() const
    {
        return SegAll;
    }

    SegInfo PackInfo::GetSegHead() const
    {
        return SegHead;
    }

    SegInfo PackInfo::GetSegFrame() const
    {
        return SegFrame;
    }

    SegInfo PackInfo::GetSegData() const
    {
        return SegData;
    }

    char* PackInfo::GetPackData() const
    {
        return PackData.get();
    }

    //数据构造基类

    DataConstruct::DataConstruct(ArgRegCFG<unsigned char> a)
        : RegCFG(a)
    {}

    DataConstruct::~DataConstruct()
    {}

    const ArgRegCFG<unsigned char>& DataConstruct::GetRegCFG() const
    {
        return RegCFG;
    }

    void DataConstruct::UpdateCfgCMD()
    {
        CmdRegCFG[0x41] |= 0xFFu;
        CmdRegCFG[0x42] |= 0xFFu;
        CmdRegCFG[0x43] |= 0xFFu;
        CmdRegCFG[0x44] |= 0xFFu;
        CmdRegCFG[0x45] |= 0xFFu;
        CmdRegCFG[0x46] |= 0xFFu;
        CmdRegCFG[0x47] |= 0xFFu;
        CmdRegCFG[0x48] |= 0xFFu;

        unsigned int DL = RegCFG.GetDL();
        DL |= 0xFFFFFF00u;
        CmdRegCFG[0x41] &= DL;
        CmdRegCFG[0x42] &= DL;
        unsigned int DR = RegCFG.GetDR();
        DR |= 0xFFFFFF00u;
        CmdRegCFG[0x43] &= DR;
        CmdRegCFG[0x44] &= DR;
        unsigned int AL = RegCFG.GetAL();
        AL |= 0xFFFFFF00u;
        CmdRegCFG[0x45] &= AL;
        CmdRegCFG[0x46] &= AL;
        unsigned int AR = RegCFG.GetAR();
        AR |= 0xFFFFFF00u;
        CmdRegCFG[0x47] &= AR;
		CmdRegCFG[0x48] &= AR;

		unsigned int chksum_head_all = TOOLWZ::AccVerify((char*)(CmdRegCFG + 1), 320);
        CmdRegCFG[0x51] = chksum_head_all;
    }

    const char const* DataConstruct::GetRegCfgCMD()
    {
        return (const char const*)CmdRegCFG;
    }

    void DataConstruct::InitCmdRegCFG()
    {
        FRAME_HEAD_STAR(CmdRegCFG)->msgID = 0xA0;
        FRAME_HEAD_STAR(CmdRegCFG)->rev = 0xF;
        FRAME_HEAD_STAR(CmdRegCFG)->ctlFractstop = 0;
        FRAME_HEAD_STAR(CmdRegCFG)->ctlFractStrt = 0;
        FRAME_HEAD_STAR(CmdRegCFG)->ctlFractMode = 0;
        FRAME_HEAD_STAR(CmdRegCFG)->reserved_base = 0;
        FRAME_HEAD_STAR(CmdRegCFG)->siglen = 0x141;

        PTR_DN_ARG_A0(CmdRegCFG + 1)->data_bound_num8b = 4;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->read_flash2ram = 0;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->read_ram = 0;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->write_ram = 1;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->chksum_head = 0;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->data_depth = 0x4e;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->addr_start = 0x0;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->ram_id = 0x1e;
        PTR_DN_ARG_A0(CmdRegCFG + 1)->reserved3 = 0;
		PTR_DN_ARG_A0(CmdRegCFG + 1)->chksum_head = TOOLWZ::AccVerify((char*)(CmdRegCFG + 1), 8);//计算头部校验和（累加取法校验和）
    }

    unsigned int DataConstruct::CmdRegCFG[0x52] = {
        0x00000000,
        0x00000000,
        0x00000000,
        0x01A00100,
        0x01A016FF,
        0x01A018FF,
        0x01A00202,
        0x01A00302,
        0x01A00C08,
        0x01A00D08,
        0x01A00E01,
        0x01A04703,
        0x01A04803,
        0x01A011A0,
        0x01A0A203,
        0x01A0A3F5,
        0x01A0A400,
        0x01A0A500,
        0x01A0A601,
        0x01A0A708,
        0x01A0A800,
        0x01A0A90C,
        0x01A0AA00,
        0x01A0AB10,
        0x01A0AC01,
        0x01A0AD18,
        0x01A0AE02,
        0x01A0AF20,
        0x01A0B07F,
        0x01A0B128,
        0x01A0B27F,
        0x01A0B328,
        0x01A0B440,
        0x01A0B528,
        0x01A0B67F,
        0x01A0B72C,
        0x01A0B87F,
        0x01A0B92C,
        0x01A0BA7F,
        0x01A0BB2C,
        0x01A0BC7F,
        0x01A0BD30,
        0x01A0BE7F,
        0x01A0BF34,
        0x01A0C07F,
        0x01A0C13C,
        0x01A0C265,
        0x01A0C33C,
        0x01A0F002,
        0x01A0F101,
        0x01A0F38A,
        0x01A0120B,
        0x01A00000,
        0x01A00101,
        0x01A00100,
        0x01A00003,
        0x01A00101,
        0x01A00100,
        0x01A01505,
        0x01A00101,
        0x01A00100,
        0x01A0FF01,
        0x01A01205,
        0x01A01D00,
        0x01A01F00,
        0x01A02414,
        0x01A02514,
        0x01A02612,
        0x01A02712,
        0x01A03454,
        0x01A03554,
        0x01A03828,
        0x01A03928,
        0x01A0C800,
        0x01A0C900,
        0x01A00101,
        0x01A00100,
        0x01A10000,
        0x01A10C00,
        0x01A10D00,
        0x03A10E00,
       
    };
    
    /*
        mif文件数据构造类
    */

    DC_DN_MNIC_64K::DC_DN_MNIC_64K()
        : FP(), PackInfoArr(nullptr), PackNum(0), Arg(), SendCnt(0)
    {}

    DC_DN_MNIC_64K::DC_DN_MNIC_64K(FPWZ::ArgDM a)
        : FP(), PackInfoArr(nullptr), PackNum(0), Arg(a), SendCnt(0)
    {
        FP.Init(a.GetPath());
    }

    DC_DN_MNIC_64K::~DC_DN_MNIC_64K()
    {
        CLear();
    }

    void DC_DN_MNIC_64K::CLear()
    {
        FP.Clear();
        if(PackInfoArr != nullptr)
        {
            for(int i = 0; i < PackNum; i++)
            {
                PackInfoArr[i].Clear();
            }
            PackInfoArr.reset();
            PackInfoArr = nullptr;
        }
        PackNum = 0;
    }

    void DC_DN_MNIC_64K::Construct()
    {
        FP.Parse();
        if(FP.GetFileDataSize() % PACK_DATA_LEN)
        {
            PackNum = FP.GetFileDataSize() / PACK_DATA_LEN + 1;
            PackInfoArr = std::make_unique<PackInfo[]>(PackNum);
            for(int i = 0; i < PackNum - 1; i++)
            {
                PackInfoArr[i].Init(HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN);
                PackInfoArr[i].SetSegAll(SegInfo(0, HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN));
                PackInfoArr[i].SetSegHead(SegInfo(0, HEAD_DATA_LEN));
                PackInfoArr[i].SetSegFrame(SegInfo(0 + HEAD_DATA_LEN, DN_MNIC_64k_DATA_LEN));
                PackInfoArr[i].SetSegData(SegInfo(0 + HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN, PACK_DATA_LEN));
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->msgID = 0x38;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->ctlFractstop = 0;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->ctlFractStrt = 0;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->ctlFractMode = 0;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->siglen = DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN;
                DN_MNIC_64K_STAR(PackInfoArr[i].GetPackData() + PackInfoArr[i].GetSegFrame().GetStart())->channel = 0;
                DN_MNIC_64K_STAR(PackInfoArr[i].GetPackData() + PackInfoArr[i].GetSegFrame().GetStart())->reserved = 0;//每次下发通道0的数据包时加1
                for(int j = PackInfoArr[i].GetSegData().GetStart(); j < PackInfoArr[i].GetSegData().GetStart() + PackInfoArr[i].GetSegData().GetLen(); j++)
                {
                    PackInfoArr[i].GetPackData()[j] = FP.GetBuffer()[i * PACK_DATA_LEN + j - PackInfoArr[i].GetSegData().GetStart()];
                }
            }

            PackInfoArr[PackNum - 1].Init(HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + FP.GetFileDataSize() % PACK_DATA_LEN);
            PackInfoArr[PackNum - 1].SetSegAll(SegInfo(0, HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + FP.GetFileDataSize() % PACK_DATA_LEN));
            PackInfoArr[PackNum - 1].SetSegHead(SegInfo(0, HEAD_DATA_LEN));
            PackInfoArr[PackNum - 1].SetSegFrame(SegInfo(0 + HEAD_DATA_LEN, DN_MNIC_64k_DATA_LEN));
            PackInfoArr[PackNum - 1].SetSegData(SegInfo(0 + HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN, FP.GetFileDataSize() % PACK_DATA_LEN));
            FRAME_HEAD_STAR(PackInfoArr[PackNum - 1].GetPackData())->msgID = 0x38;
            FRAME_HEAD_STAR(PackInfoArr[PackNum - 1].GetPackData())->ctlFractstop = 0;
            FRAME_HEAD_STAR(PackInfoArr[PackNum - 1].GetPackData())->ctlFractStrt = 0;
            FRAME_HEAD_STAR(PackInfoArr[PackNum - 1].GetPackData())->ctlFractMode = 0;
            FRAME_HEAD_STAR(PackInfoArr[PackNum - 1].GetPackData())->siglen = DN_MNIC_64k_DATA_LEN + FP.GetFileDataSize() % PACK_DATA_LEN;
            DN_MNIC_64K_STAR(PackInfoArr[PackNum - 1].GetPackData() + PackInfoArr[PackNum - 1].GetSegFrame().GetStart())->channel = 0;
            DN_MNIC_64K_STAR(PackInfoArr[PackNum - 1].GetPackData() + PackInfoArr[PackNum - 1].GetSegFrame().GetStart())->reserved = 0;//每次下发通道0的数据包时加1 
            for(int j = PackInfoArr[PackNum - 1].GetSegData().GetStart(); j < PackInfoArr[PackNum - 1].GetSegData().GetStart() + PackInfoArr[PackNum - 1].GetSegData().GetLen(); j++)
            {
                PackInfoArr[PackNum - 1].GetPackData()[j] = FP.GetBuffer()[(PackNum - 1) * PACK_DATA_LEN + j - PackInfoArr[PackNum - 1].GetSegData().GetStart()];
            }
        }
        else
        {
            PackNum = FP.GetFileDataSize() / PACK_DATA_LEN;
            PackInfoArr = std::make_unique<PackInfo[]>(PackNum);
            for(int i = 0; i < PackNum; i++)
            {
                PackInfoArr[i].Init(HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN);
                PackInfoArr[i].SetSegAll(SegInfo(0, HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN));
                PackInfoArr[i].SetSegHead(SegInfo(0, HEAD_DATA_LEN));
                PackInfoArr[i].SetSegFrame(SegInfo(0 + HEAD_DATA_LEN, DN_MNIC_64k_DATA_LEN));
                PackInfoArr[i].SetSegData(SegInfo(0 + HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN, PACK_DATA_LEN));
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->msgID = 0x38;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->ctlFractstop = 0;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->ctlFractStrt = 0;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->ctlFractMode = 0;
                FRAME_HEAD_STAR(PackInfoArr[i].GetPackData())->siglen = DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN;
                DN_MNIC_64K_STAR(PackInfoArr[i].GetPackData() + PackInfoArr[i].GetSegFrame().GetStart())->channel = 0;
                DN_MNIC_64K_STAR(PackInfoArr[i].GetPackData() + PackInfoArr[i].GetSegFrame().GetStart())->reserved = 0;//每次下发通道0的数据包时加1
                for(int j = PackInfoArr[i].GetSegData().GetStart(); j < PackInfoArr[i].GetSegData().GetStart() + PackInfoArr[i].GetSegData().GetLen(); j++)
                {
                    PackInfoArr[i].GetPackData()[j] = FP.GetBuffer()[i * PACK_DATA_LEN + j - PackInfoArr[i].GetSegData().GetStart()];
                }
            }
        }
    }

    void DC_DN_MNIC_64K::Construct(int start)
    {}

    const FPWZ::FileParsingMif& DC_DN_MNIC_64K::GetFP() const
    {
        return FP;
    }

    const SDG::SamplingDataGenerate& DC_DN_MNIC_64K::GetSDG() const
    {
        return SDG::SamplingDataGenerate();
    }

    PackInfo& DC_DN_MNIC_64K::GetPackInfo(long long index)
    {
        DN_MNIC_64K_STAR(PackInfoArr[index % PackNum].GetPackData() + PackInfoArr[index % PackNum].GetSegFrame().GetStart())->reserved = ++SendCnt;
        return PackInfoArr[index % PackNum];
    }

    long long DC_DN_MNIC_64K::GetPackNum() const
    {
        return PackNum;
    }

    long long DC_DN_MNIC_64K::GetSendNum() const
    {
        return Arg.GetDuration() * 125;
    }

    int DC_DN_MNIC_64K::GetDuration() const
    {
        return Arg.GetDuration();
    }

    //参数数据构造类
    DCWZ::ARG_RTC_GENERATE::ARG_RTC_GENERATE()
        : ArgAudio(), RegCFG(), JudgeCondCodec(), JudgeCondAdpow() {
    }

    DCWZ::ARG_RTC_GENERATE::ARG_RTC_GENERATE(SDG::ARG a, ArgRegCFG<unsigned char> regCfg,
        const TCOND::TestCondition& codecCond,
        const TCOND::TestCondition& adpowCond)
        : ArgAudio(a), RegCFG(regCfg), JudgeCondCodec(codecCond), JudgeCondAdpow(adpowCond) {
    }

    DCWZ::ARG_RTC_GENERATE::ARG_RTC_GENERATE(const ARG_RTC_GENERATE& cp)
        : ArgAudio(cp.ArgAudio), RegCFG(cp.RegCFG),
        JudgeCondCodec(cp.JudgeCondCodec), JudgeCondAdpow(cp.JudgeCondAdpow) {
    }
    /*ARG_RTC_GENERATE::ARG_RTC_GENERATE()
        : ArgAudio(), RegCFG(), JudgeCondCodec(), JudgeCondAdpow()
    {}

    ARG_RTC_GENERATE::ARG_RTC_GENERATE(SDG::ARG a, ArgRegCFG<unsigned char> regCfg)
        : ArgAudio(a), RegCFG(regCfg)
    {}

    ARG_RTC_GENERATE::ARG_RTC_GENERATE(const ARG_RTC_GENERATE& cp)
        : ArgAudio(cp.GetAudioARG()), RegCFG(cp.GetRegCfgARG())
    {}*/

    ARG_RTC_GENERATE::~ARG_RTC_GENERATE()
    {}

    void DCWZ::ARG_RTC_GENERATE::operator=(const ARG_RTC_GENERATE& as) 
    {
        ArgAudio = as.ArgAudio;
        RegCFG = as.RegCFG;
        JudgeCondCodec = as.JudgeCondCodec;
        JudgeCondAdpow = as.JudgeCondAdpow;
    }
   /* void ARG_RTC_GENERATE::operator=(const ARG_RTC_GENERATE& as)
    {
    
        ArgAudio = as.GetAudioARG();
        RegCFG = as.GetRegCfgARG();
    }*/

    bool ARG_RTC_GENERATE::operator==(const ARG_RTC_GENERATE& as) const
    {
        // if(ArgAudio == as.GetAudioARG() && RegCFG == as.GetRegCfgARG())
        // {
        //     WRITE_DLG_MENU_AUDIO_ARG_DBG("DCWZ::ARG_RTC_GENERATE::operator==(), AudioArg: dB = %f, Freq = %lld\n",
        //         ArgAudio.GetDB(), ArgAudio.GetFreq());
        //     WRITE_DLG_MENU_AUDIO_ARG_DBG("DCWZ::ARG_RTC_GENERATE::operator==(), as.audioArg: dB = %f, Freq = %lld\n",
        //         as.GetAudioARG().GetDB(), as.GetAudioARG().GetFreq());
        //     return true;
        // }
        // else
        // {
        //     return false;
        // }
        return ArgAudio == as.GetAudioARG() && RegCFG == as.GetRegCfgARG();
    }

    void ARG_RTC_GENERATE::SetAudioARG(SDG::ARG a)
    {
        ArgAudio = a;
    }

    SDG::ARG ARG_RTC_GENERATE::GetAudioARG() const
    {
        return ArgAudio;
    }

    void ARG_RTC_GENERATE::SetRegcfgARG(ArgRegCFG<unsigned char> regCfg)
    {
        RegCFG = regCfg;
    }

    ArgRegCFG<unsigned char> ARG_RTC_GENERATE::GetRegCfgARG() const
    {
        return RegCFG;
    }

    //

    DC_DN_MNIC_64K_RCT_GENERATE::DC_DN_MNIC_64K_RCT_GENERATE():SampDataGenerator(), PacksBuffer(nullptr){}

    DC_DN_MNIC_64K_RCT_GENERATE::DC_DN_MNIC_64K_RCT_GENERATE(ARG_RTC_GENERATE a)
        :DataConstruct(a.GetRegCfgARG()), SampDataGenerator(a.GetAudioARG()), PacksBuffer(new PackInfo),
        Amp((std::round(std::pow(10, SampDataGenerator.GetArg().GetDB() / 20) * 10000) / 10000) * 32767),
        StepLen(SampDataGenerator.GetArg().GetFreq() / 125),
        SendPacksNum(SampDataGenerator.GetArg().GetDur() * 125),
        PackCnt(-1)
    {}

    DC_DN_MNIC_64K_RCT_GENERATE::~DC_DN_MNIC_64K_RCT_GENERATE()
    {
        CLear();
    }

    void DC_DN_MNIC_64K_RCT_GENERATE::CLear()
    {
        if(PacksBuffer != nullptr)
        {
            PacksBuffer.reset();
            PacksBuffer = nullptr;
        }
    }

    void DC_DN_MNIC_64K_RCT_GENERATE::Construct()
    {
        PacksBuffer->Init(HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN);
        PacksBuffer->SetSegAll(SegInfo(0, HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN));
        PacksBuffer->SetSegHead(SegInfo(0, HEAD_DATA_LEN));
        PacksBuffer->SetSegFrame(SegInfo(0 + HEAD_DATA_LEN, DN_MNIC_64k_DATA_LEN));
        PacksBuffer->SetSegData(SegInfo(0 + HEAD_DATA_LEN + DN_MNIC_64k_DATA_LEN, PACK_DATA_LEN));
        FRAME_HEAD_STAR(PacksBuffer->GetPackData())->msgID = 0x38;
        FRAME_HEAD_STAR(PacksBuffer->GetPackData())->ctlFractstop = 0;
        FRAME_HEAD_STAR(PacksBuffer->GetPackData())->ctlFractStrt = 0;
        FRAME_HEAD_STAR(PacksBuffer->GetPackData())->ctlFractMode = 0;
        FRAME_HEAD_STAR(PacksBuffer->GetPackData())->siglen = DN_MNIC_64k_DATA_LEN + PACK_DATA_LEN;
        DN_MNIC_64K_STAR(PacksBuffer->GetPackData() + PacksBuffer->GetSegFrame().GetStart())->channel = 0;
        DN_MNIC_64K_STAR(PacksBuffer->GetPackData() + PacksBuffer->GetSegFrame().GetStart())->reserved = 0;//每次下发通道0的数据包时加1
    }

    void DC_DN_MNIC_64K_RCT_GENERATE::Construct(int start)
    {
        // WRITE_TASK_DATA_SEND_DBG("DCWZ::DC_DN_64K_RTC_GENERATE::Construct(int) Enter\n");
        DN_MNIC_64K_STAR(PacksBuffer->GetPackData() + PacksBuffer->GetSegFrame().GetStart())->reserved++;
        // WRITE_TASK_DATA_SEND_DBG("reserved++\n");
        // WRITE_TASK_DATA_SEND_DBG("Amp = %f\n", Amp);
        for(int i = 0; i < DEFAULT_SAMPLING_NUM; i++)
        {
            // WRITE_TASK_DATA_SEND_DBG("for(int i = 0; i < DEFAULT_SAMPLING_NUM; i++) i = %d\n", i);
            int V = std::round(SampDataGenerator.GetSampVal(i * StepLen + start) * Amp);
            V = V & 0xFFFFi32;
            PacksBuffer->GetPackData()[i * 2 + PacksBuffer->GetSegData().GetStart()] = V & 0xFFi16;
            PacksBuffer->GetPackData()[i * 2 + 1 + PacksBuffer->GetSegData().GetStart()] = (V >> 8) & 0xFFi16;
        }
        // WRITE_TASK_DATA_SEND_DBG("DCWZ::DC_DN_64K_RTC_GENERATE::Construct(int) Leave\n");
    }

    const FPWZ::FileParsingMif& DC_DN_MNIC_64K_RCT_GENERATE::GetFP() const
    {
        return FPWZ::FileParsingMif();
    }

    const SDG::SamplingDataGenerate& DC_DN_MNIC_64K_RCT_GENERATE::GetSDG() const
    {
        return SampDataGenerator;
    }

    PackInfo& DC_DN_MNIC_64K_RCT_GENERATE::GetPackInfo(long long index)
    {
        // WRITE_TASK_DATA_SEND_DBG("DCWZ::DC_DN_MNIC_64K_RTC_GENERATE::GetPackInfo(long long index) Enter\n");
        if(index != PackCnt)
        {
            // WRITE_TASK_DATA_SEND_DBG("index != PackCnt\n");
            Construct((index * StepLen * DEFAULT_SAMPLING_NUM) % DEFAULT_SAMPLING_NUM);
            PackCnt = index;
        }
        // WRITE_TASK_DATA_SEND_DBG("DCWZ::DC_DN_MNIC_64K_RTC_GENERATE::GetPackInfo(long long index) Leave\n");
        return *PacksBuffer;
    }

    long long DC_DN_MNIC_64K_RCT_GENERATE::GetPackNum() const
    {
        return 1;
    }

    long long DC_DN_MNIC_64K_RCT_GENERATE::GetSendNum() const
    {
        return SendPacksNum;
    }

    int DC_DN_MNIC_64K_RCT_GENERATE::GetDuration() const
    {
        return SampDataGenerator.GetArg().GetDur();
    }

    //数据链表节点类

    DataNode::DataNode()
        : DC(nullptr), Next(nullptr)
    {}

    DataNode::~DataNode()
    {
        Clear();
    }

    void DataNode::Clear()
    {
        if(DC != nullptr)//清空数据
        {
            DC->CLear();
            DC.reset();
            DC = nullptr;
        }
        if(Next != nullptr)//将指针置空
        {
            Next.reset();
            Next = nullptr;
        }
    }

    void DataNode::ClearDATA()
    {
        if(DC != nullptr)
        {
            DC->CLear();
            DC.reset();
            DC = nullptr;
        }
    }

    void DataNode::SetData(std::unique_ptr<DataConstruct>& D)
    {
        this->DC = std::move(D);
    }

    void DataNode::SetNext(std::shared_ptr<DataNode>& N)
    {
        this->Next = N;
    }

    void DataNode::SetNext(const std::shared_ptr<DataNode>& N)
    {
        this->Next = N;
    }

    const std::unique_ptr<DataConstruct>& DataNode::GetData() const
    {
        return DC;
    }

    const std::shared_ptr<DataNode>& DataNode::GetNext() const
    {
        return Next;
    }

    //数据链表类

    DataList::DataList()
        : Head(nullptr), Tail(nullptr)
    {}

    DataList::~DataList()
    {
        Clear();
    }

    void DataList::AddNode(std::unique_ptr<DataConstruct>& DC)
    {
        std::shared_ptr<DataNode> Node = std::make_shared<DataNode>();
        Node->SetData(DC);
        if(Head == nullptr)
        {
            Head = Node;
            Tail = Node;
        }
        else
        {
            Tail->SetNext(Node);
            Tail = Node;
        }
    }

    void DataList::AddNode(FPWZ::ArgDM a)
    {
        std::unique_ptr<DataConstruct> DC = std::make_unique<DC_DN_MNIC_64K>(a);
        DC->Construct();
        AddNode(DC);
    }

    void DataList::DelNode(FPWZ::ArgDM a)
    {
        std::shared_ptr<DataNode> Node = Head;
        std::shared_ptr<DataNode> PreNode = nullptr;
        while(Node != nullptr)
        {
            if(Node->GetData()->GetFP().GetFilePath() == a.GetPath() && Node->GetData()->GetDuration() == a.GetDuration())
            {
                if(Node == Head)
                {
                    Head = Node->GetNext();
                    Node->Clear();
                    Node = nullptr;
                }
                else if(Node == Tail)
                {
                    Tail = PreNode;
                    Tail->SetNext(nullptr);
                    Node->Clear();
                    Node = nullptr;
                }
                else
                {
                    PreNode->SetNext(Node->GetNext());
                    Node->Clear();
                    Node = nullptr;
                }
            }
            else
            {
                PreNode = Node;
                Node = Node->GetNext();
            }
        }
    }

    void DataList::AddNode(ARG_RTC_GENERATE a)
    {
        std::unique_ptr<DataConstruct> DC = std::make_unique<DC_DN_MNIC_64K_RCT_GENERATE>(a);
        DC->Construct();
        AddNode(DC);
    }

    void DataList::DelNode(ARG_RTC_GENERATE a)
    {
        std::shared_ptr<DataNode> Node = Head;
        std::shared_ptr<DataNode> PreNode = nullptr;
        for(; Node != nullptr; PreNode = Node, Node = Node->GetNext())
        {
            if(Node->GetData()->GetSDG().GetArg() == a.GetAudioARG() && Node->GetData()->GetRegCFG() == a.GetRegCfgARG())
            {
                if(Node == Head)
                {
                    Head = Node->GetNext();
                    Node->Clear();
                    break;
                }
                else if(Node == Tail)
                {
                    Tail = PreNode;
                    Tail->SetNext(nullptr);
                    Node->Clear();
                    break;
                }
                else
                {
                    PreNode->SetNext(Node->GetNext());
                    Node->Clear();
                    break;
                }
            }
        }
    }

    void DataList::Exchange(ARG_RTC_GENERATE a, ARG_RTC_GENERATE exchg)
    {
        if(a == exchg)
        {
            return;
        }
        std::shared_ptr<DataNode> Node = Head;
        std::shared_ptr<DataNode> PreNode = nullptr;
        for(; Node != nullptr; PreNode = Node, Node = Node->GetNext())
        {
            if(Node->GetData()->GetSDG().GetArg() == a.GetAudioARG() && Node->GetData()->GetRegCFG() == a.GetRegCfgARG())
            {
                std::shared_ptr<DataNode> NodeExchg = Head;
                std::shared_ptr<DataNode> PreNodeExchg = nullptr;
                for(; NodeExchg != nullptr; PreNodeExchg = NodeExchg, NodeExchg = NodeExchg->GetNext())
                {
                    if(NodeExchg->GetData()->GetSDG().GetArg() == exchg.GetAudioARG() && NodeExchg->GetData()->GetRegCFG() == exchg.GetRegCfgARG())
                    {
                        PreNode->SetNext(NodeExchg);
                        PreNodeExchg->SetNext(Node);
                        std::shared_ptr<DataNode> NodeNext = Node->GetNext();
                        Node->SetNext(NodeExchg->GetNext());
                        NodeExchg->SetNext(NodeNext);
                        break;
                    }
                }
                break;
            }
        }
    }

    void DataList::ToBottom(ARG_RTC_GENERATE a)
    {
        std::shared_ptr<DataNode> Node = Head;
        std::shared_ptr<DataNode> PreNode = nullptr;
        for(; Node != nullptr; PreNode = Node, Node = Node->GetNext())
        {
            if(Node->GetData()->GetSDG().GetArg() == a.GetAudioARG() && Node->GetData()->GetRegCFG() == a.GetRegCfgARG())
            {
                if(Node != Tail)
                {
                    if(Node == Head)
                    {
                        Tail->SetNext(Head);
                        Head = Head->GetNext();
                        Tail->GetNext()->SetNext(nullptr);
                        Tail = Tail->GetNext();
                    }
                    else
                    {
                        PreNode->SetNext(Node->GetNext());
                        Tail->SetNext(Node);
                        Node->SetNext(nullptr);
                        Tail = Node;
                    }
                }
            }
        }
    }

    const std::shared_ptr<DataNode>& DataList::GetHead() const
    {
        return Head;
    }

    void DataList::Clear()
    {
        std::shared_ptr<DataNode> Node = Head;
        std::shared_ptr<DataNode> NextNode = nullptr;
        while(Node != nullptr)
        {
            NextNode = Node->GetNext();
            Node->Clear();
            Node = NextNode;
        }
        Head = nullptr;
        Tail = nullptr;
    }

    DataList DataMana::DataListGlobal;
};