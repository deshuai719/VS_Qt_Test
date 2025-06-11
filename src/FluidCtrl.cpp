#include "FluidCtrl.hpp"

namespace FCT{
    FluidCtrl::FluidCtrl(){}
    FluidCtrl::~FluidCtrl(){}

    void FluidCtrl::Init()
    {
        FluidStore(0);
    }

    FLUID_TYPE FluidCtrl::FluidLoad(int Index)
    {
#if defined FLUID_IGNORE
        return 2048;
#endif
        return FluidCtrlBuffer[Index].load();
    }

    void FluidCtrl::FluidStore(int Index, FLUID_TYPE v)
    {
        FluidCtrlBuffer[Index].store(v);
    }

    void FluidCtrl::FluidStore(FLUID_TYPE v)
    {
        for(int i = 0; i < MAX_FLUID_NUM; i++)
        {
            FluidCtrlBuffer[i].store(v);
        }
    }

    FLUID_TYPE FluidCtrl::FluidFetchSub(int Index, FLUID_TYPE sub)
    {
        return FluidCtrlBuffer[Index].fetch_sub(sub);
    }

    FluidCheckRes FluidCtrl::FluidCheck(int Index, FLUID_TYPE Len, FLUID_TYPE Remain)
    {
#if defined FLUID_IGNORE
        return FLUID_SATISFY;
#endif
        if(FluidLoad(Index) > (Len + Remain))
        {
            return FLUID_SATISFY;
        }
        else
        {
            return FLUID_REJECTION;
        }
    }

    FluidCheckRes FluidCtrl::FluidCheckUpdate(int Index, FLUID_TYPE Len, FLUID_TYPE Remain)
    {
#if defined FLUID_IGNORE
        return FLUID_SATISFY;
#endif
        if(FluidFetchSub(Index, Len) > Remain)
        {
            return FLUID_SATISFY;
        }
        else
        {
            return FLUID_REJECTION;
        }
    }

    std::unique_ptr<FluidCtrl> FluidCtrlGlob(new FluidCtrl);
};