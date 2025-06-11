#ifndef _SAMPLINGDATAGENERATE_HPP_
#define _SAMPLINGDATAGENERATE_HPP_

#include <memory>
#include <cmath>

#define PI 3.14159265358979323846
#define DEFAULT_SAMPLING_NUM 512

namespace SDG{
    class ARG{
        double dB;
        unsigned long long Freq;//Hz
        unsigned int Duration;//单位s
    public:
        ARG();
        ARG(double db, unsigned long long f, unsigned int du);
        ARG(const ARG& a);
        ~ARG();

        void operator=(const ARG& a);
        bool operator==(const ARG& a) const;
        double GetDB() const;
        unsigned long long GetFreq() const;//单位Hz
        unsigned int GetDur() const;//s
        void SetDB(const double);
        void SetFreq(const unsigned long long freq);
        void SetDuration(const unsigned int dur);
    };

    class SamplingDataGenerate{
        ARG Arg;
        static std::unique_ptr<double[]> cosBuffer;//存放余弦函数一个周期内平均512个采样点
    public:
        SamplingDataGenerate();
        SamplingDataGenerate(ARG a);
        ~SamplingDataGenerate();

        void SetArg(ARG a);
        ARG GetArg() const;
        
        static void cosBufferInit();
        static double GetSampVal(long long i);
        static void ReleaseCosBuffer();
    };
};

#endif //_SAMPLINGDATAGENERATE_HPP_