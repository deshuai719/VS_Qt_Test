#include "FluidCtrl.hpp"

namespace FCT{
    FluidCtrl::FluidCtrl(){}
    FluidCtrl::~FluidCtrl(){}

    void FluidCtrl::Init()
    {
        FluidStore(0);
    }

    FLUID_TYPE FluidCtrl::FluidLoad(int Index)                                               //读取指定通道（Index）的流控缓冲区的当前值。
    {
#if defined FLUID_IGNORE                                                                     //如果定义了 FLUID_IGNORE，则直接返回2048（用于测试或忽略流控）。
        return 2048;
#endif
        return FluidCtrlBuffer[Index].load();                                                //否则返回原子变量 FluidCtrlBuffer[Index] 的值。
    }

    void FluidCtrl::FluidStore(int Index, FLUID_TYPE v)                                      //设置指定通道（Index）的流控缓冲区的值为 v。
    {
        FluidCtrlBuffer[Index].store(v);
    }

    void FluidCtrl::FluidStore(FLUID_TYPE v)                                                 //将所有通道的流控缓冲区的值都设置为 v。
    {
        for(int i = 0; i < MAX_FLUID_NUM; i++)
        {
            FluidCtrlBuffer[i].store(v);
        }
    }

    FLUID_TYPE FluidCtrl::FluidFetchSub(int Index, FLUID_TYPE sub)                           //对指定通道的流控缓冲区做原子减法（减去 sub），并返回减法前的旧值。
    {
        return FluidCtrlBuffer[Index].fetch_sub(sub);
    }

    FluidCheckRes FluidCtrl::FluidCheck(int Index, FLUID_TYPE Len, FLUID_TYPE Remain)        //检查流控是否满足条件（不修改缓冲区的值）。
    {
#if defined FLUID_IGNORE
        return FLUID_SATISFY;
#endif
        if(FluidLoad(Index) > (Len + Remain))                                                //如果当前缓冲区剩余值大于 Len + Remain，返回 FLUID_SATISFY（允许发送）。
        {
            return FLUID_SATISFY;
        }
        else
        {
            return FLUID_REJECTION;                                                          //否则返回 FLUID_REJECTION（不允许发送）。
        }
    }

    FluidCheckRes FluidCtrl::FluidCheckUpdate(int Index, FLUID_TYPE Len, FLUID_TYPE Remain)  //检查流控并原子性地减少缓冲区的值（即“消耗”掉本次要发送的数据量）。
    {
#if defined FLUID_IGNORE
        return FLUID_SATISFY;
#endif
        if(FluidFetchSub(Index, Len) > Remain)                                               //如果减去 Len 后剩余值大于 Remain，返回 FLUID_SATISFY（允许发送）。
        {
            return FLUID_SATISFY;
        }
        else
        {
            return FLUID_REJECTION;                                                          //否则返回 FLUID_REJECTION（不允许发送）。
        }
    }

    std::unique_ptr<FluidCtrl> FluidCtrlGlob(new FluidCtrl);                                //创建一个全局唯一的 FluidCtrl 实例，供全局流控使用。
};