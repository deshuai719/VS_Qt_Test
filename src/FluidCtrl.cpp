//流控相关函数的设计
//

#include "FluidCtrl.hpp"
#include "LogWZ.hpp"
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

    /*************************新增等待函数的实现*******************************************/
    //void FluidCtrl::WaitForFluid(int index, FLUID_TYPE len, FLUID_TYPE remain) {
    //    std::unique_lock<std::mutex> lock(mtx);
    //    cv.wait(lock, [&] {
    //        return FluidCheck(index, len, remain) == FLUID_SATISFY;
    //        });
    //    // 满足条件后返回，发送线程继续
    //}

    //void FluidCtrl::NotifyFluid() {
    //    cv.notify_all();
    //}

    void FluidCtrl::FluidStore(int Index, FLUID_TYPE v)                                      //设置指定通道（Index）的流控缓冲区的值为 v。
    {
        // 优化：移除频繁的性能测量，减少开销
        static int store_count = 0;
        store_count++;
        
        // 只在每10000次操作时测量一次性能
        bool should_measure = (store_count % 10000 == 0);
        
        auto t_store_start = should_measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
        FluidCtrlBuffer[Index].store(v);
        auto t_store_end = should_measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
        
        if (should_measure) {
            auto store_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_store_end - t_store_start).count();
            // 只在耗时异常时记录日志（超过10us）
            if (store_duration > 10) {
                WRITE_TASK_DATA_SEND_DBG("FluidCtrlBuffer[%d].store异常耗时: %lld us (第%d次)\n", Index, store_duration, store_count);
            }
        }
    }

   /* void FluidCtrl::FluidStore(int Index, FLUID_TYPE v)
    {
        static std::chrono::steady_clock::time_point lastUpdate;
        static bool firstUpdate = true;

        auto now = std::chrono::steady_clock::now();
        if (!firstUpdate) {
            auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
            WRITE_TASK_DATA_SEND_DBG("流控更新间隔: %lld ms\n", interval);
        }
        else {
            firstUpdate = false;
        }
        lastUpdate = now;

        FluidCtrlBuffer[Index].store(v);
        NotifyFluid();
    }*/

    void FluidCtrl::FluidStore(FLUID_TYPE v)                                                 //将所有通道的流控缓冲区的值都设置为 v。
    {
        for(int i = 0; i < MAX_FLUID_NUM; i++)
        {
            FluidCtrlBuffer[i].store(v);
        }
        
    }
    
    FLUID_TYPE FluidCtrl::FluidFetchSub(int Index, FLUID_TYPE sub)                           //对指定通道的流控缓冲区做原子减法（减去 sub）
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