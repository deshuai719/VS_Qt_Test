#ifndef _TOOLWZ_HPP_
#define _TOOLWZ_HPP_

#include <mutex>
#include <condition_variable>
#include <QDebug>
#include "LogWZ.hpp"

#define N_BITS_HIGH(n)									((0x1LL << (n)) - 1)
#define RIGHT_SHIFT_U8(data, n)    (unsigned char)(((data) >> (n)) & N_BITS_HIGH(8))
#define NEGATION_BITS(v)								~(v)

namespace TOOLWZ{
    unsigned long long DecToHexBigInteger(unsigned char* dec, unsigned long long len, unsigned char* hex);
    unsigned char CharToHex(unsigned char c);

	//累加校验计算
	uint8_t AccVerify(char* p, int n);
    //

    template<typename T>
    void default_destruct(T& v)
    {}

    template<typename T, int cells = 0x100, void(*destruct_func)(T&) = default_destruct<T>>
    class queue {
    public:
    	T* _que;
    	unsigned int _q_sz, _q_cell_num, _q_cell_sz, _rear, _front;
    	std::mutex mtx; 
        std::condition_variable cv;
        bool exit_flag = false; // 新增：退出标志
    public:
    	queue(unsigned int cell_num);
    	queue();
    	~queue();

    	bool empty();
    	bool rear(T& get);
        bool try_rear(T& get);
    	bool rear_with_destruct(T& get);
    	bool front(T& 
        );
    	bool get_with_overflow(T& get);
    	void add_even_full(T& add);
    	bool get_with_lock(T& get);
    	bool add_with_lock(T& add);
    	void rear_no_pop(T& get);
    	void pop();
    	void clear();
    	void release_memory();
        void exit(); // 建议补充声明
    };

    template<typename T, int cells, void(*destruct_func)(T&)>
    queue<T, cells, destruct_func>::queue(unsigned int cell_num)
    	:_que(0), _q_sz(cell_num * sizeof(T)), _q_cell_num(cell_num), _q_cell_sz(sizeof(T)), _rear(0), _front(0)
    {
    	_que = new T[_q_cell_num];
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    queue<T, cells, destruct_func>::queue()
    	:_que(0), _q_sz(cells * sizeof(T)), _q_cell_num(cells), _q_cell_sz(sizeof(T)), _rear(0), _front(0)
    {
    	_que = new T[cells];
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    queue<T, cells, destruct_func>::~queue()
    {
    	if (_que)
    	{
    		delete[] _que;
    		_que = 0;
    	}
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>::empty()//返回false为非空
    {
    	if (_rear == _front)
    		return true;
    	return false;
    }


    /******************************新增：阻塞出队**************************************/
    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>::rear(T& get)//新增：阻塞出队
    {

        //static std::chrono::steady_clock::time_point lastCallTime;
        //static bool firstCall = true;
        //auto now = std::chrono::steady_clock::now();
        //if (!firstCall) {
        //    auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCallTime).count();
        //    // 建议用统一日志宏，便于分析
        //    WRITE_TASK_DATA_SEND_DBG("rear 两次调用间隔(ms): %lld\n", interval);
        //}
        //else {
        //    firstCall = false;
        //}
        //lastCallTime = now;

        //qDebug() << "[queue] rear wait, front=" << _front << " rear=" << _rear;
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return _rear != _front || exit_flag; }); // 队列非空才继续,增加退出条件
        if (exit_flag && _rear == _front) return false; // 退出且队列空，直接返回
        get = _que[_rear];
        _rear = (_rear + 1) % _q_cell_num;
        return true;
    }

    ////无阻塞出队
    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>:: try_rear(T& get)//函数内部赋值
    {
    	// std::unique_lock<std::mutex> lock(mtx);
    	if (_rear == _front)
    	{
    		// lock.unlock();
    		return false;
    	}
    	get = _que[_rear];
    	_rear = (_rear + 1) % _q_cell_num;
    	// lock.unlock();
    	return true;
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>::rear_with_destruct(T& get)
    {
    	// std::unique_lock<std::mutex> lock(mtx);
    	if (_rear == _front)
    	{
    		// lock.unlock();
    		return false;
    	}
    	get = _que[_rear];
    	destruct_func(_que[_rear]);
    	_rear = (_rear + 1) % _q_cell_num;
    	// lock.unlock();
    	return true;
    }

    /*****************************新增：阻塞入队*****************************************/
    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>::front(T& add)//新增：阻塞入队
    {
        //std::unique_lock<std::mutex> lock(mtx);
        //qDebug() << "[queue] front notify_one, front=" << _front << " rear=" << _rear;
        if ((_front + 1) % _q_cell_num == _rear)
            return false;
        _que[_front] = add;
        _front = (_front + 1) % _q_cell_num;
        cv.notify_one(); // 唤醒阻塞的出队线程
        return true;
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    void queue<T, cells, destruct_func>::exit()
    {
        //qDebug() << "[queue] exit notify_all, exit_flag=" << exit_flag;
        {
            std::lock_guard<std::mutex> lock(mtx);
            exit_flag = true;
        }
        cv.notify_all();
    }

    //template<typename T, int cells, void(*destruct_func)(T&)>
    //bool queue<T, cells, destruct_func>::front(T& add)//函数内部赋值
    //{
    //	// std::unique_lock<std::mutex> lock(mtx);
    //	if ((_front + 1) % _q_cell_num == _rear)
    //	{
    //		// lock.unlock();
    //		return false;
    //	}
    //	_que[_front] = add;
    //	_front = (_front + 1) % _q_cell_num;
    //	// lock.unlock();
    //	return true;
    //}

    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>::get_with_overflow(T& get)
    {
    	std::unique_lock<std::mutex> lock(mtx);
    	if (_rear == _front)
    	{
    		lock.unlock();
    		return false;
    	}
    	get = _que[_rear];
    	_rear = (_rear + 1) % _q_cell_num;
    	lock.unlock();
    	return true;
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    void queue<T, cells, destruct_func>::add_even_full(T& add)
    {
    	_que[_front] = add;
    	std::unique_lock<std::mutex> lock(mtx);
    	_front = (_front + 1) % _q_cell_num;//在front加1前，队列被取空，此时rear == front，不能再取
    	if (_front == _rear)//在判断前由于front + 1,rear != front，队列非空，此时再取一次，取出了此次add操作的数据，队列又为空，rear == front
    	{
    		destruct_func(_que[_rear]);
    		_rear = (_rear + 1) % _q_cell_num;//若前述成立，则执行该步，此时rear != front队列非空，但队列实际并无新的add操作被执行
    	}										//综上，此操作要加锁
    	lock.unlock();
    	return;
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>::get_with_lock(T& get)
    {
    	std::unique_lock<std::mutex> lock(mtx);
    	if (_rear == _front)
    	{
    		lock.unlock();
    		return;
    	}
    	get = _que[_rear];
    	destruct_func(_que[_rear]);
    	_rear = (_rear + 1) % _q_cell_num;
    	lock.unlock();
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    bool queue<T, cells, destruct_func>::add_with_lock(T& add)
    {
    	std::unique_lock<std::mutex> lock(mtx);
    	if ((_front + 1) % _q_cell_num == _rear)
    	{
    		lock.unlock();
    		return false;
    	}
    	_que[_front] = add;
    	_front = (_front + 1) % _q_cell_num;
    	lock.unlock();
    	return true;
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    void queue<T, cells, destruct_func>::rear_no_pop(T& get)
    {
    	if (_rear == _front)
    		return;
    	get = _que[_rear];
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    void queue<T, cells, destruct_func>::pop()
    {
    	// std::unique_lock<std::mutex> lock(mtx);
    	if (_rear == _front)
    	{
    		// lock.unlock();
    		return;
    	}
    	_rear = (_rear + 1) % _q_cell_num;
    	// lock.unlock();
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    void queue<T, cells, destruct_func>::clear()
    {
    	// std::unique_lock<std::mutex> lock(mtx);
    	// for(int i = _rear; i != _front; i = (i + 1) % _q_cell_num)
    	// {
    	//     destruct_func(_que[i]);
    	// }
    	// _rear = 0;
    	// _front = 0;
    	// lock.unlock();

    /*
     *	在没有加锁的情况下，需要在add线程和get线程分别调用一次，防止在判断通过后进行置0的操作
    */

    	for (int i = 0; i < _q_cell_num; i++)
    	{
    		destruct_func(_que[i]);
    	}
    	_rear = 0;
    	_front = 0;
		exit_flag = false;//新增：退出标志复位
    }

    template<typename T, int cells, void(*destruct_func)(T&)>
    void queue<T, cells, destruct_func>::release_memory()
    {
    	if (_que)
    	{
    		for (int i = 0; i < _q_cell_num; i++)
    		{
    			destruct_func(_que[i]);
    		}
    		if (_q_cell_num > 1)
    		{
    			delete[] _que;
    		}
    		else
    		{
    			delete _que;
    		}
    		_que = 0;
    		_q_sz = 0;
    		_q_cell_num = 0;
    		_rear = 0;
    		_front = 0;
			exit_flag = false;// 新增：退出标志复位
    	}
    }

    //
    template<typename T>
    void data_wz_default_destruct(T& v)
    {
    }

    template<typename T, int sz, void(*delete_func)(T&) = data_wz_default_destruct<T>>
    class data_wz {
    public:
    	T* addr;
    	int buf_len;//内存字节长度
    	int data_len;//有效数据单元数目

    	data_wz();
    	data_wz(int n);
    	data_wz(T** p, int num);
    	data_wz(data_wz<T, sz, delete_func>& cp);
    	~data_wz();
    	void set(T* p, int num);
    	T* get() const;
    	inline int& lenth();
    	int lenth() const;
    	T* operator->() const;
    	void operator=(data_wz<T, sz, delete_func>& as);
    	T& operator[](int index);
    	T& operator[](long long index);
    	T& operator[](unsigned long long index);
    	const T& operator[](int index) const;
    	const T& operator[](long long index) const;
    	const T& operator[](unsigned long long index) const;
    	bool exist();
    	void clear();
    };

    template<typename T, int sz, void(*delete_func)(T&)>
    data_wz<T, sz, delete_func>::data_wz() :addr(0), buf_len(0), data_len(0) {}

    template<typename T, int sz, void(*delete_func)(T&)>
    data_wz<T, sz, delete_func>::data_wz(int n) : addr((T*)0), buf_len(n* sz), data_len(n)
    {
    	if (buf_len > 0)
    	{
    		addr = new T[n];
    	}
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    data_wz<T, sz, delete_func>::data_wz(T** p, int num) :addr(*p), buf_len(num* sz), data_len(num)
    {
    	*p = 0;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    data_wz<T, sz, delete_func>::data_wz(data_wz<T, sz, delete_func>& cp) :addr(cp.addr), buf_len(cp.buf_len), data_len(cp.data_len)
    {
    	cp.addr = 0;
    	cp.buf_len = 0;
    	cp.data_len = 0;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    data_wz<T, sz, delete_func>::~data_wz()
    {
    	clear();
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    void data_wz<T, sz, delete_func>::set(T* p, int num)
    {
    	clear();
    	if (num > 0)
    	{
    		addr = p;
    		buf_len = num * sz;
    		data_len = num;
    	}
    	else
    	{
    		addr = 0;
    		buf_len = 0;
    		data_len = 0;
    	}
    	p = 0;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    T* data_wz<T, sz, delete_func>::get() const
    {
    	return addr;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    int& data_wz<T, sz, delete_func>::lenth()
    {
    	return data_len;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    int data_wz<T, sz, delete_func>::lenth() const
    {
    	return data_len;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    T* data_wz<T, sz, delete_func>::operator->() const
    {
    	return addr;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    void data_wz<T, sz, delete_func>::operator=(data_wz<T, sz, delete_func>& as)
    {
    	clear();
    	// DBG_PRINT("operator=()\n");
    	addr = as.addr;
    	buf_len = as.buf_len;
    	data_len = as.data_len;
    	as.addr = 0;
    	as.buf_len = 0;
    	as.data_len = 0;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    T& data_wz<T, sz, delete_func>::operator[](int index)
    {
    	if (index < data_len)
    		return addr[index];
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    T& data_wz<T, sz, delete_func>::operator[](long long index)
    {
    	if (index < data_len)
    		return addr[index];
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    T& data_wz<T, sz, delete_func>::operator[](unsigned long long index)
    {
    	if (index < data_len)
    		return addr[index];
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    const T& data_wz<T, sz, delete_func>::operator[](int index) const
    {
    	if (index < data_len)
    		return addr[index];
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    const T& data_wz<T, sz, delete_func>::operator[](long long index) const
    {
    	if (index < data_len)
    		return addr[index];
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    const T& data_wz<T, sz, delete_func>::operator[](unsigned long long index) const
    {
    	if (index < data_len)
    		return addr[index];
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    bool data_wz<T, sz, delete_func>::exist()
    {
    	return addr != 0;
    }

    template<typename T, int sz, void(*delete_func)(T&)>
    void data_wz<T, sz, delete_func>::clear()
    {
    	if (addr)
    	{
    		if (data_len > 1)
    		{
    			// DBG_PRINT("clear() delete[]\n");
    			for (int i = 0; i < data_len; i++)
    			{
    				delete_func(addr[i]);
    			}
    			delete[] addr;
    			addr = 0;
    			buf_len = 0;
    			data_len = 0;
    		}
    		else
    		{
    			// DBG_PRINT("clear() delete\n");
    			delete_func(addr[0]);
    			delete addr;
    			addr = 0;
    			buf_len = 0;
    			data_len = 0;
    		}
    	}
    }

//

    template<class T>
    void stack_default_destruct(T& v)
    {}

    template<class T = int, int depth = 20, void(*default_destruct)(T&) = stack_default_destruct<T>>
    class stack_wz {
    public:
    	data_wz<T, sizeof(T), default_destruct> stack_ptr;
    	uint64_t cnt;
    	std::mutex mtx;
    public:
    	stack_wz();
    	stack_wz(stack_wz<T, depth, default_destruct>& cp);
    	~stack_wz();

    	void operator=(stack_wz<T, depth, default_destruct>& as);
    	void push(T&);
    	void push(const T&);
    	void pop(T& v);
    	void pop();
    	bool empty();
    	void reset();
    	void clear();
    };

    template<class T, int depth, void(*default_destruct)(T&)>
    stack_wz<T, depth, default_destruct>::stack_wz()
    	:stack_ptr(depth), cnt(0)
    {}

    template<class T, int depth, void(*default_destruct)(T&)>
    stack_wz<T, depth, default_destruct>::stack_wz(stack_wz<T, depth, default_destruct>& cp)
    	:stack_ptr(cp.stack_ptr), cnt(cp.cnt)
    {
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    stack_wz<T, depth, default_destruct>::~stack_wz()
    {
    	clear();
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    void stack_wz<T, depth, default_destruct>::operator=(stack_wz<T, depth, default_destruct>& as)
    {
    	stack_ptr = as.stack_ptr;
    	cnt = as.cnt;
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    void stack_wz<T, depth, default_destruct>::push(T& v)
    {
    	if (cnt < stack_ptr.data_len)
    	{
    		stack_ptr.addr[cnt] = v;
    		cnt++;
    	}
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    void stack_wz<T, depth, default_destruct>::push(const T& v)
    {
    	if (cnt < stack_ptr.data_len)
    	{
    		std::unique_lock<std::mutex> lock(mtx);
    		stack_ptr.addr[cnt] = v;
    		cnt++;
    		lock.unlock();
    	}
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    void stack_wz<T, depth, default_destruct>::pop(T& v)
    {
    	if (cnt > 0)
    	{
    		std::unique_lock<std::mutex> lock(mtx);
    		v = stack_ptr.addr[cnt - 1];
    		cnt--;
    		lock.unlock();
    	}
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    void stack_wz<T, depth, default_destruct>::pop()
    {
    	if (cnt > 0)
    	{
    		std::unique_lock<std::mutex> lock(mtx);
    		cnt--;
    		lock.unlock();
    	}
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    bool stack_wz<T, depth, default_destruct>::empty()
    {
    	return cnt == 0;
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    void stack_wz<T, depth, default_destruct>::reset()
    {
    	cnt = 0;
    	for (int i = 0; i < stack_ptr.data_len; i++)
    	{
    		default_destruct(stack_ptr.addr[i]);
    	}
    }

    template<class T, int depth, void(*default_destruct)(T&)>
    void stack_wz<T, depth, default_destruct>::clear()
    {
    	cnt = 0;
    	stack_ptr.clear();
    }

};//命名空间

#endif // _TOOLWZ_HPP_