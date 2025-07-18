#ifndef _FLUID_CTRL_HPP_
#define _FLUID_CTRL_HPP_

#include <atomic>
#include <memory>
#include <condition_variable>
#include <mutex>

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
		//std::condition_variable cv;//新增条件变量，用于等待流控
		//std::mutex mtx;//新增互斥锁，用于保护流控数据
    public:
        FluidCtrl();
        ~FluidCtrl();

        void Init();
        FLUID_TYPE FluidLoad(int Index);
        void FluidStore(int Index, FLUID_TYPE v);
        void FluidStore(FLUID_TYPE v);
		//void WaitForFluid(int index, FLUID_TYPE len, FLUID_TYPE remain);//新增等待流控函数
		//void NotifyFluid();//新增通知流控函数
        FLUID_TYPE FluidFetchSub(int Index, FLUID_TYPE sub);
        FluidCheckRes FluidCheck(int Index, FLUID_TYPE Len, FLUID_TYPE Remain);
        FluidCheckRes FluidCheckUpdate(int Index, FLUID_TYPE Len, FLUID_TYPE Remain);
    };

    extern std::unique_ptr<FluidCtrl> FluidCtrlGlob;
};

#endif // _FLUID_CTRL_HPP_