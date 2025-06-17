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


//bool TaskDataSend::ReadyToSendFirstPack = false;//新增变量，判断是否可以发送第一个包

bool worker::execute()
{
	switch (type)                                                // 根据当前 worker 的类型，决定如何执行任务
	{
	case ON_IDLE:
		break;                                                   // 空闲状态，不做任何事，直接跳出 switch
	case EXECUTE_THREAD:
	{
		std::thread t(worker_manager::task_callback_func, &task);// 创建一个新线程，执行 worker_manager::task_callback_func，参数为当前 worker 的 task
		th.swap(t);                                              // 把新线程对象 t 赋值给成员变量 th（管理线程生命周期）
		return true;                                             // 返回 true，表示已启动线程
	}
	case EXECUTE_NO_THREAD:
	case JOIN_TASK_BY_TYPE:
	case END_TASK_BY_TYPE:
		task.addr->run();                                        // 这几种类型都直接在当前线程中执行 task 的 run() 方法
		return true;                                             // 返回 true，表示已执行
	case END_ALL_TASK:
		task.addr->run();                                        // 结束所有任务时，也直接执行 run()，但返回 false
		return false;                                            // 返回 false，表示特殊结束
	default:
		break;                                                   // 其他未知类型，不做任何事
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
	if (task.exist())                    // 1. 如果当前 worker 持有有效任务（task 不为空）
	{
		switch (type)                    // 2. 根据 worker 当前的类型，决定如何处理
		{
		case ON_IDLE:
			break;                       // 3. 如果是空闲状态，不做任何操作
		case EXECUTE_THREAD:
			if (th.joinable())           // 4. 如果是线程执行模式，并且线程对象 th 可 join
			{
				th.join();               // 5. 等待线程执行结束（阻塞当前线程，直到 th 线程结束）
			}
			task.clear();                // 6. 清理 task（释放任务资源）
			type = worker_type::ON_IDLE; // 7. 设置 worker 状态为 ON_IDLE
			break;
		case EXECUTE_NO_THREAD:
		case JOIN_TASK_BY_TYPE:
		case END_TASK_BY_TYPE:
		case END_ALL_TASK:
			task.clear();                // 8. 这些类型下，直接清理 task
			type = worker_type::ON_IDLE; // 9. 设置 worker 状态为 ON_IDLE
			break;
		default:
			break;                       // 10. 其他未知类型，不做任何操作
		}
	}
}

//安全地关闭并回收任务资源
void worker::close()
{
	if (task.exist())                    // 1. 判断当前 worker 是否持有有效任务（task 不为空）
	{
		switch (type)                    // 2. 根据 worker 当前的类型，决定如何关闭任务
		{
		case ON_IDLE:
			break;                       // 3. 如果是空闲状态，不做任何操作
		case EXECUTE_THREAD:
			task.get()->close();         // 4. 如果是线程执行模式，先调用任务对象的 close() 方法（通知任务停止）
			if (th.joinable())           // 5. 如果线程对象 th 可 join（线程还在运行）
			{
				th.join();               // 6. 等待线程执行结束（阻塞当前线程，直到 th 线程结束）
			}
			task.clear();                // 7. 清理 task（释放任务资源）
			type = worker_type::ON_IDLE; // 8. 设置 worker 状态为 ON_IDLE
			break;
		case EXECUTE_NO_THREAD:
		case JOIN_TASK_BY_TYPE:
		case END_TASK_BY_TYPE:
		case END_ALL_TASK:
			task.clear();                // 9. 这些类型下，直接清理 task
			type = worker_type::ON_IDLE; // 10. 设置 worker 状态为 ON_IDLE
			break;
		default:
			break;                       // 11. 其他未知类型，不做任何操作
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

//实现了任务系统的安全关闭，通过统一调度和资源回收，确保所有任务和线程都能被有序终止，避免资源泄漏和线程悬挂。
void worker_manager::end_task_system()                             // UI线程执行
{
	if (workers.exist())                                           // 1. 判断当前是否有worker存在（即任务系统已初始化）
	{
		TOOLWZ::stack_wz<task_type> stack_end;                     // 2. 创建一个任务类型的栈，用于存放需要结束的任务类型
		stack_end.push(task_type::TASK_JOIN);                      // 3. 依次将所有需要结束的任务类型压入栈
		stack_end.push(task_type::TASK_END);
		stack_end.push(task_type::TASK_DISPATCH);
		stack_end.push(task_type::TASK_RCV);
		stack_end.push(task_type::TASK_CHIP_STAT_PARSING);
		stack_end.push(task_type::TASK_RESULT_STATISTICS);
		stack_end.push(task_type::TASK_DATA_CONSTRUCT);
		stack_end.push(task_type::TASK_DATA_SEND);
		stack_end.push(task_type::TASK_VERSION_PARSING);
		stack_end.push(task_type::TASK_GENERAL);

		create(new task_end(stack_end), worker_type::END_ALL_TASK);// 4. 创建一个 task_end 任务，类型为 END_ALL_TASK，交由 worker 管理
		join(task_type::TASK_MANAGER);                             // 5. 等待 task_manager 类型的任务全部结束（join会阻塞直到所有相关worker结束）
		close(task_type::TASK_END);                                // 6. 关闭所有 task_end 类型的任务（确保 task_end 彻底结束）
                                                                   // 7. 清理worker管理器的内部状态
		cnt = 0;                                                   // 8. worker计数归零
		queue_worker.clear();                                      // 9. 清空worker队列
		stack_idle_index.reset();                                  // 10. 重置空闲worker索引栈
		stack_end.clear();                                         // 11. 清空本地的stack_end栈
	}
}

//实现了任务的分配与 worker 复用，优先复用空闲 worker，避免重复分配，保证任务调度的高效与线程安全。
void worker_manager::create(task_wz* t, worker_type typ)//ui线程
{

	if (workers.exist())                                // 1. 判断 worker 池是否已初始化（即有可用 worker）
	{
		if (t)                                          // 2. 判断传入的任务指针 t 是否有效
		{
			if (stack_idle_index.empty())               // 3. 如果空闲 worker 索引栈为空，说明没有可复用的 worker
			{
				workers.addr[cnt].create(t, typ);       // 4. 用当前计数 cnt 位置的 worker 创建新任务
				queue_worker.front(cnt);                // 5. 把该 worker 的索引放入队列头，等待调度
				cnt++;                                  // 6. worker 数量加一
                Semaphore.release(1);                   // 7. 信号量+1，唤醒等待的调度线程
			}
			else                                        // 8. 如果有空闲 worker 可复用
			{
				int index;
				stack_idle_index.pop(index);            // 9. 从空闲栈弹出一个可用 worker 的索引
				workers.addr[index].create(t, typ);     // 10. 用该 worker 创建新任务
				queue_worker.front(index);              // 11. 把该 worker 的索引放入队列头，等待调度
                Semaphore.release(1);                   // 12. 信号量+1，唤醒等待的调度线程
			}
		}
	}
	t = 0;                                              // 13. 置 t 为 0，防止外部误用（无实际内存释放，仅清空指针）
}

//实现了任务调度的核心流程，通过信号量和队列机制，安全、高效地分配和执行任务，并对正在运行的 worker 进行类型化管理，便于后续的资源回收和系统维护。
bool worker_manager::execute()                                          // type: task_manager/UI线程, 也可在task_manager线程中调用
{
	if (workers.exist())                                                // 1. 判断 worker 池是否已初始化（即有可用 worker）
	{
		Semaphore.acquire(1);                                           // 2. 等待信号量（信号量减1），如果为0则阻塞，直到有新任务可调度
		int index(0);                                                   // 3. 定义一个索引变量 index，初始为0
		queue_worker.rear(index);                                       // 4. 从 worker 队列尾部取出一个 worker 的索引（即将要执行的 worker）
		bool ret = workers.addr[index].execute();                       // 5. 调用该 worker 的 execute() 方法，执行任务，返回执行结果
		stack_running_type[workers.addr[index].task_typ()].push(index); // 6. 将该 worker 的索引按任务类型压入“正在运行”栈，便于后续管理和回收
		return ret;                                                     // 7. 返回任务执行结果（true/false）
	}
	return false;                                                       // 8. 如果 worker 池未初始化，直接返回 false
}

//实现了任务的等待和回收，通过栈结构管理正在运行的任务，确保任务能够被正确地 join 或 close，避免资源泄漏和线程悬挂。
void worker_manager::join(task_type typ)          //type:task_manager-UI线程, 也可在task_manager线程中调用
{
	if (workers.exist())                          // 1. 判断 worker 池是否已初始化（即有可用 worker）
	{
		while (!stack_running_type[typ].empty()) {// 2. 只要该类型的“正在运行”栈不为空，就循环处理
			int index;
			stack_running_type[typ].pop(index);   // 3. 从该类型的“正在运行”栈弹出一个 worker 索引
			workers.addr[index].join();           // 4. 调用该 worker 的 join() 方法，等待其任务完成并回收资源
			stack_idle_index.push(index);         // 5. 将该 worker 的索引压入空闲 worker 栈，便于后续复用
		}
	}
}

void worker_manager::close(task_type typ)          //task_end或task_manager调用
{
	if (workers.exist())                           // 1. 判断 worker 池是否已初始化（即有可用 worker）
	{
		while (!stack_running_type[typ].empty()) { // 2. 只要该类型的“正在运行”栈不为空，就循环处理
			int index;
			stack_running_type[typ].pop(index);    // 3. 从该类型的“正在运行”栈弹出一个 worker 的索引
			workers.addr[index].close();           // 4. 调用该 worker 的 close() 方法，安全关闭并回收该 worker 的任务资源
			stack_idle_index.push(index);          // 5. 将该 worker 的索引压入空闲 worker 栈，便于后续复用
		}
	}
}

//实现了指定类型任务的批量安全关闭，确保所有该类型的 worker 都被正确 close，资源被安全回收，并将 worker 重新放入空闲池，便于后续复用。
void worker_manager::clear()                    //ui线程调用，确保所有任务和线程已结束后释放内存
{
	if (cnt == 0)                               // 1. 只有当当前 worker 数量为 0 时才允许清理（确保没有任务在运行）
	{
		workers.clear();                        // 2. 清空 worker 池，释放所有 worker 对象的内存
		stack_idle_index.clear();               // 3. 清空空闲 worker 索引栈
		for (int i = 0; i < TASK_TYPE_NUM; i++) // 4. 遍历所有任务类型
		{
			stack_running_type[i].clear();      // 5. 清空每种任务类型对应的“正在运行”worker索引栈
		}
	}
}

void worker_manager::task_callback_func(TOOLWZ::data_wz<task_wz, 20, task_destruct>* t)
{
	t->addr->init();							 // 1. 调用任务对象的 init() 方法，进行任务初始化
	t->addr->run();								// 2. 调用任务对象的 run() 方法，执行任务的主要逻辑
	t = 0;										// 3. 将指针 t 置为 0（仅清空本地指针，不释放内存，防止误用）
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
	while (1)                           // 1. 无限循环，持续调度任务
	{
		if (!worker_manager::execute()) // 2. 调用 worker_manager::execute() 执行一个任务，如果返回 false（无任务可执行）
		{
			break;                      // 3. 跳出循环，结束任务管理器的 run
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
	while (!stack_join_type.empty()) // 1. 只要 join 类型栈不为空
	{
		task_type typ;
		stack_join_type.pop(typ);    // 2. 弹出一个任务类型
		worker_manager::join(typ);   // 3. 等待该类型的所有任务完成（join）
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
	while (!stack_end_type.empty()) // 1. 只要 end 类型栈不为空
	{
		task_type typ;
		stack_end_type.pop(typ);    // 2. 弹出一个任务类型
		worker_manager::close(typ); // 3. 关闭该类型的所有任务（close）
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
	OPEN_TASK_RCV_DBG(LogTaskRcv.txt);                               // 1. 打开接收任务的调试日志文件

#ifndef TEST_WITHOUT_BOARD                                           // 2. 如果没有定义 TEST_WITHOUT_BOARD（即实际硬件环境）
	while (Loop)                                                     // 3. 只要 Loop 为 true，就持续循环接收数据
	{
		RcvData Data(800);                                           // 4. 创建一个长度为800字节的接收数据对象 Data
		int res = SOCKWZ::SockGlob::Recv(Data.GetData().get(), 800); // 5. 从全局Socket接收数据，最多接收800字节，返回实际接收长度res

		if (res == SOCKET_ERROR)                                     // 6. 如果接收出错
		{
			if (Loop == true)                                        // 7. 如果Loop仍为true（任务未被要求关闭）
			{
                                                                     // WRITE_TASK_RCV_DBG("Socket Error Continue\n");
				continue;                                            // 8. 继续下一次循环，尝试重新接收
			}
			else                                                     // 9. 如果Loop为false（任务被要求关闭）
			{
				WRITE_TASK_RCV_DBG("Socket Error Break\n");          // 10. 记录调试日志
				Data.Clear();                                        // 11. 清理Data对象
				break;                                               // 12. 跳出循环，结束任务
			}
		}
		else                                                         // 13. 如果接收成功
		{
			WRITE_TASK_RCV_DBG("Data recived, Len = %d\n", res);     // 14. 记录接收到的数据长度
		}
		Data.SetLen(res);                                            // 15. 设置Data对象的实际数据长度
		QueueRcv.front(Data);                                        // 16. 将Data对象放入接收队列的队首，供后续任务处理
		Data.Clear();                                                // 17. 清理Data对象，释放内存
	}
                                                                     // 18. 循环结束后，日志关闭在函数结尾
	WRITE_TASK_RCV_DBG("Cycle Over Loop = %d", Loop);
#else
	int sec(0);                                                      // 1. 定义一个整型变量 sec，初始为0，用于模拟时间戳
	while (Loop)                                                     // 2. 只要 Loop 为 true，就持续循环（模拟数据接收）
	{
		std::this_thread::sleep_for(std::chrono::microseconds(100)); // 3. 每次循环延时100微秒，模拟数据接收间隔
		RcvData Data(800);                                           // 4. 创建一个长度为800字节的 RcvData 对象 Data

                                                                     // 5. 设置数据包头部各字段，模拟一个0x28类型的数据包
		FRAME_HEAD_STAR(Data.GetData().get())->msgID = 0x28;
		FRAME_HEAD_STAR(Data.GetData().get())->rev = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->ctlFractstop = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->ctlFractStrt = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->ctlFractMode = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->reserved_base = 0;
		FRAME_HEAD_STAR(Data.GetData().get())->siglen = 784;

                                                                     // 6. 设置数据包体的各字段，模拟芯片状态信息
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->timeStamp_8ms = sec;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->free_codec_dac = 2048;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->mnic_online = 0xFFFFFFFF;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->ds18b20_16b = 50;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->fpga_heat = 60;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->dbg_dn_aic = 0;
		PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->cnt_dn_aic = 0;

                                                                     // 7. 填充每个芯片的ADC信息，8组，每组12个Info
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 12; j++)
			{
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].sinad = 33;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].reserved = 0;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].freqInHz = 64000;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].vpp = 20;
				PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j].rms = 30;
			}
		}

		Data.SetLen(788);                                            // 8. 设置Data对象的实际数据长度为788字节（模拟有效数据长度）
		QueueRcv.front(Data);                                        // 9. 将Data对象放入接收队列的队首，供后续任务处理
		Data.Clear();                                                // 10. 清理Data对象，释放内存
	}
#endif

	CLOSE_TASK_RCV_DBG();                                            // 11. 关闭调试日志
}

void task_rcv::close()
{
	WRITE_TASK_RCV_DBG("task_rcv::close\n"); // 1. 写调试日志，记录关闭操作
	Loop = false;                           // 2. 设置 Loop 为 false，通知 run() 循环安全退出
}

void task_rcv::clear()
{
	QueueRcv.clear(); // 1. 清空接收数据队列，释放所有队列内的 RcvData 对象
}



TOOLWZ::queue<RcvData, 500, RcvDataDestruct> task_rcv::QueueRcv;

//task_dispatch

task_dispatch::task_dispatch() :task_wz(TASK_DISPATCH) {}

task_dispatch::~task_dispatch() {}

void task_dispatch::init()
{
	Loop = true; // 1. 初始化时将 Loop 设为 true，表示任务可以正常运行
}

void task_dispatch::run()
{
	OPEN_TASK_DISPATCH_DBG(LogTaskDispatch.txt);                      // 打开调试日志
	while(Loop)                                                       // 只要 Loop 为 true，就一直循环
	{
		RcvData Data;
		task_rcv::QueueRcv.rear(Data);                                // 从接收队列取出一个数据包（尾部出队）
		if(Data.GetData().get())                                      // 如果数据有效
		{
			switch (FRAME_HEAD_STAR(Data.GetData().get())->msgID)     // 读取数据包头部的 msgID 字段，判断数据类型
			{
			case 0x28:
				TaskChipStatParsing::QueueChipStatParsing.front(Data);// 如果是 0x28 类型的数据包，放入芯片状态解析队列
				break;
			case 0xa0:
				TaskVersionParsing::QueueVeriosnParsing.front(Data);  // 如果是 0xa0 类型的数据包，放入版本号解析队列
			default:                                                  // 其他类型的数据包不处理
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
		QueueChipStatParsing.rear(Data);// 从队列尾部取出一个数据包
		if(Data.GetData().get())
		{
			if(FRAME_HEAD_STAR(Data.GetData().get())->msgID == 0x28)// 判断数据包类型是否为0x28（芯片状态包）
			{
				FCT::FluidCtrlGlob->FluidStore(0, PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->free_codec_dac);// 更新流控缓存
				DCR::DeviceCheckResultGlobal->SetTemperatureInner(PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->fpga_heat); // 更新芯片内部温度
				DCR::DeviceCheckResultGlobal->SetTemperatureEnv(PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->ds18b20_16b);// 更新环境温度
				DCR::DeviceCheckResultGlobal->SetUpPackCount(PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->timeStamp_8ms); // 更新上行包计数

				// 如果启用包日志记录，写入一条记录
				if(bPackLogRecord)
				{
					WRITE_LOG_UP_PACK_RECORD(DCR::DeviceCheckResultGlobal->GetCheckCompletedCount(), DCR::DeviceCheckResultGlobal->GetUpPackCount());
				}
				// 遍历所有板卡和芯片
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 4; j++)
					{
						// 获取在线状态
						unsigned int IfOnline = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->mnic_online;
						bool ChipIfOnline = DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline();
						// 更新在线状态
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetIfOnline(GET_BIT_U32(IfOnline, i * 4 + j));
						// 如果当前芯片在线
						if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline())
						{
							// 如果之前不在线，在线芯片数加一
							if(!ChipIfOnline)
							{
								DCR::DeviceCheckResultGlobal->ChipOnlineNumInc();
							}
							// 三组通道的检测结果
							bool Res[3]{ true, true, true };
							unsigned short Sinad[3]{ 0 }, Vpp[3]{ 0 }, Rms[3]{ 0 };
							// 第一组通道参数
							Sinad[0] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2].sinad;
							Vpp[0] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2].vpp;
							Rms[0] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2].rms;
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateRangeSINAD(Sinad[0]);
							Res[0] &= DCR::DeviceCheckResultGlobal->GetCondition()[0].CheckAll(Sinad[0], Vpp[0], Rms[0]);
							// 第二组通道参数
							Sinad[1] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2 + 1].sinad;
							Vpp[1] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2 + 1].vpp;
							Rms[1] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[j * 2 + 1].rms;
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateRangeSINAD(Sinad[1]);
							Res[1] &= DCR::DeviceCheckResultGlobal->GetCondition()[0].CheckAll(Sinad[1], Vpp[1], Rms[1]);
							// 第三组通道参数
							Sinad[2] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[8 + j].sinad;
							Vpp[2] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[8 + j].vpp;
							Rms[2] = PTR_UP_MNIC_STA(Data.GetData().get() + HEAD_DATA_LEN)->MNICx4_ADC_INFO_ARR[i].Info[8 + j].rms;
							Res[2] &= DCR::DeviceCheckResultGlobal->GetCondition()[1].CheckAll(Sinad[2], Vpp[2], Rms[2]);
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).CheckPacksOfMifInc(Res[0] & Res[1] & Res[2]);

							// 如果启用芯片日志记录，写入详细记录
							if(bPackLogRecord)
							{
								WRITE_LOG_UP_CHIP_RECORD(i + 1, j + 1,
									TCOND::TestCondition::SinadTransfer(Sinad[0]), Vpp[0], Rms[0], Res[0] ? " TRUE" : "FALSE",
									TCOND::TestCondition::SinadTransfer(Sinad[1]), Vpp[1], Rms[1], Res[1] ? " TRUE" : "FALSE",
									TCOND::TestCondition::SinadTransfer(Sinad[2]), Vpp[2], Rms[2], Res[2] ? " TRUE" : "FALSE");
							}
						}
						else// 当前芯片不在线
						{
							// 如果之前在线，在线芯片数减一
							if(ChipIfOnline)
							{
								// 连续有效包数清零
								DCR::DeviceCheckResultGlobal->ChipOnlineNumDec();
							}
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetCheckPacksOfMif(0);
						}
					}
				}
			}
		}
		Data.Clear();// 清理数据，释放内存
	}
	CLOSE_TASK_0X28_PARSING();// 关闭调试日志
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

// 构造函数，接收两个参数：测试状态 s 和统计模式 m，分别赋值给成员变量 Stat 和 Mode
MsgToCentralWt::MsgToCentralWt(TestStat s, StatisticMode m)
	:Stat(s), Mode(m)
{}

// 拷贝构造函数，用另一个 MsgToCentralWt 对象 cp 初始化本对象
// Stat 通过 cp.GetStat() 获取，Mode 通过 cp.GetMode() 获取
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
	OPEN_TASK_STATISTICS_DBG(LogTaskStatistics.txt);// 打开调试日志
	WRITE_TASK_STATISTICS_DBG("TaskTestStatistics::run()\n");// 写入调试日志
	while (Loop)
	{
		StatisticMode Mode(StatisticMode::MODE_DEFAULT);// 定义统计模式变量，初始为 MODE_DEFAULT
		QueueStatistcsMODE.rear(Mode);// 从统计模式队列取出一个统计模式
		switch (Mode)// 根据不同的统计模式进行处理
		{
		case MODE_DEFAULT:
			// 默认模式，不做任何处理
			break;
		case STATISTICS_A_MIF:
			WRITE_TASK_STATISTICS_DBG("Mode :STATISTICS_A_MIF\n");
			// 遍历所有板卡和芯片
			for(int i = 0; i < 8; i ++)
			{
				for(int j = 0; j < 4; j++)
				{
					// 判断该芯片的连续有效包数是否小于要求
					if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckPacksOfMif() < ContinousValidNum)
					{
						WRITE_TASK_STATISTICS_DBG("Check Result A Mif: False\n");
						// 如果之前检测结果为 true，需要更新统计
						if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckResult())
						{
							DCR::DeviceCheckResultGlobal->ChipSatisfiedNumDec();// 满足芯片数减一
							if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline())
							{
								// 如果芯片在线，不满足芯片数加一
								DCR::DeviceCheckResultGlobal->ChipUnSatisfiedNumInc();
							}
						}
						// 更新检测结果为 false
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateCheckResult(false);
					}
					else
					{
						WRITE_TASK_STATISTICS_DBG("Check Result A Mif: True\n");
						// 检测结果为 true
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).UpdateCheckResult(true);
					}
				}
			}
			// 统计完成后，发出统计结果信号
			emit MsgOfStatistics(MsgToCentralWt(TestStat::TEST_RUNNING, StatisticMode::STATISTICS_A_MIF));
			break;
		case STATISTICS_A_TIME:
			WRITE_TASK_STATISTICS_DBG("Mode :STATISTICS_A_TIME\n");
			// 遍历所有板卡和芯片
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					// 如果检测结果为 true，累计满足次数
					if(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckResult())
					{
						WRITE_TASK_STATISTICS_DBG("A TIME Res True\n");
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).CheckSatisfiedCountInc();
					}
				}
			}
			// 统计完成后，发出统计结果信号
			emit MsgOfStatistics(MsgToCentralWt(TestStat::TEST_RUNNING, StatisticMode::STATISTICS_A_TIME));
			break;
		default:
			// 其他未知模式，不做处理
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

void TaskDataConstruct::run()                                                                               // 任务主循环，处理文件链表中的每个节点
{
	OPEN_TASK_CONSTRUCT_DBG(LogTaskConstruct.txt);
	for(auto it = FileList.GetHead(); it != nullptr; it = it->GetNext())                                    // 遍历文件链表的每个节点
	{
		switch(it->GetOpt())                                                                                // 根据节点的操作类型进行处理
		{
		case FLST::FileOPT::ADD_FILE:
			WRITE_TASK_CONSTRUCT_DBG("AddNode(), Path:%s\n", it->GetData().GetPath().toStdString().c_str());// 日志记录：添加节点
			DCWZ::DataMana::DataListGlobal.AddNode(it->GetData());                                          // 向全局数据链表添加节点
			WRITE_TASK_CONSTRUCT_DBG("AddNode Complete\n");                                                 // 日志记录：添加完成
			break;
		case FLST::FileOPT::DEL_FIEL:                                                                       // 从全局数据链表删除节点
			DCWZ::DataMana::DataListGlobal.DelNode(it->GetData());
			break;
		default:                                                                                            // 其他操作类型不处理
			break;
		}
	}
	FileList.Clear();                                                                                       // 清空本地文件链表
	CLOSE_TASK_CONSTRUCT_DBG();                                                                             // 关闭调试日志
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

//
void TaskDataConstructARG::init(){}

void TaskDataConstructARG::run()
{
	OPEN_TASK_CONSTRUCT_ARG_DBG(LogTaskConstructArg.txt);
	// 遍历参数链表 ArgList 的每个节点
	for(auto it = ArgList.GetHead(); it != nullptr; it = it->GetNext())
	{
		switch (it->GetOpt())// 根据节点的操作类型进行处理
		{
		case FLST::FileOPT::ADD_FILE:
			// 向全局数据链表添加节点
			DCWZ::DataMana::DataListGlobal.AddNode(it->GetData());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run(), ADD_FILE\n");
			break;
		case FLST::FileOPT::DEL_FIEL:
			// 从全局数据链表删除节点
			DCWZ::DataMana::DataListGlobal.DelNode(it->GetData());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run(), DEL_FILE\n");
			break;
		case FLST::FileOPT::EXCHANGE:
			// 交换全局数据链表中的两个节点
			DCWZ::DataMana::DataListGlobal.Exchange(it->GetData(), it->GetExchg());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run(), EXCHANGE\n");
			break;
		case FLST::FileOPT::TO_BOTTOM:
			// 将指定节点移动到全局数据链表底部
			DCWZ::DataMana::DataListGlobal.ToBottom(it->GetData());
			WRITE_TASK_CONSTRUCT_ARG_DBG("run, TO_BOTTOM\n");
			break;
		default:
			// 其他操作类型不处理
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
				//if (i == 0 && !ReadyToSendFirstPack)//新增：首包未准备好不发送
				//	continue; //新增： 未收到参数A0回包，不发首包
				if(FCT::FluidCtrlGlob->FluidCheckUpdate(0, Pack.GetSegAll().GetLen(), FLUID_SIZE_INDEX0 / 4) == FCT::FluidCheckRes::FLUID_SATISFY)
				{
					// WRITE_TASK_DATA_SEND_DBG("Fluid Val = %lld, Send Data Len = %lld\n", FCT::FluidCtrlGlob->FluidLoad(0), Pack.GetSegAll().GetLen());
#ifndef TEST_WITHOUT_BOARD
					SOCKWZ::SockGlob::Send(Pack.GetPackData(), Pack.GetSegAll().GetLen());
					// WRITE_TASK_DATA_SEND_INFO_VERIFY(Pack.GetPackData(), Pack.GetSegAll().GetLen());
					// WRITE_TASK_DATA_SEND_DBG("Send Pack\n");
					//if (i == 0)                       //新增：首包发送后清除标志
					//	ReadyToSendFirstPack = false; //新增 首包发出后清除标志
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
	while(Loop)																		//只要 Loop 为 true，就循环执行。
	{
		SOCKWZ::SockGlob::Send(DataDown.GetData().get(), DataDown.GetLen());		//发送下行数据包，请求获取版本号（0xa0包）
		for(int i = 0; i < 100; i++)												//最多尝试100次从版本号解析队列 QueueVeriosnParsing 取出上行数据包（DataUp）
		{
			QueueVeriosnParsing.rear(DataUp);
			if(DataUp.GetData())
			{
				WRITE_TASK_VERSION_GET_DBG("ram_id = %d, chk_fail = %d, cmd_ack = %d\n",
					PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->ram_id,
					PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->chk_fail,
					PTR_ARG_UP_A0(DataUp.GetData().get() + HEAD_DATA_LEN)->cmd_ack);		//如果成功取到数据，打印调试信息，显示 ram_id、chk_fail、cmd_ack 字段的值。
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