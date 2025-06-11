#ifndef _TESTCONDITION_HPP_
#define _TESTCONDITION_HPP_

#include <memory>

namespace TCOND{

    class Range{
        int Left;
        int Right;
    public:
        Range();
        Range(int left, int right);
        Range(const Range& range);
        ~Range();

        void operator=(const Range& range);
        void SetRange(int left, int right);
        int GetLeft() const;
        int GetRight() const;
    };

    class TestCondition{
        Range RangeSINAD;
        Range RangeVppPTP;//Vpp峰峰值
        Range RangeVppRMS;//Vpp均方根
    public:
        TestCondition();
        TestCondition(const TestCondition& cp);
        ~TestCondition();

        void operator=(const TestCondition& as);
        void SetRangeSINAD(Range range);
        void SetRangeVppPTP(Range range);
        void SetRangeVppRMS(Range range);
        Range GetRangeSINAD() const;
        Range GetRangeVppPTP() const;
        Range GetRangeVppRMS() const;
        bool CheckSINAD(unsigned short sinad) const;
        bool CheckVppPTP(unsigned short vpp) const;
        bool CheckVppRMS(unsigned short rms) const;
        bool CheckAll(unsigned short sinad, unsigned short vpp, unsigned short rms) const;
        static short SinadTransfer(unsigned short sinad);
    };
};// namespace TCOND

#endif // _TESTCONDITION_HPP_