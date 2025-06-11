#ifndef _FLUID_CTRL_HPP_
#define _FLUID_CTRL_HPP_

#include <atomic>
#include <memory>

#define MAX_FLUID_NUM 37
#define FLUID_SIZE_INDEX0 2048
#define FLUID_TYPE long long
#define QFREE_SYM 0
#define QFREE_SIG 8
#define QFREE_AIC 16

// #define FLUID_IGNORE
#undef FLUID_IGNORE

namespace FCT{

    enum FluidCheckRes{//是否检查流控
        FLUID_SATISFY,
        FLUID_REJECTION,
    };

    class FluidCtrl{
        std::atomic<FLUID_TYPE> FluidCtrlBuffer[MAX_FLUID_NUM];
    public:
        FluidCtrl();
        ~FluidCtrl();

        void Init();
        FLUID_TYPE FluidLoad(int Index);
        void FluidStore(int Index, FLUID_TYPE v);
        void FluidStore(FLUID_TYPE v);
        FLUID_TYPE FluidFetchSub(int Index, FLUID_TYPE sub);
        FluidCheckRes FluidCheck(int Index, FLUID_TYPE Len, FLUID_TYPE Remain);
        FluidCheckRes FluidCheckUpdate(int Index, FLUID_TYPE Len, FLUID_TYPE Remain);
    };

    extern std::unique_ptr<FluidCtrl> FluidCtrlGlob;
};

#endif // _FLUID_CTRL_HPP_