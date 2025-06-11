#include "DeviceCheckResult.hpp"

namespace DCR{

    ChipCheckResult::ChipCheckResult()
        :CheckResult(true), IfOnline(false), CheckSatisfiedCount(0), CheckPacksOfMif(0), RangeSINAD(), ChipTestSt(WAITING_FOR_TESTING)
    {}

    ChipCheckResult::~ChipCheckResult()
    {}

    void ChipCheckResult::Init()
    {
        CheckResult = true;
        IfOnline = false;
        CheckSatisfiedCount = 0;
        CheckPacksOfMif = 0;
        RangeSINAD.SetRange(0, 0);
        ChipTestSt = WAITING_FOR_TESTING;
    }

    void ChipCheckResult::Reset()
    {
        CheckResult = true;
        CheckPacksOfMif = 0;
        ChipTestSt = WAITING_FOR_TESTING;
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

    void ChipCheckResult::SetCheckPacksOfMif(int CheckPacks)
    {
        CheckPacksOfMif = CheckPacks;
    }

    void ChipCheckResult::SetRangeSINAD(TCOND::Range rangeSINAD)
    {
        RangeSINAD = rangeSINAD;
    }

    void ChipCheckResult::SetChipTestStat(ChipTestStat st)
    {
        ChipTestSt = st;
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
        return CheckPacksOfMif;
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

    void ChipCheckResult::CheckPacksOfMifInc(bool res)
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
    }

    void ChipCheckResult::UpdateRangeSINAD(unsigned short sinad)
    {
        short res = TCOND::TestCondition::SinadTransfer(sinad);
        if(res < SINAD_VALID_VALUE_MIN)
        {
            return;
        }
        if(RangeSINAD.GetLeft() == RangeSINAD.GetRight() && RangeSINAD.GetLeft() == 0)
        {
            RangeSINAD.SetRange(res, res);
        }
        else if(res < RangeSINAD.GetLeft())
        {
            RangeSINAD.SetRange(res, RangeSINAD.GetRight());
        }
        else if(res > RangeSINAD.GetRight())
        {
            RangeSINAD.SetRange(RangeSINAD.GetLeft(), res);
        }        
    }

    //设备检测结果类

    DeviceCheckResult::DeviceCheckResult()
        :Condition(nullptr), CheckResult(nullptr), 
        BoardNum(0), ChipNum(0), CheckCount(0), 
        CheckCompletedCount(0), ChipOnLineNum(0), 
        ChipSatisfiedNum(0), ChipUnSatisfiedNum(0),
        TemperatureInner(0), TemperatureEnviroment(0),
        UpPackCount(0)
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
        ChipOnLineNum = 0;
        ChipSatisfiedNum = BoardNum * ChipNum;
        // ChipSatisfiedNum = 0;
        ChipUnSatisfiedNum = 0;
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
        ChipSatisfiedNum = BoardNum * ChipNum;
        // ChipSatisfiedNum = 0;
        ChipUnSatisfiedNum = 0;
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

    void DeviceCheckResult::SetTemperatureInner(unsigned short Temp)
    {
        TemperatureInner = TemperatureTransferInner(Temp);
    }

    void DeviceCheckResult::SetTemperatureEnv(unsigned short Temp)
    {
        TemperatureEnviroment = TemperatureTransferEnv(Temp);
    }

    void DeviceCheckResult::SetUpPackCount(unsigned short count)
    {
        UpPackCount = count;
    }

    int DeviceCheckResult::GetCheckCount() const
    {
        return CheckCount;
    }

    int DeviceCheckResult::GetCheckCompletedCount() const
    {
        return CheckCompletedCount;
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

    double DeviceCheckResult::TemperatureTransferInner(unsigned short data)
    {
        int t = data;
        t = (t * 503.975) / 65536 - 273.15;
        return t;
    }

    double DeviceCheckResult::TemperatureTransferEnv(unsigned short data)
    {
        return data / 16;
    }

    int DeviceCheckResult::GetUpPackCount() const
    {
        return UpPackCount;
    }

    std::unique_ptr<DeviceCheckResult> DeviceCheckResultGlobal(new DeviceCheckResult);
};