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

    void Range::operator=(const Range& range)
    {
        Left = range.Left;
        Right = range.Right;
    }

    void Range::SetRange(int left, int right)
    {
        Left = left;
        Right = right;
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

    void TestCondition::operator=(const TestCondition& as)
    {
        RangeSINAD = as.RangeSINAD;
        RangeVppPTP = as.RangeVppPTP;
        RangeVppRMS = as.RangeVppRMS;
    }

    void TestCondition::SetRangeSINAD(Range range)
    {
        RangeSINAD = range;
    }

    void TestCondition::SetRangeVppPTP(Range range)
    {
        RangeVppPTP = range;
    }

    void TestCondition::SetRangeVppRMS(Range range)
    {
        RangeVppRMS = range;
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
        //qDebug() << "[CheckSINAD] 输入sinad:" << sinad << "转换后:" << res << "区间:[" << RangeSINAD.GetLeft() << "," << RangeSINAD.GetRight() << "]";
        return res >= RangeSINAD.GetLeft() && res <= RangeSINAD.GetRight();
    }

    bool TestCondition::CheckVppPTP(unsigned short vpp) const
    {
        /*qDebug() << "[CheckVppPTP] 输入vpp:" << vpp
            << "区间:[" << RangeVppPTP.GetLeft() << "," << RangeVppPTP.GetRight() << "]";*/
        return vpp >= RangeVppPTP.GetLeft() && vpp <= RangeVppPTP.GetRight();
    }

    bool TestCondition::CheckVppRMS(unsigned short rms) const
    {
       /* qDebug() << "[CheckVppRMS] 输入rms:" << rms
            << "区间:[" << RangeVppRMS.GetLeft() << "," << RangeVppRMS.GetRight() << "]";*/
        return rms >= RangeVppRMS.GetLeft() && rms <= RangeVppRMS.GetRight();
    }

    bool TestCondition::CheckAll(unsigned short sinad, unsigned short vpp, unsigned short rms) const
    {
        return CheckSINAD(sinad) && CheckVppPTP(vpp) && CheckVppRMS(rms);
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

}; // namespace TCOND