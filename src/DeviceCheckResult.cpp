//设置各类变量和接口用来表示测试结果
//

#include "DeviceCheckResult.hpp"
#include <QDebug>

namespace DCR{

    ChipCheckResult::ChipCheckResult()  
        : CheckResult(true), IfOnline(false), CheckSatisfiedCount(0), CheckPacksOfMifCodec(0), CheckPacksOfMifAdpow(0), TotalValidCodec(0), TotalValidAdpow(0), PassedGroupCount(0), RangeSINAD(), ChipTestSt(WAITING_FOR_TESTING),  
        ChipTestStCodec(WAITING_FOR_TESTING), ChipTestStAdpow(WAITING_FOR_TESTING) // 初始化 ChipTestStCodec 和 ChipTestStAdpow  
    {}

    ChipCheckResult::~ChipCheckResult() 
    {}

   /* void ChipCheckResult::Init()
    {
        CheckResult = true;
        IfOnline = false;
        CheckSatisfiedCount = 0;
        RangeSINAD.SetRange(0, 0);
        ChipTestSt = WAITING_FOR_TESTING;
        ChipTestStCodec = WAITING_FOR_TESTING;
        ChipTestStAdpow = WAITING_FOR_TESTING;
    }

    void ChipCheckResult::Reset()
    {
        CheckResult = true;
        CheckPacksOfMifCodec = 0;
        CheckPacksOfMifAdpow = 0;
        ChipTestSt = WAITING_FOR_TESTING;
        ChipTestStCodec = WAITING_FOR_TESTING;
        ChipTestStAdpow = WAITING_FOR_TESTING;
    }*/

    void ChipCheckResult::Init()//初始化
    {
        CheckResult = true;
        IfOnline = false;
        CheckSatisfiedCount = 0;
        CheckPacksOfMifCodec = 0;
        CheckPacksOfMifAdpow = 0;
        TotalValidCodec = 0;      // 新增
        TotalValidAdpow = 0;      // 新增
        PassedGroupCount = 0;     // 新增：初始化芯片级别的通过组数
        RangeSINAD.SetRange(0, 0);
        ChipTestSt = WAITING_FOR_TESTING;
        ChipTestStCodec = WAITING_FOR_TESTING;
        ChipTestStAdpow = WAITING_FOR_TESTING;
    }

    void ChipCheckResult::Reset()//每次下发完成后重置
    {
        //CheckResult = true;
        CheckPacksOfMifCodec = 0;
        CheckPacksOfMifAdpow = 0;
        TotalValidCodec = 0;      // 新增
        TotalValidAdpow = 0;      // 新增
        //PassedGroupCount = 0;     // 新增：重置芯片级别的通过组数
        ChipTestSt = WAITING_FOR_TESTING;
        ChipTestStCodec = WAITING_FOR_TESTING;
        ChipTestStAdpow = WAITING_FOR_TESTING;
    }

    void ChipCheckResult::SetCheckResult(bool checkResult)
    {
        CheckResult = checkResult;
    }

    void ChipCheckResult::SetIfOnline(bool ifOnline)
    {
        IfOnline = ifOnline;
    }

    void ChipCheckResult::SetCheckSatisfiedCount(int checkSatisfiedCount)
    {
        CheckSatisfiedCount = checkSatisfiedCount;
    }

    void ChipCheckResult::SetTotalValid(int totalCodec, int totalAdpow)
    {
        TotalValidCodec = totalCodec;
        TotalValidAdpow = totalAdpow;
    }

    void ChipCheckResult::SetPassedGroupCount(int count)
    {
        PassedGroupCount = count;
    }

    void ChipCheckResult::SetCheckPacksOfMif(int CheckPacksCodec, int CheckPacksAdpow)
    {
        
        CheckPacksOfMifCodec = CheckPacksCodec;
        CheckPacksOfMifAdpow = CheckPacksAdpow;

    }

    void ChipCheckResult::SetRangeSINAD(TCOND::Range rangeSINAD)
    {
        RangeSINAD = rangeSINAD;
    }

    void ChipCheckResult::SetChipTestStat(ChipTestStat st)
    {
        ChipTestSt = st;
        //新增，同步状态
        ChipTestStCodec = st;
        ChipTestStAdpow = st;
    }

    bool ChipCheckResult::GetCheckResult() const
    {
        return CheckResult;
    }

    bool ChipCheckResult::GetIfOnline() const
    {
        return IfOnline;
    }

    int ChipCheckResult::GetCheckSatisfiedCount() const
    {
        return CheckSatisfiedCount;
    }

    int ChipCheckResult::GetCheckPacksOfMif() const
    {
        //qDebug() << "CheckPacksOfMifCodec:" << CheckPacksOfMifCodec
        //    << "CheckPacksOfMifAdpow:" << CheckPacksOfMifAdpow;
        return std::min(CheckPacksOfMifCodec, CheckPacksOfMifAdpow);
    }

	int ChipCheckResult::GetTotalValidCodec() const//获取累计有效的Codec包数
    { 
        return TotalValidCodec; 
    
    }
	int ChipCheckResult::GetTotalValidAdpow() const //获取累计有效的Adpow包数
    {
        return TotalValidAdpow; 
    }

	int ChipCheckResult::GetPassedGroupCount() const //获取芯片级别的通过组数
    { 
        return PassedGroupCount;
    }

	int ChipCheckResult::GetCheckPacksOfMifCodec() const //获取连续有效的Codec包数
    { 
        return CheckPacksOfMifCodec;
    }

	int ChipCheckResult::GetCheckPacksOfMifAdpow() const //获取连续有效的Adpow包数
    { 
        return CheckPacksOfMifAdpow;
    }

    TCOND::Range ChipCheckResult::GetRangeSINAD() const
    {
        return RangeSINAD;
    }

    void ChipCheckResult::UpdateCheckResult(bool res)
    {
        CheckResult &= res;
    }

    void ChipCheckResult::CheckSatisfiedCountInc()
    {
        CheckSatisfiedCount++;
    }

    void ChipCheckResult::PassedGroupCountInc()
    {
        PassedGroupCount++;
    }

   /* void ChipCheckResult::CheckPacksOfMifInc(bool res)
    {
        if(IfOnline)
        {
            switch (ChipTestSt)
            {
            case WAITING_FOR_TESTING:
                if(res)
                {
                    SetChipTestStat(BE_TESTING);
                    CheckPacksOfMif++;
                }
                break;
            case BE_TESTING:
                if(res)
                {
                    CheckPacksOfMif++;
                }
                else
                {
                    SetChipTestStat(END_OF_TESTING);
                }
                break;
            case END_OF_TESTING:     
                break;
            default:
                break;
            }
        }
    }*/

    void ChipCheckResult::TotalValidCodecInc(bool valid)
    {
        if (valid) 
            ++TotalValidCodec;
    }
    void ChipCheckResult::TotalValidAdpowInc(bool valid)
    {
        if (valid) 
            ++TotalValidAdpow;
    }

    void ChipCheckResult::CheckPacksOfMifAdpowInc(bool res)
    {
        /*qDebug() << "AdpowInc: IfOnline=" << IfOnline
            << "State=" << ChipTestStAdpow
            << "Res=" << res
            << "Count=" << CheckPacksOfMifAdpow;*/
        if (IfOnline) {
            switch (ChipTestStAdpow) {
            case WAITING_FOR_TESTING:
                if (res) {
                    ChipTestStAdpow = BE_TESTING;
                    CheckPacksOfMifAdpow++;
                }
                break;
            case BE_TESTING:
                if (res) {
                    CheckPacksOfMifAdpow++;
                }
                else {
                    ChipTestStAdpow = END_OF_TESTING;
                }
                break;
            default:
                break;
            }
        }
    }

    void ChipCheckResult::CheckPacksOfMifCodecInc(bool res)
    {
        if (IfOnline) {
            switch (ChipTestStCodec) {
            case WAITING_FOR_TESTING:
                if (res) {
                    ChipTestStCodec = BE_TESTING;
                    CheckPacksOfMifCodec++;
                }
                break;
            case BE_TESTING:
                if (res) {
                    CheckPacksOfMifCodec++;
                }
                else {
                    ChipTestStCodec = END_OF_TESTING;
                }
                break;
            default:
                break;
            }
        }
    }

    //void ChipCheckResult::CheckPacksOfMifAdpowInc(bool res)
    //{
    //    if (IfOnline) {
    //        switch (ChipTestStAdpow) {
    //        case WAITING_FOR_TESTING:
    //            if (res) {
    //                ChipTestStAdpow = BE_TESTING;
    //                CheckPacksOfMifAdpow++;
    //            }
    //            break;
    //        case BE_TESTING:
    //            if (res) {
    //                CheckPacksOfMifAdpow++;
    //            }
    //            else {
    //                ChipTestStAdpow = END_OF_TESTING;
    //            }
    //            break;
    //        default:
    //            break;
    //        }
    //    }
    //}


    //动态统计一次测试过程中 SINAD 的最大最小值区间。
    void ChipCheckResult::UpdateRangeSINAD(unsigned short sinad)
    {
        short res = TCOND::TestCondition::SinadTransfer(sinad); // 将原始sinad值转换为实际dB值（如有缩放或偏移）
        if (res < SINAD_VALID_VALUE_MIN) // 如果转换后的值小于有效阈值（无效或异常数据），则不处理
        {
            return; // 直接返回
        }
        if (RangeSINAD.GetLeft() == RangeSINAD.GetRight() && RangeSINAD.GetLeft() == 0) // 如果区间未初始化（首次赋值）
        {
            RangeSINAD.SetRange(res, res); // 用当前值初始化区间
        }
        else if (res < RangeSINAD.GetLeft()) // 如果当前值比区间左端更小
        {
            RangeSINAD.SetRange(res, RangeSINAD.GetRight()); // 更新左端为当前值
        }
        else if (res > RangeSINAD.GetRight()) // 如果当前值比区间右端更大
        {
            RangeSINAD.SetRange(RangeSINAD.GetLeft(), res); // 更新右端为当前值
        }
        // 如果当前值在区间内，不做任何操作
    }

    //设备检测结果类

    DeviceCheckResult::DeviceCheckResult()
        :Condition(nullptr), CheckResult(nullptr),
        BoardNum(0), ChipNum(0), CheckCount(0),
        CheckCompletedCount(0), CheckedGroupCount(0),
        ChipOnLineNum(0), ChipSatisfiedNum(0), 
        ChipUnSatisfiedNum(0), PassedGroupCount(0), TotalGroupCount(0),
        TemperatureInner(0), TemperatureEnviroment(0), UpPackCount(0)
    {}

    DeviceCheckResult::~DeviceCheckResult()
    {}

    void DeviceCheckResult::Init(int boardNum, int chipNum)
    {
        BoardNum = boardNum;
        ChipNum = chipNum;
        Condition = std::make_unique<TCOND::TestCondition[]>(2);
        CheckResult = std::make_unique<std::unique_ptr<ChipCheckResult[]>[]>(BoardNum);
        for(int i = 0; i < BoardNum; i++)
        {
            CheckResult[i] = std::make_unique<ChipCheckResult[]>(ChipNum);
        }
    }

    void DeviceCheckResult::Init()
    {
        for(int i = 0; i < BoardNum; i++)
        {
            for(int j = 0; j < ChipNum; j++)
            {
                CheckResult[i][j].Init();
            }
        }
        CheckCompletedCount = 0;
        CheckedGroupCount = 0;
        ChipOnLineNum = 0;
        ChipSatisfiedNum = BoardNum * ChipNum;
        //ChipSatisfiedNum = 0;
        ChipUnSatisfiedNum = 0;
        //PassedGroupCount = 0;//新增：初始化通过测试的组数
        TotalGroupCount = 0;//新增：初始化总测试组数
    }

    void DeviceCheckResult::Reset()
    {
        for(int i = 0; i < BoardNum; i++)
        {
            for(int j = 0; j < ChipNum; j++)
            {
                CheckResult[i][j].Reset();
            }
        }
        //ChipSatisfiedNum = BoardNum * ChipNum;
        //CheckedGroupCount = 0;
        //ChipSatisfiedNum = 0;
        //PassedGroupCount = 0;//新增：重置通过测试的组数
        //TotalGroupCount = 0;//新增：重置总测试组数
    }

    void DeviceCheckResult::Clear()
    {
        Condition.reset();
        for(int i = 0; i < BoardNum; i++)
        {
            CheckResult[i].reset();
        }
        CheckResult.reset();
    }

    std::unique_ptr<TCOND::TestCondition[]>& DeviceCheckResult::GetCondition()
    {
        return Condition;
    }

    ChipCheckResult& DeviceCheckResult::GetChipCheckResult(int boardIndex, int chipIndex)
    {
        return CheckResult[boardIndex][chipIndex];
    }

    void DeviceCheckResult::SetCheckCount(int checkCount)
    {
        CheckCount = checkCount;
    }

    void DeviceCheckResult::SetCheckCompletedCount(int checkCompletedCount)
    {
        CheckCompletedCount = checkCompletedCount;
    }

    void DeviceCheckResult::SetCheckedGroupCount(int count) 
    {
        CheckedGroupCount = count;
    }

    void DeviceCheckResult::SetChipOnLineNum(int chipOnLineNum)
    {
        ChipOnLineNum = chipOnLineNum;
    }

    void DeviceCheckResult::SetChipSatisfiedNum(int chipSatisfiedNum)
    {
        ChipSatisfiedNum = chipSatisfiedNum;
    }

    void DeviceCheckResult::SetChipUnSatisfiedNum(int chipUnSatisfiedNum)
    {
        ChipUnSatisfiedNum = chipUnSatisfiedNum;
    }

    void DeviceCheckResult::SetPassedGroupCount(int count)
    {
        PassedGroupCount = count;
    }

    void DeviceCheckResult::SetTotalGroupCount(int count)
    {
        TotalGroupCount = count;
    }

    void DeviceCheckResult::SetTemperatureInner(unsigned short Temp)
    {
        TemperatureInner = TemperatureTransferInner(Temp);
    }

    void DeviceCheckResult::SetTemperatureEnv( short Temp)
    {
        TemperatureEnviroment = TemperatureTransferEnv(Temp);
    }

    void DeviceCheckResult::SetUpPackCount(unsigned short count)
    {
        UpPackCount = count;
    }

    void DeviceCheckResult::SetDbgDnAicCount(unsigned short count)//新增：设置调试下发AIC包计数
    {
        DbgDnAicCount = count;
    }

    int DeviceCheckResult::GetCheckCount() const
    {
        return CheckCount;
    }

    int DeviceCheckResult::GetCheckCompletedCount() const
    {
        return CheckCompletedCount;
    }

    int DeviceCheckResult::GetCheckedGroupCount() const 
    {
        return CheckedGroupCount;
    }

    int DeviceCheckResult::GetChipOnLineNum() const
    {
        return ChipOnLineNum;
    }

    int DeviceCheckResult::GetChipSatisfiedNum() const
    {
        return ChipSatisfiedNum;
    }

    int DeviceCheckResult::GetChipUnSatisfiedNum() const
    {
        return ChipUnSatisfiedNum;
    }

    int DeviceCheckResult::GetPassedGroupCount() const
    {
        return PassedGroupCount;
    }

    int DeviceCheckResult::GetTotalGroupCount() const
    {
        return TotalGroupCount;
    }

    double DeviceCheckResult::GetTemperatureInner() const
    {
        return TemperatureInner;
    }

    double DeviceCheckResult::GetTempeartureEnv() const
    {
        return TemperatureEnviroment;
    }

    void DeviceCheckResult::CheckCompletedCountInc()
    {
        CheckCompletedCount++;
    }

    void DeviceCheckResult::CheckedGroupCountInc() {
        CheckedGroupCount++;
    }

    void DeviceCheckResult::ChipOnlineNumInc()
    {
        ChipOnLineNum++;
    }

    void DeviceCheckResult::ChipOnlineNumDec()
    {
        if(ChipOnLineNum > 0)
            ChipOnLineNum--;
    }

    void DeviceCheckResult::ChipSatisfiedNumInc()
    {
        ChipSatisfiedNum++;
    }

    void DeviceCheckResult::ChipSatisfiedNumDec()
    {
        if(ChipSatisfiedNum > 0)
        {
            ChipSatisfiedNum--;
        }
    }

    void DeviceCheckResult::ChipUnSatisfiedNumInc()
    {
        ChipUnSatisfiedNum++;
    }

    void DeviceCheckResult::ChipUnSatisfiedNumDec()
    {
        if(ChipUnSatisfiedNum > 0)
            ChipUnSatisfiedNum--;
    }

    void DeviceCheckResult::PassedGroupCountInc()
    {
        PassedGroupCount++;
    }

    void DeviceCheckResult::TotalGroupCountInc()
    {
        TotalGroupCount++;
    }

    double DeviceCheckResult::TemperatureTransferInner(unsigned short data)
    {
        int t = data;
        t = (t * 503.975) / 65536 - 273.15;
        //t = (t * 503.975) / 65536;
        return t;
    }

    //修改：DS18B20的数据处理，低4位为小数部分，中间7位为整数部分，高5位为符号位，00000为正，11111为负，正温度十六位乘以0.0625，负温度取反加一后乘以0.0625。
    double DeviceCheckResult::TemperatureTransferEnv( short data)
    {
        return data / 16;

    }

    int DeviceCheckResult::GetUpPackCount() const
    {
        return UpPackCount;
    }

    int DeviceCheckResult::GetDbgDnAicCount() const//新增：获取调试下发AIC包计数
    {
        return DbgDnAicCount;
    }

    std::unique_ptr<DeviceCheckResult> DeviceCheckResultGlobal(new DeviceCheckResult);
};