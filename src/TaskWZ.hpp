#ifndef _TASKWZ_HPP_
#define _TASKWZ_HPP_

#include <set>
#include <thread>
#include <QSemaphore>
#include <QObject>
#include "FileList.hpp"
#include "SocketWZ.hpp"
#include "DeviceCheckResult.hpp"
#include "FluidCtrl.hpp"
#include "ConfigInit.hpp"
#include "LogProcess.hpp"

#define MAX_TASK_NUM 50

namespace TASKWZ{

enum worker_type {
	ON_IDLE,
	EXECUTE_THREAD,
	EXECUTE_NO_THREAD,
	JOIN_TASK_BY_TYPE,
	END_TASK_BY_TYPE,
	END_ALL_TASK,
};

enum task_type {
	TASK_EMPTY = 0,
	TASK_MANAGER,
	TASK_JOIN,
	TASK_END,
	TASK_RCV,
	TASK_DISPATCH,
	TASK_CHIP_STAT_PARSING,
	TASK_RESULT_STATISTICS,
    TASK_DATA_CONSTRUCT, 
	TASK_DATA_SEND,
	TASK_VERSION_PARSING,
	TASK_GENERAL,
};

#define TASK_TYPE_NUM 12

enum message_type {

};

class task_wz :public QObject {
	Q_OBJECT
public:
	task_type type;
public:
	task_wz();
	task_wz(task_type t);
	~task_wz();

	virtual void init() = 0;
	virtual void run() = 0;

	virtual void close() = 0;
	virtual void clear() = 0;
};

void task_destruct(task_wz& t);

class worker {
	TOOLWZ::data_wz<task_wz, 20, task_destruct> task;
	std::thread th;
public:
	worker_type type;
public:
	worker();
	~worker();

	void create(task_wz* t, worker_type typ);
	bool execute();
	task_type task_typ();
	void join();
	void close();
	void clear();
};

void worker_destruct(worker& w);

class worker_manager {
private:
	static TOOLWZ::data_wz<worker, sizeof(worker), worker_destruct> workers;
	static int cnt;
	static TOOLWZ::queue<int> queue_worker;
	static TOOLWZ::stack_wz<int, MAX_TASK_NUM> stack_idle_index;
	static TOOLWZ::stack_wz<int, MAX_TASK_NUM> stack_running_type[TASK_TYPE_NUM];
	static std::mutex mtx;
    static QSemaphore Semaphore;
public:
	worker_manager();
	~worker_manager();

	static void init(int num);
	static void begin_task_system();
	static void end_task_system();
	static void create(task_wz* t, worker_type typ);
	static bool execute();
	static void join(task_type typ);
	static void close(task_type typ);
	static void clear();

	static void task_callback_func(TOOLWZ::data_wz<task_wz, 20, task_destruct>* t);
};

//

class task_manager :public task_wz {
public:
	task_manager();
	~task_manager();

	void init();
	void run();
	void close();
	void clear();
};

class task_join:public task_wz {
	TOOLWZ::stack_wz<task_type> stack_join_type;
public:
	task_join(TOOLWZ::stack_wz<task_type> stack_join);
	~task_join();

	void init();
	void run();
	void close();
	void clear();
};

#define CREATE_TASK_JOIN(arg) TASKWZ::worker_manager::create(new TASKWZ::task_join(arg), TASKWZ::worker_type::JOIN_TASK_BY_TYPE); arg.clear();

//

class task_end :public task_wz {
	TOOLWZ::stack_wz<task_type> stack_end_type;
public:
	task_end(TOOLWZ::stack_wz<task_type> stack_end);
	~task_end();

	void init();
	void run();
	void close();
	void clear();
};

#define CREATE_TASK_END(arg)	TASKWZ::worker_manager::create(new TASKWZ::task_end(arg), TASKWZ::worker_type::END_TASK_BY_TYPE);

//

class RcvData{
	std::shared_ptr<char[]> Data;
	int Len;
public:
	RcvData();
	RcvData(int len);
	RcvData(const RcvData& cp);

	void Clear();
	void operator=(const RcvData& as);
	const std::shared_ptr<char[]>& GetData() const;
	void SetLen(int len);
	int GetLen() const;
};

void RcvDataDestruct(RcvData& d);

class task_rcv :public task_wz {
public:
	task_rcv();
	~task_rcv();

	void init();
	void run();
	void close();
	void clear();

	bool Loop;
	static TOOLWZ::queue<RcvData, 500, RcvDataDestruct> QueueRcv;
};

#define CREATE_TASK_RCV		TASKWZ::worker_manager::create(new TASKWZ::task_rcv(), TASKWZ::worker_type::EXECUTE_THREAD);

//

class task_dispatch :public task_wz {
public:
	task_dispatch();
	~task_dispatch();

	void init();
	void run();
	void close();
	void clear();

	bool Loop;
};

#define CREATE_TASK_DISPATCH	TASKWZ::worker_manager::create(new TASKWZ::task_dispatch(), TASKWZ::worker_type::EXECUTE_THREAD);

//上报芯片状态包解析

class TaskChipStatParsing:public task_wz{
public:
	TaskChipStatParsing();
	~TaskChipStatParsing();

	void init();
	void run();
	void close();
	void clear();

	bool Loop;
	static TOOLWZ::queue<RcvData, 500, RcvDataDestruct> QueueChipStatParsing;
	static bool bPackLogRecord;//是否记录log
};

#define CREATE_TASK_MNIC_PARSING	TASKWZ::worker_manager::create(new TASKWZ::TaskChipStatParsing(), TASKWZ::worker_type::EXECUTE_THREAD);

//测试结果统计

enum StatisticMode{
	MODE_DEFAULT,
	STATISTICS_A_MIF,
	STATISTICS_A_TIME,
};

enum TestStat{
	TEST_START,
	TEST_RUNNING,
	TEST_OVER,
};

class MsgToCentralWt{
	TestStat Stat;
	StatisticMode Mode;

public:
	MsgToCentralWt();
	MsgToCentralWt(TestStat, StatisticMode);
	MsgToCentralWt(const MsgToCentralWt& cp);

	TestStat GetStat() const;
	StatisticMode GetMode() const;
};

class TaskTestStatistics: public task_wz{
	Q_OBJECT;
public:
	TaskTestStatistics();
	~TaskTestStatistics();

	void init();
	void run();
	void close();
	void clear();

signals:
	void MsgOfStatistics(MsgToCentralWt m);
public:
	bool Loop;
	static long long ContinousValidNum;//连续有效包数条件
	static TOOLWZ::queue<StatisticMode> QueueStatistcsMODE;
};

//数据解析构造任务,参数是mif文件

class TaskDataConstruct :public task_wz {
	FLST::FileList<FPWZ::ArgDM> FileList;
public:
	TaskDataConstruct(FLST::FileList<FPWZ::ArgDM>& FileList);
	~TaskDataConstruct();

	void init();
	void run();
	void close();
	void clear();
};

#define CREATE_TASK_DATA_CONSTRUCT(arg) TASKWZ::worker_manager::create(new TASKWZ::TaskDataConstruct(arg), TASKWZ::worker_type::EXECUTE_THREAD);

//参数链表数据生成

class TaskDataConstructARG: public task_wz{
	FLST::FileList<DCWZ::ARG_RTC_GENERATE> ArgList;
public:
	TaskDataConstructARG(FLST::FileList<DCWZ::ARG_RTC_GENERATE>& ArgL);
	~TaskDataConstructARG();

	void init();
	void run();
	void close();
	void clear();
};

#define CREATE_TASK_DATA_CONSTRUCT_ARG(arg) TASKWZ::worker_manager::create(new TASKWZ::TaskDataConstructARG(arg), TASKWZ::worker_type::EXECUTE_THREAD);

//数据发送

class TaskDataSend: public task_wz{
	Q_OBJECT
public:
	TaskDataSend(long long cnt);
	~TaskDataSend();

	void init();
	void run();
	void close();
	void clear();

	bool Loop;
	long long TestCount;//测试次数
	static QSemaphore SemaWaitForUI;
signals:
	void MsgOfStartEnd(MsgToCentralWt m);
};

#define CREATE_TASK_DATA_SEND(arg) TASKWZ::worker_manager::create(new TASKWZ::TaskDataSend(arg), TASKWZ::worker_type::EXECUTE_THREAD);

//版本号解析

class TaskVersionParsing: public task_wz{
	Q_OBJECT
public:
	TaskVersionParsing();
	~TaskVersionParsing();

	void init();
	void run();
	void close();
	void clear();

signals:
	void UpdateBoardVersion();
public:
	bool Loop;
	static TOOLWZ::queue<RcvData, 500, RcvDataDestruct> QueueVeriosnParsing;
	static DF::VERSION_UP Version;
};

};

#endif //_TASKWZ_HPP_