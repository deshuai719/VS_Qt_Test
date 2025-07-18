﻿#ifndef _DEVICECHECKRESULT_HPP_
#define _DEVICECHECKRESULT_HPP_

#include "TestCondition.hpp"

namespace DCR{

    #define GET_BIT_U32(v, bit) (((v) >> bit) & ((unsigned int)0x1))
    #define SINAD_VALID_VALUE_MIN 50//sinad最小有效值

    enum ChipTestStat{//在一次测试中芯片的检测状态
        WAITING_FOR_TESTING,//等待检测
        BE_TESTING,//正在检测
        END_OF_TESTING,//检测结束
    };

    class ChipCheckResult{
    private:
        bool CheckResult;//当前测试结果
        bool IfOnline;//芯片是否在线
        int CheckSatisfiedCount;//当前测试条件累计合格次数
        //int CheckPacksOfMif;  //当前测试条件有效合格次数
        int CheckPacksOfMifCodec;  // 新增：前两通道为一组
		int CheckPacksOfMifAdpow;  // 新增：第三通道独立
        int TotalValidCodec = 0;   // Codec总有效包数
        int TotalValidAdpow = 0;   // Adpow总有效包数
        TCOND::Range RangeSINAD;//记录一次测试中codec的SINAD的最大最小值
        ChipTestStat ChipTestSt;
        // 在 ChipCheckResult 中添加
        ChipTestStat ChipTestStCodec;
        ChipTestStat ChipTestStAdpow;

    public:
        ChipCheckResult();
        ~ChipCheckResult();

        void Init();
        void Reset();
        void SetCheckResult(bool checkResult);
        void SetIfOnline(bool ifOnline);
        void SetCheckSatisfiedCount(int checkCompleteCount);
        void SetCheckPacksOfMif(int CheckPacksCodec, int CheckPacksAdpow);
        void SetTotalValid(int totalCodec, int totalAdpow);
        void SetRangeSINAD(TCOND::Range rangeSINAD);
        void SetChipTestStat(ChipTestStat st);
        void TotalValidCodecInc(bool valid);
        void TotalValidAdpowInc(bool valid);
        bool GetCheckResult() const;
        bool GetIfOnline() const;
        int GetCheckSatisfiedCount() const;
        int GetCheckPacksOfMif() const;
        int GetCheckPacksOfMifCodec() const;  // 新增：前两通道为一组
        int GetCheckPacksOfMifAdpow() const;  // 新增：第三通道为一组
        int GetTotalValidCodec() const;
        int GetTotalValidAdpow() const;
        TCOND::Range GetRangeSINAD() const;
        void UpdateCheckResult(bool res);
        void CheckSatisfiedCountInc();
        //void CheckPacksOfMifInc(bool res);
        void CheckPacksOfMifCodecInc(bool res);   // 新增:前两通道组
        void CheckPacksOfMifAdpowInc(bool res);   // 新增：第三通道
        void UpdateRangeSINAD(unsigned short sinad);
    };

    class DeviceCheckResult{
    private:
        std::unique_ptr<TCOND::TestCondition[]> Condition;//数组长度为2，分别表示Codec和ADPOW的判断条件
        std::unique_ptr<std::unique_ptr<ChipCheckResult[]>[]> CheckResult;
        int BoardNum;//板卡数
        int ChipNum;//每张板卡芯片数
        int CheckCount;//总测试次数
        int CheckCompletedCount;//测试完成(包含未通过的)次数
        int CheckedGroupCount; // 新增：已测试组数
        int ChipOnLineNum;//在线芯片数
        int ChipSatisfiedNum;//满足测试条件的芯片数
        int ChipUnSatisfiedNum;//不满足测试条件的芯片数
        double TemperatureInner;//内部温度
        double TemperatureEnviroment;//环境温度
        int UpPackCount;//上报包计数
    public:
        DeviceCheckResult();
        ~DeviceCheckResult();

        void Init(int boardNum, int chipNum);
        void Init();
        void Reset();
        void Clear();
        std::unique_ptr<TCOND::TestCondition[]>& GetCondition();        
        ChipCheckResult& GetChipCheckResult(int boardIndex, int chipIndex);
        void SetCheckCount(int checkCount);
        void SetCheckCompletedCount(int checkCompletedCount);
        void SetCheckedGroupCount(int count);
        void SetChipOnLineNum(int chipOnLineNum);
        void SetChipSatisfiedNum(int chipSatisfiedNum);
        void SetChipUnSatisfiedNum(int chipUnSatisfiedNum);
        void SetTemperatureInner(unsigned short t);
        void SetTemperatureEnv(unsigned short t);
        void SetUpPackCount(unsigned short count);
        int GetCheckCount() const;
        int GetCheckCompletedCount() const;
        int GetCheckedGroupCount() const;
        int GetChipOnLineNum() const;
        int GetChipSatisfiedNum() const;
        int GetChipUnSatisfiedNum() const;
        double GetTemperatureInner() const;
        double GetTempeartureEnv() const;
        void CheckCompletedCountInc();
        void CheckedGroupCountInc();
        void ChipOnlineNumInc();
        void ChipOnlineNumDec();
        void ChipSatisfiedNumInc();
        void ChipSatisfiedNumDec();
        void ChipUnSatisfiedNumInc();
        void ChipUnSatisfiedNumDec();
        double TemperatureTransferInner(unsigned short data);
        double TemperatureTransferEnv(unsigned short data);
        int GetUpPackCount() const;
    };

    extern std::unique_ptr<DeviceCheckResult> DeviceCheckResultGlobal;
};

#endif // _DEVICECHECKRESULT_HPP_