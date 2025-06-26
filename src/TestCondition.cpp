#include "TestCondition.hpp"
#include <QDebug> 

namespace TCOND{
    Range::Range()
        :Left(0), Right(0)
    {}

    Range::Range(int left, int right)
        :Left(left), Right(right)
    {}

    Range::Range(const Range& range)
        :Left(range.Left), Right(range.Right)
    {}

    Range::~Range()
    {}

   /* void Range::operator=(const Range& range)
    {
        Left = range.Left;
        Right = range.Right;
    }*/

    Range& Range::operator=(const Range& range)
    {
        if (this != &range) {
            Left = range.Left;
            Right = range.Right;
        }
        return *this;
    }

    void Range::SetRange(int left, int right)
    {
        Left = left;
        Right = right;
        qDebug() << "[Range::SetRange] 设置为:" << Left << Right;
    }

    int Range::GetLeft() const
    {
        return Left;
    }

    int Range::GetRight() const
    {
        return Right;
    }

    TestCondition::TestCondition()
        :RangeSINAD(), RangeVppPTP(), RangeVppRMS()
    {}

    TestCondition::TestCondition(const TestCondition& cp)
        :RangeSINAD(cp.RangeSINAD), RangeVppPTP(cp.RangeVppPTP), RangeVppRMS(cp.RangeVppRMS)
    {}

    TestCondition::~TestCondition()
    {}

    TestCondition& TestCondition::operator=(const TestCondition& as) {
        if (this != &as) {
            RangeSINAD = as.RangeSINAD;
            RangeVppPTP = as.RangeVppPTP;
            RangeVppRMS = as.RangeVppRMS;
        }
        return *this;
    }

    
    void TestCondition::SetRangeSINAD(Range range)
    {
        RangeSINAD = range;
        qDebug() << "[SetRangeSINAD] 设置为:" << range.GetLeft() << range.GetRight();
    }

    void TestCondition::SetRangeVppPTP(Range range)
    {
        RangeVppPTP = range;
        qDebug() << "[SetRangeVppPTP] 设置为:" << range.GetLeft() << range.GetRight();
    }

    void TestCondition::SetRangeVppRMS(Range range)
    {
        RangeVppRMS = range;
        qDebug() << "[SetRangeVppRMS] 设置为:" << range.GetLeft() << range.GetRight();
    }

    Range& TestCondition::GetRangeSINAD() {
        return RangeSINAD;
    }
    const Range& TestCondition::GetRangeSINAD() const {
        return RangeSINAD;
    }

    Range& TestCondition::GetRangeVppPTP() {
        return RangeVppPTP;
    }
    const Range& TestCondition::GetRangeVppPTP() const {
        return RangeVppPTP;
    }

    Range& TestCondition::GetRangeVppRMS() {
        return RangeVppRMS;
    }
    const Range& TestCondition::GetRangeVppRMS() const {
        return RangeVppRMS;
    }

    //Range TestCondition::GetRangeVppPTP() const
    //{
    //    return RangeVppPTP;
    //}

    //Range TestCondition::GetRangeVppRMS() const
    //{
    //    return RangeVppRMS;
    //}

    bool TestCondition::CheckSINAD(unsigned short sinad) const
    {
        short res = SinadTransfer(sinad);
        return res >= RangeSINAD.GetLeft() && res <= RangeSINAD.GetRight();
    }

    bool TestCondition::CheckVppPTP(unsigned short vpp) const
    {
        return vpp >= RangeVppPTP.GetLeft() && vpp <= RangeVppPTP.GetRight();
    }

    bool TestCondition::CheckVppRMS(unsigned short rms) const
    {
        return rms >= RangeVppRMS.GetLeft() && rms <= RangeVppRMS.GetRight();
    }

    bool TestCondition::CheckAll(unsigned short sinad, unsigned short vpp, unsigned short rms) const
    {
        //return CheckSINAD(sinad) && CheckVppPTP(vpp) && CheckVppRMS(rms);
        bool sinadOk = CheckSINAD(sinad);
        bool vppOk = CheckVppPTP(vpp);
        bool rmsOk = CheckVppRMS(rms);

        /**********************************新增：检查值是否被正确传入*************************************************/
       /* qDebug() << "[CheckAll]"
            << "SINAD:" << sinad << "in [" << RangeSINAD.GetLeft() << "," << RangeSINAD.GetRight() << "]" << (sinadOk ? "OK" : "NG")
            << "VppPTP:" << vpp << "in [" << RangeVppPTP.GetLeft() << "," << RangeVppPTP.GetRight() << "]" << (vppOk ? "OK" : "NG")
            << "VppRMS:" << rms << "in [" << RangeVppRMS.GetLeft() << "," << RangeVppRMS.GetRight() << "]" << (rmsOk ? "OK" : "NG");*/

        return sinadOk && vppOk && rmsOk;
    }

    short TestCondition::SinadTransfer(unsigned short sinad)
    {
        if(sinad < 2048)
        {
            return sinad / 16;
        }
        else
        {
            return (sinad - 4096) / 16;
        }
    }
/*******************************新增：打印函数实现*******************************************/
    void TestCondition::Print(const QString& name) const 
    {
        qDebug() << (name.isEmpty() ? "[TestCondition]" : name)
            << "RangeSINAD:[" << RangeSINAD.GetLeft() << "," << RangeSINAD.GetRight() << "]"
            << "RangeVppPTP:[" << RangeVppPTP.GetLeft() << "," << RangeVppPTP.GetRight() << "]"
            << "RangeVppRMS:[" << RangeVppRMS.GetLeft() << "," << RangeVppRMS.GetRight() << "]";
    }
}; // namespace TCOND