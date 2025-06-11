#include "SamplingDataGenerate.hpp"

namespace SDG{

    ARG::ARG():dB(0), Freq(0), Duration(0){}

    ARG::ARG(double db, unsigned long long f, unsigned int du):dB(db), Freq(f), Duration(du){}

    ARG::ARG(const ARG& a): dB(a.dB), Freq(a.Freq), Duration(a.Duration){}

    ARG::~ARG(){}

    void ARG::operator=(const ARG& a)
    {
        dB = a.GetDB();
        Freq = a.GetFreq();
        Duration = a.GetDur();
    }

    bool ARG::operator==(const ARG& a) const
    {
        return dB == a.GetDB() && Freq == a.GetFreq() && Duration == a.GetDur();
    }

    double ARG::GetDB() const
    {
        return dB;
    }

    unsigned long long ARG::GetFreq() const
    {
        return Freq;
    }

    unsigned int ARG::GetDur() const
    {
        return Duration;
    }

    void ARG::SetDB(const double db)
    {
        dB = db;
    }

    void ARG::SetFreq(const unsigned long long freq)
    {
        Freq = freq;
    }

    void ARG::SetDuration(const unsigned int dur)
    {
        Duration = dur;
    }

    //

    SamplingDataGenerate::SamplingDataGenerate():Arg(){}

    SamplingDataGenerate::SamplingDataGenerate(ARG a): Arg(a){}

    SamplingDataGenerate::~SamplingDataGenerate(){}

    void SamplingDataGenerate::SetArg(ARG a)
    {
        Arg = a;
    }

    ARG SamplingDataGenerate::GetArg() const
    {
        return Arg;
    }

    void SamplingDataGenerate::cosBufferInit()
    {
        if(cosBuffer == nullptr)
            cosBuffer = std::make_unique<double[]>(DEFAULT_SAMPLING_NUM);
        for(int i = 0; i < DEFAULT_SAMPLING_NUM; i++)
        {            
            cosBuffer[i] = std::cos(PI * i / 256);
        }
    }

    double SamplingDataGenerate::GetSampVal(long long i)
    {
        return cosBuffer[i % DEFAULT_SAMPLING_NUM];
    }

    void SamplingDataGenerate::ReleaseCosBuffer()
    {
        if(cosBuffer != nullptr)
        {
            cosBuffer.reset();
            cosBuffer = nullptr;
        }
    }

    std::unique_ptr<double[]> SamplingDataGenerate::cosBuffer = nullptr;

};