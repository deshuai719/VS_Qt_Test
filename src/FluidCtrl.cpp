#include "FluidCtrl.hpp"

namespace FCT{
    FluidCtrl::FluidCtrl(){}
    FluidCtrl::~FluidCtrl(){}

    void FluidCtrl::Init()
    {
        FluidStore(0);
    }

    FLUID_TYPE FluidCtrl::FluidLoad(int Index)                                               //��ȡָ��ͨ����Index�������ػ������ĵ�ǰֵ��
    {
#if defined FLUID_IGNORE                                                                     //��������� FLUID_IGNORE����ֱ�ӷ���2048�����ڲ��Ի�������أ���
        return 2048;
#endif
        return FluidCtrlBuffer[Index].load();                                                //���򷵻�ԭ�ӱ��� FluidCtrlBuffer[Index] ��ֵ��
    }

    void FluidCtrl::FluidStore(int Index, FLUID_TYPE v)                                      //����ָ��ͨ����Index�������ػ�������ֵΪ v��
    {
        FluidCtrlBuffer[Index].store(v);
    }

    void FluidCtrl::FluidStore(FLUID_TYPE v)                                                 //������ͨ�������ػ�������ֵ������Ϊ v��
    {
        for(int i = 0; i < MAX_FLUID_NUM; i++)
        {
            FluidCtrlBuffer[i].store(v);
        }
    }

    FLUID_TYPE FluidCtrl::FluidFetchSub(int Index, FLUID_TYPE sub)                           //��ָ��ͨ�������ػ�������ԭ�Ӽ�������ȥ sub���������ؼ���ǰ�ľ�ֵ��
    {
        return FluidCtrlBuffer[Index].fetch_sub(sub);
    }

    FluidCheckRes FluidCtrl::FluidCheck(int Index, FLUID_TYPE Len, FLUID_TYPE Remain)        //��������Ƿ��������������޸Ļ�������ֵ����
    {
#if defined FLUID_IGNORE
        return FLUID_SATISFY;
#endif
        if(FluidLoad(Index) > (Len + Remain))                                                //�����ǰ������ʣ��ֵ���� Len + Remain������ FLUID_SATISFY�������ͣ���
        {
            return FLUID_SATISFY;
        }
        else
        {
            return FLUID_REJECTION;                                                          //���򷵻� FLUID_REJECTION���������ͣ���
        }
    }

    FluidCheckRes FluidCtrl::FluidCheckUpdate(int Index, FLUID_TYPE Len, FLUID_TYPE Remain)  //������ز�ԭ���Եؼ��ٻ�������ֵ���������ġ�������Ҫ���͵�����������
    {
#if defined FLUID_IGNORE
        return FLUID_SATISFY;
#endif
        if(FluidFetchSub(Index, Len) > Remain)                                               //�����ȥ Len ��ʣ��ֵ���� Remain������ FLUID_SATISFY�������ͣ���
        {
            return FLUID_SATISFY;
        }
        else
        {
            return FLUID_REJECTION;                                                          //���򷵻� FLUID_REJECTION���������ͣ���
        }
    }

    std::unique_ptr<FluidCtrl> FluidCtrlGlob(new FluidCtrl);                                //����һ��ȫ��Ψһ�� FluidCtrl ʵ������ȫ������ʹ�á�
};