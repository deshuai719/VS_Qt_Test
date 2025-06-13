#include "TaskWZ.hpp"
#include <QDebug>

namespace TASKWZ{

task_wz::task_wz() :QObject(), type(TASK_EMPTY) {}

task_wz::task_wz(task_type t) :QObject(), type(t) {}

task_wz::~task_wz() {}

void task_destruct(task_wz& t)
{
	t.clear();
}

worker::worker()
{
	type = worker_type::ON_IDLE;
}

worker::~worker()
{
	clear();
}

void worker::create(task_wz* t, worker_type typ)
{
	if (t)
	{
		clear();
		task.set(t, 1);
		type = typ;
		t = 0;
	}
}

bool worker::execute()
{
	switch (type)
	{
	case ON_IDLE:
		break;
	case EXECUTE_THREAD:
	{
		std::thread t(worker_manager::task_callback_func, &task);
		th.swap(t);
		return true;
	}
	case EXECUTE_NO_THREAD:
	case JOIN_TASK_BY_TYPE:
	case END_TASK_BY_TYPE:
		task.addr->run();
		return true;
	case END_ALL_TASK:
		task.addr->run();
		return false;
	default:
		break;
	}
}

task_type worker::task_typ()
{
	if (type != ON_IDLE)
	{
		return task.addr->type;
	}
}

void worker::join()
{
	if (task.exist())
	{
		switch (type)
		{
		case ON_IDLE:
			break;
		case EXECUTE_THREAD:
			if (th.joinable())
			{
				th.join();
			}
			task.clear();
			type = worker_type::ON_IDLE;
			break;
		case EXECUTE_NO_THREAD:
		case JOIN_TASK_BY_TYPE:
		case END_TASK_BY_TYPE:
		case END_ALL_TASK:
			task.clear();
			type = worker_type::ON_IDLE;
			break;
		default:
			break;
		}
	}
}

void worker::close()
{
	if (task.exist())
	{
		switch (type)
		{
		case ON_IDLE:
			break;
		case EXECUTE_THREAD:
			task.get()->close();
			if (th.joinable())
			{
				th.join();
			}
			task.clear();
			type = worker_type::ON_IDLE;
			break;
		case EXECUTE_NO_THREAD:
		case JOIN_TASK_BY_TYPE:
		case END_TASK_BY_TYPE:
		case END_ALL_TASK:
			task.clear();
			type = worker_type::ON_IDLE;
			break;
		default:
			break;
		}
	}
}

void worker::clear()
{
	close();
}

void worker_destruct(worker& w)
{
	w.clear();
}

worker_manager::worker_manager()
{}

worker_manager::~worker_manager()
{
	clear();
}

void worker_manager::init(int n)//��UI�߳�ִ��
{
	workers.set(new worker[n + 5], n + 5);
	cnt = 0;
}

void worker_manager::begin_task_system()//��UI�߳�ִ��
{
	if (workers.exist())
	{
		if (cnt == 0)
		{
			create(new task_manager, worker_type::EXECUTE_THREAD);
			execute();
		}
	}
}

void worker_manager::end_task_system()//��UI�߳�ִ��
{
	if (workers.exist())
	{
		TOOLWZ::stack_wz<task_type> stack_end;
		stack_end.push(task_type::TASK_JOIN);
		stack_end.push(task_type::TASK_END);
		stack_end.push(task_type::TASK_DISPATCH);
		stack_end.push(task_type::TASK_RCV);
		stack_end.push(task_type::TASK_CHIP_STAT_PARSING);
		stack_end.push(task_type::TASK_RESULT_STATISTICS);
		stack_end.push(task_type::TASK_DATA_CONSTRUCT);
		stack_end.push(task_type::TASK_DATA_SEND);
		stack_end.push(task_type::TASK_VERSION_PARSING);
		stack_end.push(task_type::TASK_GENERAL);

		create(new task_end(stack_end), worker_type::END_ALL_TASK);
		join(task_type::TASK_MANAGER);//��task_manager����close�ǽ�ֹ��
		close(task_type::TASK_END);//�ոմ�����task_end����end�Լ�����������Ҫ�ֶ�end
		//��ʱ�����߳̽���
		cnt = 0;
		queue_worker.clear();
		stack_idle_index.reset();
		stack_end.clear();
	}
}

void worker_manager::create(task_wz* t, worker_type typ)//ui�߳�
{

	if (workers.exist())
	{
		if (t)
		{
			if (stack_idle_index.empty())
			{
				workers.addr[cnt].create(t, typ);
				queue_worker.front(cnt);
				cnt++;
                Semaphore.release(1);
			}
			else
			{
				int index;
				stack_idle_index.pop(index);
				workers.addr[index].create(t, typ);
				queue_worker.front(index);
                Semaphore.release(1);
			}
		}
	}
	t = 0;
}

bool worker_manager::execute()//type:task_manager-UI�߳�, ��������-task_manager�߳�
{
	if (workers.exist())
	{
        Semaphore.acquire(1);
		int index(0);
		queue_worker.rear(index);
		bool ret = workers.addr[index].execute();
		stack_running_type[workers.addr[index].task_typ()].push(index);
		return ret;
	}
	return false;
}

void worker_manager::join(task_type typ)//type:task_manager-UI�߳�, ��������-task_manager�߳�
{
	if (workers.exist())
	{
		while (!stack_running_type[typ].empty()) {
			int index;
			stack_running_type[typ].pop(index);
			workers.addr[index].join();
			stack_idle_index.push(index);
		}
	}
}

void worker_manager::close(task_type typ)//task_end��task_manager��ִ��
{
	if (workers.exist())
	{
		while (!stack_running_type[typ].empty()) {
			int index;
			stack_running_type[typ].pop(index);
			workers.addr[index].close();
			stack_idle_index.push(index);
		}
	}
}

void worker_manager::clear()//ui�̵߳��ã�ȷ�������߳̽����������ڴ�
{
	if (cnt == 0)
	{
		workers.clear();
		stack_idle_index.clear();
		for (int i = 0; i < TASK_TYPE_NUM; i++)
		{
			stack_running_type[i].clear();
		}
	}
}

void worker_manager::task_callback_func(TOOLWZ::data_wz<task_wz, 20, task_destruct>* t)
{
	t->addr->init();
	t->addr->run();
	t = 0;
}

TOOLWZ::data_wz<worker, sizeof(worker), worker_destruct> worker_manager::workers;
int worker_manager::cnt;
TOOLWZ::queue<int> worker_manager::queue_worker;
TOOLWZ::stack_wz<int, MAX_TASK_NUM> worker_manager::stack_idle_index;
TOOLWZ::stack_wz<int, MAX_TASK_NUM> worker_manager::stack_running_type[TASK_TYPE_NUM];
std::mutex worker_manager::mtx;
QSemaphore worker_manager::Semaphore(0);

//

task_manager::task_manager() :task_wz(task_type::TASK_MANAGER) {}

task_manager::~task_manager() {}

void task_manager::init() {}

void task_manager::run()
{
	while (1)
	{
		if (!worker_manager::execute())
		{
			break;
		}
	}
}

void task_manager::close() {}

void task_manager::clear() {}

//

task_join::task_join(TOOLWZ::stack_wz<task_type> stack_join)
	:task_wz(task_type::TASK_JOIN), 
	stack_join_type(stack_join) 
{

}

task_join::~task_join() { clear(); }

void task_join::init()
{}

void task_join::run()
{
	while (!stack_join_type.empty())
	{
		task_type typ;
		stack_join_type.pop(typ);
		worker_manager::join(typ);
	}
}

void task_join::close()
{}

void task_join::clear()
{
	stack_join_type.clear();
}

//

task_end::task_end(TOOLWZ::stack_wz<task_type> stack_end) :task_wz(task_type::TASK_END), stack_end_type(stack_end) {}
task_end::~task_end() { stack_end_type.clear(); }

void task_end::init() {}

void task_end::run()
{
	while (!stack_end_type.empty())
	{
		task_type typ;
		stack_end_type.pop(typ);
		worker_manager::close(typ);
	}
}

void task_end::close() {}

void task_end::clear()
{
	stack_end_type.clear();
}

//task_rcv

RcvData::RcvData():Data(nullptr), Len(0){}

RcvData::RcvData(int len):Data(new char[len]), Len(len){}

RcvData::RcvData(const RcvData& cp):Data(cp.GetData()), Len(cp.GetLen()){}

void RcvData::Clear()
{
	Data.reset();
}

void RcvData::operator=(const RcvData& as)
{
	Data = as.GetData();
	Len = as.GetLen();
}

const std::shared_ptr<char[]>& RcvData::GetData() const
{
	return Data;
}

void RcvData::SetLen(int len)
{
	Len = len;
}

int RcvData::GetLen() const
{
	return Len;
}

void RcvDataDestruct(RcvData& d)
{
	d.Clear();
}

//

task_rcv::task_rcv() :task_wz(TASK_RCV) {}

task_rcv::~task_rcv() { clear(); }

void task_rcv::init()
{
	Loop = TRUE;
}

void task_rcv::run()
{
	OPEN_TASK_RCV_DBG(LogTaskRcv.txt);
#ifndef TEST_WITHOUT_BOARD //DataFrameWz.hpp
	while(Loop)
	{
		RcvData Data(800);
		int res = SOCKWZ::SockGlob::Recv(Data.GetData().get(), 800);
		if(res == SOCKET_ERROR)
		{
			if(Loop == true)
			{
				// WRITE_TASK_RCV_DBG("Socket Error Continue\n");
				continue;
			}
			else
			{
				WRITE_TASK_RCV_DBG("Socket Error Break\n");
				Data.Clear();
				break;
			}
		}
		else
		{
			WRITE_TASK_RCV_DBG("Data recived, Len = %d\n", res);
		}
		Data.SetLen(res);
		QueueRcv.front(Data);
		Data.Clear();
	}
	WRITE_TASK_RCV_DBG("Cycle Over Loop = %d", Loop);
#else
	int sec(0);
	while(Loop)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		RcvData Data(800);
		FRAME_HEAD_STAR(Data.GetData().get())->msgID = 0x28;
		FRAME_HEAD_STAR(Data.GetData().get())->rev = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->ctlFractstop = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->ctlFractStrt = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->ctlFractMode = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->reserved_base = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->siglen = 784;

		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->timeStamp_8ms = sec;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->free_codec_dac = 2048;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->mnic_online = 0xFFFFFFFF;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->ds18b20_16b = 50;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->fpga_heat = 60;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->dbg_dn_aic = 0;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->cnt_dn_aic = 0;
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 12; j++)
			{
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].sinad = 33;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].reserved = 0;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].freqInHz = 64000;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].vpp = 20;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].rms = 30;
			}
		}
		Data.SetLen(788);
		QueueRcv.front(Data);
		Data.Clear();
	}
#endif

	CLOSE_TASK_RCV_DBG();
}

void task_rcv::close()
{
	WRITE_TASK_RCV_DBG("task_rcv::close\n");
	Loop = false;
}

void task_rcv::clear()
{
	QueueRcv.clear();
}

TOOLWZ::queue<RcvData, 500, RcvDataDestruct> task_rcv::QueueRcv;

//task_dispatch

task_dispatch::task_dispatch() :task_wz(TASK_DISPATCH) {}

task_dispatch::~task_dispatch() {}

void task_dispatch::init()
{
	Loop = true;
}

void task_dispatch::run()
{
	OPEN_TASK_DISPATCH_DBG(LogTaskDispatch.txt);
	while(Loop)
	{
		RcvData Data;
		task_rcv::QueueRcv.rear(Data);
		if(Data.GetData().get())
		{
			// WRITE_TASK_DISPATCH_DBG("Data recived, MsgId = %x\n", FRAME_HEAD_STAR(Data.GetData().get())->msgID);
			switch (FRAME_HEAD_STAR(Data.GetData().get())->msgID)
			{
			case 0x28:
				// WRITE_TASK_DISPATCH_DBG("0x28 recived\n");
				TaskChipStatParsing::QueueChipStatParsing.front(Data);
				break;	
			case 0xa0:
				// WRITE_TASK_DISPATCH_DBG("0xa0 recived\n");
				TaskVersionParsing::QueueVeriosnParsing.front(Data);
			default:
				break;
			}			
		}
		Data.Clear();
	}
	CLOSE_TASK_DISPATCH_DBG();
}

void task_dispatch::close()
{
	Loop = false;
}

void task_dispatch::clear()
{
	
}

//TaskChipParsing

TaskChipStatParsing::TaskChipStatParsing():task_wz(TASK_CHIP_STAT_PARSING){}

TaskChipStatParsing::~TaskChipStatParsing(){ clear(); }

void TaskChipStatParsing::init()
{
	Loop = TRUE;
}

/*
	在发线程发送完一个mif结束后等待流控为空并延时1s之后开始统计每个芯片的连续接收有效计数
*/

void TaskChipStatParsing::run()
{
	OPEN_TASK_0X28_PARSING(LogTask0X28Parsing.txt);
	while(Loop)
	{
		RcvData Data;
		QueueChipStatParsing.rear(Data);
		// WRITE_TASK_0X28_PARSING("Get Data From Que\n");
		if(Data.GetData().get())
		{
			// WRITE_TASK_0X28_PARSING("Data EXist\n");
			if(FRAME_HEAD_STAR(Data.GetData().get())->msgID == 0x28)
			{
				// WRITE_TASK_0X28_PARSING("0x28 recived\n");
				FCT::FluidCtrlGlob->FluidStore(0, PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->free_codec_dac);
				// WRITE_TASK_0X28_PARSING("Fluid 0 = %lld\n", FCT::FluidCtrlGlob->FluidLoad(0));
				// WRITE_TASK_0X28_PARSING("Temperature Chip:%u\n", PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->fpga_heat);
				DCR::DeviceCheckResultGlobal->SetTemperatureInner(PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->fpga_heat);
				// WRITE_TASK_0X28_PARSING("Temperature Env:%u\n", PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->ds18b20_16b);
				DCR::DeviceCheckResultGlobal->SetTemperatureEnv(PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->ds18b20_16b);
				DCR::DeviceCheckResultGlobal->SetUpPackCount(PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->timeStamp_8ms);
				if(bPackLogRecord)
				{
					WRITE_LOG_UP_PACK_RECORD(DCR::DeviceCheckResultGlobal->GetCheckCompletedCount(), DCR::DeviceCheckResultGlobal->GetUpPackCount());
				}
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 4; j++)
					{
						unsigned int IfOnline = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->mnic_online;
						bool ChipIfOnline = DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline();
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetIfOnline(GET_BIT_U32(IfOnline, i * 4 + j));
						if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline())
						{
							// WRITE_TASK_0X28_PARSING("Board:%d Chip:%d Online Current\n", i, j);
							if(!ChipIfOnline)
							{
								// WRITE_TASK_0X28_PARSING("Board:%d Chip:%d Offline Ever\n", i, j);
								DCR::DeviceCheckResultGlobal->ChipOnlineNumInc();
							}
							bool Res[3]{ true, true, true };
							unsigned short Sinad[3]{ 0 }, Vpp[3]{ 0 }, Rms[3]{ 0 };
							Sinad[0] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2].sinad;
							Vpp[0] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2].vpp;
							Rms[0] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2].rms;
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateRangeSINAD(Sinad[0]);
							Res[0] &= DCR::DeviceCheckResultGlobal->GetCondition()[0].CheckAll(Sinad[0], Vpp[0], Rms[0]);
							// WRITE_TASK_0X28_PARSING("Codec Left Siand = %u, Vpp = %u, RMS = %u, %s\n", Sinad, Vpp, Rms, Res ? "true" : "false");
							// WRITE_TASK_0X28_PARSING("Codec Left Siand Left = %d, Sinad Right = %d\n", 
							// 	DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD().GetLeft(),
							// 	DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD().GetRight());
							Sinad[1] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2 + 1].sinad;
							Vpp[1] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2 + 1].vpp;
							Rms[1] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2 + 1].rms;
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateRangeSINAD(Sinad[1]);
							Res[1] &= DCR::DeviceCheckResultGlobal->GetCondition()[0].CheckAll(Sinad[1], Vpp[1], Rms[1]);
							// WRITE_TASK_0X28_PARSING("Codec Right Siand = %u, Vpp = %u, RMS = %u, %s\n", Sinad, Vpp, Rms, Res ? "true" : "false");
							// WRITE_TASK_0X28_PARSING("Codec Left Siand Left = %d, Sinad Right = %d\n", 
							// 	DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD().GetLeft(),
							// 	DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD().GetRight());
							Sinad[2] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[8 + j].sinad;
							Vpp[2] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[8 + j].vpp;
							Rms[2] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[8 + j].rms;
							Res[2] &= DCR::DeviceCheckResultGlobal->GetCondition()[1].CheckAll(Sinad[2], Vpp[2], Rms[2]);
							// WRITE_TASK_0X28_PARSING("AdPow Siand = %u, Vpp = %u, RMS = %u, %s\n\n", Sinad, Vpp, Rms, Res ? "true" : "false");
							// if(Res)WRITE_TASK_0X28_PARSING("Res = true\n", true);
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).CheckPacksOfMifInc(Res[0] & Res[1] & Res[2]);

							if(bPackLogRecord)
							{
								WRITE_LOG_UP_CHIP_RECORD(i + 1, j + 1,
									TCOND::TestCondition::SinadTransfer(Sinad[0]), Vpp[0], Rms[0], Res[0] ? " TRUE" : "FALSE",
									TCOND::TestCondition::SinadTransfer(Sinad[1]), Vpp[1], Rms[1], Res[1] ? " TRUE" : "FALSE",
									TCOND::TestCondition::SinadTransfer(Sinad[2]), Vpp[2], Rms[2], Res[2] ? " TRUE" : "FALSE");
							}
						}
						else
						{
							// WRITE_TASK_0X28_PARSING("Board:%d Chip:%d Offline Current\n", i, j);
							if(ChipIfOnline)
							{
								// WRITE_TASK_0X28_PARSING("Board:%d Chip:%d Online Ever\n", i, j);
								DCR::DeviceCheckResultGlobal->ChipOnlineNumDec();
							}
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetCheckPacksOfMif(0);
						}
					}
				}
			}
		}
		Data.Clear();
	}
	CLOSE_TASK_0X28_PARSING();
}

void TaskChipStatParsing::close()
{
	Loop = FALSE;
}

void TaskChipStatParsing::clear()
{
	QueueChipStatParsing.clear();
}

TOOLWZ::queue<RcvData, 500, RcvDataDestruct> TaskChipStatParsing::QueueChipStatParsing;
bool TaskChipStatParsing::bPackLogRecord(false);

//TaskTestStatistics

MsgToCentralWt::MsgToCentralWt()
{}

MsgToCentralWt::MsgToCentralWt(TestStat s, StatisticMode m)
	:Stat(s), Mode(m)
{}

MsgToCentralWt::MsgToCentralWt(const MsgToCentralWt& cp)
	:Stat(cp.GetStat()), Mode(cp.GetMode())
{}

TestStat MsgToCentralWt::GetStat() const
{
	return Stat;
}

StatisticMode MsgToCentralWt::GetMode() const
{
	return Mode;
}

//

TaskTestStatistics::TaskTestStatistics():task_wz(TASK_RESULT_STATISTICS){}

TaskTestStatistics::~TaskTestStatistics(){ clear(); }

void TaskTestStatistics::init()
{
	Loop = true;
}

void TaskTestStatistics::run()
{
	OPEN_TASK_STATISTICS_DBG(LogTaskStatistics.txt);
	WRITE_TASK_STATISTICS_DBG("TaskTestStatistics::run()\n");
	while (Loop)
	{
		StatisticMode Mode(StatisticMode::MODE_DEFAULT);
		QueueStatistcsMODE.rear(Mode);
		switch (Mode)
		{
		case MODE_DEFAULT:
			// WRITE_TASK_STATISTICS_DBG("MODE DEFAULT\n");
			break;
		case STATISTICS_A_MIF:
			WRITE_TASK_STATISTICS_DBG("Mode :STATISTICS_A_MIF\n");
			for(int i = 0; i < 8; i ++)
			{
				for(int j = 0; j < 4; j++)
				{
					// WRITE_TASK_STATISTICS_DBG("Satisfied pack num: %d, Valid num: %d\n", DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckPacksOfMif(), ContinousValidNum);
					if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckPacksOfMif() < ContinousValidNum)
					{
						WRITE_TASK_STATISTICS_DBG("Check Result A Mif: False\n");
						if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckResult())
						{
							DCR::DeviceCheckResultGlobal->ChipSatisfiedNumDec();
							if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline())
							{
								// WRITE_TASK_STATISTICS_DBG("Previous result a mif: True\n");
								DCR::DeviceCheckResultGlobal->ChipUnSatisfiedNumInc();
							}
						}
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateCheckResult(false);
					}
					else
					{
						WRITE_TASK_STATISTICS_DBG("Check Result A Mif: True\n");
						// if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline() 
						// 	&& DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckResult())
						// {
						// 	DCR::DeviceCheckResultGlobal->ChipSatisfiedNumInc();
						// }
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateCheckResult(true);
					}
					// DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetCheckPacksOfMif(0);
					// DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetChipTestStat(DCR::WAITING_FOR_TESTING);
				}
			}
			emit MsgOfStatistics(MsgToCentralWt(TestStat::TEST_RUNNING, StatisticMode::STATISTICS_A_MIF));
			break;
		case STATISTICS_A_TIME:
			WRITE_TASK_STATISTICS_DBG("Mode :STATISTICS_A_TIME\n");
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckResult())
					{
						WRITE_TASK_STATISTICS_DBG("A TIME Res True\n");
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).CheckSatisfiedCountInc();
					}
				}
			}
			emit MsgOfStatistics(MsgToCentralWt(TestStat::TEST_RUNNING, StatisticMode::STATISTICS_A_TIME));
			break;
		default:
			break;
		}
	}
	CLOSE_TASK_STATISTICS_DBG();
}

void TaskTestStatistics::close()
{
	Loop = false;
}

void TaskTestStatistics::clear()
{}

long long TaskTestStatistics::ContinousValidNum = 0;
TOOLWZ::queue<StatisticMode> TaskTestStatistics::QueueStatistcsMODE;
// StatisticMode TaskTestStatistics::Mode;

//TaskDataConstruct

TaskDataConstruct::TaskDataConstruct(FLST::FileList<FPWZ::ArgDM>& FileLst) 
	:task_wz(TASK_DATA_CONSTRUCT), FileList(FileLst){}

TaskDataConstruct::~TaskDataConstruct() { clear(); }

void TaskDataConstruct::init()
{}

void TaskDataConstruct::run()
{
	OPEN_TASK_CONSTRUCT_DBG(LogTaskConstruct.txt);
	for(auto it = FileList.GetHead(); it != nullptr; it = it->GetNext())
	{
		switch(it->GetOpt())
		{
		case FLST::FileOPT::ADD_FILE:
			WRITE_TASK_CONSTRUCT_DBG("AddNode(), Path:%s\n", it->GetData().GetPath().toStdString().c_str());
			DCWZ::DataMana::DataListGlobal.AddNode(it->GetData());
			WRITE_TASK_CONSTRUCT_DBG("AddNode Complete\n");
			break;
		case FLST::FileOPT::DEL_FIEL:
			DCWZ::DataMana::DataListGlobal.DelNode(it->GetData());
			break;
		default:
			break;
		}
	}
	FileList.Clear();
	CLOSE_TASK_CONSTRUCT_DBG();
}

void TaskDataConstruct::close()
{}

void TaskDataConstruct::clear()
{
	FileList.Clear();
}

//TaskDataConstructARG

TaskDataConstructARG::TaskDataConstructARG(FLST::FileList<DCWZ::ARG_RTC_GENERATE>& ArgL)
	:task_wz(TASK_DATA_CONSTRUCT), ArgList(ArgL)
{}

TaskDataConstructARG::~TaskDataConstructARG()
{
	clear();
}

void TaskDataConstructARG::init(){}

void TaskDataConstructARG::run()
{
	OPEN_TASK_CONSTRUCT_ARG_DBG(LogTaskConstructArg.txt);
	for(auto it = ArgList.GetHead(); it != nullptr; it = it->GetNext())
	{
		switch (it->GetOpt())
		{
		case FLST::FileOPT::ADD_FILE:
			DCWZ::DataMana::DataListGlobal.AddNode(it->GetData());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run(), ADD_FILE\n");
			break;
		case FLST::FileOPT::DEL_FIEL:
			DCWZ::DataMana::DataListGlobal.DelNode(it->GetData());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run(), DEL_FILE\n");
			break;
		case FLST::FileOPT::EXCHANGE:
			DCWZ::DataMana::DataListGlobal.Exchange(it->GetData(), it->GetExchg());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run(), EXCHANGE\n");
			break;
		case FLST::FileOPT::TO_BOTTOM:
			DCWZ::DataMana::DataListGlobal.ToBottom(it->GetData());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run, TO_BOTTOM\n");
			break;
		default:
			break;
		}
	}
	CLOSE_TASK_CONSTRUCT_ARG_DBG();
}

void TaskDataConstructARG::close(){}

void TaskDataConstructARG::clear()
{
	ArgList.Clear();
}

//TaskDataSend

TaskDataSend::TaskDataSend(long long cnt):task_wz(TASK_DATA_SEND), TestCount(cnt){}

TaskDataSend::~TaskDataSend(){ clear(); }

void TaskDataSend::init()
{
	Loop = true;
}

void TaskDataSend::run()
{
	OPEN_TASK_DATA_SEND_DBG(LogTaskDataSend.txt);
	OPEN_TASK_DATA_SEND_INFO_VERIFY(LogTaskDataSendInfoVerify.txt);
	OPEN_LOG_UP_RECORD(LogUpRecord.log);
	emit MsgOfStartEnd(MsgToCentralWt(TestStat::TEST_START, StatisticMode::STATISTICS_A_MIF));
	SemaWaitForUI.acquire(1);
	// DCR::DeviceCheckResultGlobal->Init();
	// DCR::DeviceCheckResultGlobal->SetCheckCount(TestCount);
	if(DCWZ::DataMana::DataListGlobal.GetHead() == nullptr)
	{
		emit MsgOfStartEnd(MsgToCentralWt(TestStat::TEST_OVER, StatisticMode::STATISTICS_A_MIF));
		CLOSE_TASK_DATA_SEND_DBG();
		return;
	}
	while(Loop && TestCount--)
	{	
		WRITE_TASK_DATA_SEND_DBG("TestCount = %lld\n", TestCount);
		WRITE_LOG_UP_RECORD("\n[本次参数下发开始:%04d]\n", DCR::DeviceCheckResultGlobal->GetCheckCompletedCount() + 1);
		std::shared_ptr<DCWZ::DataNode> Node = DCWZ::DataMana::DataListGlobal.GetHead();
		for(int i = 0; Node != nullptr && Loop; Node = Node->GetNext(), i++)
		{
			WRITE_TASK_DATA_SEND_DBG("Send Node\n");
			// WRITE_TASK_DATA_SEND_DBG("dB = %f, Freq = %llu, DL = %x, DR = %x, AL = %x, AR = %x\n", 
			// 	Node->GetData()->GetSDG().GetArg().GetDB(), Node->GetData()->GetSDG().GetArg().GetFreq(),
			// 	Node->GetData()->GetRegCFG().GetDL(), Node->GetData()->GetRegCFG().GetDR(),
			// 	Node->GetData()->GetRegCFG().GetAL(), Node->GetData()->GetRegCFG().GetAR());
			Node->GetData()->UpdateCfgCMD();
			SOCKWZ::SockGlob::Send((char*)DCWZ::DataConstruct::GetRegCfgCMD(), 0x51 * 4 + 1);
			// WRITE_TASK_DATA_SEND_DBG("Send RegCfg\n");
			WRITE_LOG_UP_RECORD("\n[本组参数下发开始:%04d]\n", i + 1);
			TaskChipStatParsing::bPackLogRecord = true;
			for(int i = 0; i < Node->GetData()->GetSendNum(); )
			{
				// WRITE_TASK_DATA_SEND_DBG("GetPackInfo(i) Before\n");
				DCWZ::PackInfo& Pack = Node->GetData()->GetPackInfo(i);
				// WRITE_TASK_DATA_SEND_DBG("GetPackInfo(i) After\n");
				// WRITE_TASK_DATA_SEND_DBG("Fluid 0 = %llx, PackLen = %lld\n", FCT::FluidCtrlGlob->FluidLoad(0), Pack.GetSegAll().GetLen());
				if(FCT::FluidCtrlGlob->FluidCheckUpdate(0, Pack.GetSegAll().GetLen(), FLUID_SIZE_INDEX0 / 4) == FCT::FluidCheckRes::FLUID_SATISFY)
				{
					// WRITE_TASK_DATA_SEND_DBG("Fluid Val = %lld, Send Data Len = %lld\n", FCT::FluidCtrlGlob->FluidLoad(0), Pack.GetSegAll().GetLen());
#ifndef TEST_WITHOUT_BOARD
					SOCKWZ::SockGlob::Send(Pack.GetPackData(), Pack.GetSegAll().GetLen());
					// WRITE_TASK_DATA_SEND_INFO_VERIFY(Pack.GetPackData(), Pack.GetSegAll().GetLen());
					// WRITE_TASK_DATA_SEND_DBG("Send Pack\n");
#else
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif
					i++;
				}
				if(!Loop)
				{
					WRITE_TASK_DATA_SEND_DBG("break\n");
					break;
				}
			}
			WRITE_TASK_DATA_SEND_DBG("Get Fluid\n");
			for(; FCT::FluidCtrlGlob->FluidLoad(0) < FLUID_SIZE_INDEX0; )
			{
				if(Loop)
				{
					std::this_thread::sleep_for(std::chrono::duration<long long, std::ratio<1, 100>>(1));
				}
				else
				{
					WRITE_TASK_DATA_SEND_DBG("break\n");
					break;
				}
			}
			WRITE_LOG_UP_RECORD("\n[本组参数下发结束:%04d]\n", i + 1);
			TaskChipStatParsing::bPackLogRecord = false;
			WRITE_TASK_DATA_SEND_DBG("Fluid Buffer Empty\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			TaskTestStatistics::ContinousValidNum = Node->GetData()->GetDuration() * 125 - 2;
			StatisticMode Mode = TASKWZ::StatisticMode::STATISTICS_A_MIF;
			TaskTestStatistics::QueueStatistcsMODE.front(Mode);
			WRITE_TASK_DATA_SEND_DBG("Semaphore Release Statistics a mif\n");
			SemaWaitForUI.acquire(1);
			WRITE_TASK_DATA_SEND_DBG("Semaphore Acquire Statistics a mif\n");
		}
		// DCR::DeviceCheckResultGlobal->CheckCompletedCountInc();
		StatisticMode Mode = TASKWZ::StatisticMode::STATISTICS_A_TIME;
		TaskTestStatistics::QueueStatistcsMODE.front(Mode);

		WRITE_TASK_DATA_SEND_DBG("Semaphore Release Statistics a time\n");
		SemaWaitForUI.acquire(1);
		WRITE_TASK_DATA_SEND_DBG("Semaphore Acquire Statistics a time\n");
		WRITE_LOG_UP_RECORD("\n[本次参数下发结束:%04d]\n", DCR::DeviceCheckResultGlobal->GetCheckCompletedCount());
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	emit MsgOfStartEnd(MsgToCentralWt(TestStat::TEST_OVER, StatisticMode::STATISTICS_A_MIF));
	WRITE_TASK_DATA_SEND_DBG("emit MsgOfStartEnd(TEST_OVER)\n");
	// std::this_thread::sleep_for(std::chrono::milliseconds(500));
	SemaWaitForUI.acquire(1);
	WRITE_TASK_DATA_SEND_DBG("Semaphore Acquire Over\n");
	CLOSE_LOG_UP_RECORD();
	CLOSE_TASK_DATA_SEND_DBG();
	CLOSE_TASK_DATA_SEND_INFO_VERIFY();
}

void TaskDataSend::close()
{
	Loop = false;
	TASKWZ::TaskDataSend::SemaWaitForUI.release(1);
}

void TaskDataSend::clear()
{}

QSemaphore TaskDataSend::SemaWaitForUI(0);

//版本号解析

TaskVersionParsing::TaskVersionParsing()
	:task_wz(TASK_VERSION_PARSING)
{}

TaskVersionParsing::~TaskVersionParsing()
{}

void TaskVersionParsing::init()
{
	Loop = true;
}

void TaskVersionParsing::run()
{
	OPEN_TASK_VERSION_GET_DBG(LogTaskVersionGet.txt);

	//读取版本号

	RcvData DataDown(12);
	FRAME_HEAD_STAR(DataDown.GetData().get())->msgID = 0xa0;
	FRAME_HEAD_STAR(DataDown.GetData().get())->rev = 0xf;
	FRAME_HEAD_STAR(DataDown.GetData().get())->ctlFractstop = 0;
	FRAME_HEAD_STAR(DataDown.GetData().get())->ctlFractStrt = 0;
	FRAME_HEAD_STAR(DataDown.GetData().get())->ctlFractMode = 0;
	FRAME_HEAD_STAR(DataDown.GetData().get())->reserved_base = 0;
	FRAME_HEAD_STAR(DataDown.GetData().get())->siglen = 8;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->data_bound_num8b = 1;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->read_flash2ram = 0;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->read_ram = 1;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->write_ram = 0;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->chksum_head = 0;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->data_depth = 8;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->addr_start = 0xd0;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->ram_id = 24;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->reserved3 = 0;
	PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->chksum_head = TOOLWZ::AccVerify(DataDown.GetData().get() + HEAD_DATA_LEN, 8);
	// WRITE_TASK_VERSION_GET_DBG("chksum_head = 0X%x\n", PTR_DN_ARG_A0(DataDown.GetData().get() + HEAD_DATA_LEN)->chksum_head);
	RcvData DataUp;
	while(Loop)
	{
		SOCKWZ::SockGlob::Send(DataDown.GetData().get(), DataDown.GetLen());
		for(int i = 0; i < 100; i++)
		{
			QueueVeriosnParsing.rear(DataUp);
			if(DataUp.GetData())
			{
				WRITE_TASK_VERSION_GET_DBG("ram_id = %d, chk_fail = %d, cmd_ack = %d\n",
					PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->ram_id,
					PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->chk_fail,
					PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->cmd_ack);
				if(PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->ram_id == 24
				&& PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->chk_fail == 0
				&& PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->cmd_ack == 1)
				{
					WRITE_TASK_VERSION_GET_DBG("Data[0] = 0x%02x Data[1] = 0x%02x Data[2] = 0x%02x Data[3] = 0x%02x Data[4] = 0x%02x Data[5] = 0x%02x Data[6] = 0x%02x Data[7] = 0x%02x\n",
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[0], 
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[1], 
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[2], 
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[3],
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[4],
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[5], 
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[6], 
						((unsigned char*)(DataUp.GetData().get() + HEAD_DATA_LEN))[7]);
					Version.SEC = PTR_VERSION_UP(DataUp.GetData().get() + 12)->SEC;
					Version.MIN = PTR_VERSION_UP(DataUp.GetData().get() + 12)->MIN;
					Version.HOUR = PTR_VERSION_UP(DataUp.GetData().get() + 12)->HOUR;
					Version.DAY = PTR_VERSION_UP(DataUp.GetData().get() + 12)->DAY;
					Version.MONTH = PTR_VERSION_UP(DataUp.GetData().get() + 12)->MONTH;
					Version.YEAR = PTR_VERSION_UP(DataUp.GetData().get() + 12)->YEAR;
					Version.VER = PTR_VERSION_UP(DataUp.GetData().get() + 12)->VER;
					emit UpdateBoardVersion();
					Loop = false;
					break;
				}
					//切换到外部源

				uint8_t SwitchingLocalSRC[14] = { 0xA0, 0x0F, 0x0A, 0x00, 0x81, 0x15, 0x01, 0x00, 0x50, 0x00, 0x18, 0x00, 0x02, 0xFD };
				SOCKWZ::SockGlob::Send((char*)SwitchingLocalSRC, 14);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	}
	DataDown.Clear();
	DataUp.Clear();
	CLOSE_TASK_VERSION_GET_DBG();
}

void TaskVersionParsing::close()
{
	Loop = false;
}

void TaskVersionParsing::clear()
{}

TOOLWZ::queue<RcvData, 500, RcvDataDestruct> TaskVersionParsing::QueueVeriosnParsing;
DF::VERSION_UP TaskVersionParsing::Version = { 0, 0, 0 };

};