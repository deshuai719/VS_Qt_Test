#ifndef _FLUID_CTRL_HPP_
#define _FLUID_CTRL_HPP_

#include <atomic>
#include <memory>
#include <condition_variable>
#include <mutex>

#define MAX_FLUID_NUM 37
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
        // 新增：动态硬件缓存空间大小
        std::atomic<FLUID_TYPE> HardwareCacheSize{0};
        std::atomic<bool> CacheSizeInitialized{false};
        
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
        
        // 新增：硬件缓存空间管理方法
        void SetHardwareCacheSize(FLUID_TYPE size);
        FLUID_TYPE GetHardwareCacheSize() const;
        bool IsCacheSizeInitialized() const;
        
        // 新增：获取动态的FLUID_SIZE_INDEX0值
        FLUID_TYPE GetFluidSizeIndex0() const;
    };

    extern std::unique_ptr<FluidCtrl> FluidCtrlGlob;
};

#endif // _FLUID_CTRL_HPP_