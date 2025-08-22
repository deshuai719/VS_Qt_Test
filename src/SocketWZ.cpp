//socket相关函数设计，如收发数据，队列进出，网络通断
//

#include "SocketWZ.hpp"
#include <QDebug>
#include "LogWZ.hpp"
#include <winsock2.h>
#include <chrono>

SOCKWZ::Socket::Socket()
    :wsa(),                                                                      // 初始化 WinSock 数据结构，通常为 WSADATA 类型
    localAddr(),                                                                 // 初始化本地地址结构，通常为 sockaddr_in 类型
    chipAddr(),                                                                  // 初始化目标芯片地址结构，通常为 sockaddr_in 类型
    localPort(),                                                                 // 初始化本地端口，类型应为整型，初值为0
    chipPort(),                                                                  // 初始化芯片端口，类型应为整型，初值为0
    sock(INVALID_SOCKET),                                                        // 初始化 socket 句柄为无效值
    SockAddr(),                                                                  // 初始化地址字符串数组，现在为 QString[4]
    LOCK(),                                                                      // 初始化原子锁变量，通常为 std::atomic<int>
    mtx()                                                                        // 初始化互斥锁，通常为 std::mutex
{
    LOCK.store(0);                                                               // 将原子锁变量设置为0，表示未加锁状态
    for (int i = 0; i < 4; i++)                                                  // 更新为4个元素
    {
        SockAddr[i] = "";                                                        // 将地址字符串数组的每一项初始化为空字符串
    }
}

SOCKWZ::Socket::~Socket()
{
}

bool SOCKWZ::Socket::Connect(QString localip,
    QString localport,
    QString chipip,
    QString chipport)
{
    Lock();                                                                      // 加锁，防止多线程同时操作 socket

    if (sock == INVALID_SOCKET)                                                  // 如果当前socket还未创建
    {
        SockAddr[0] = localip;                                                   // 保存本地IP
        SockAddr[1] = localport;                                                 // 保存本地端口
        SockAddr[2] = chipip;                                                    // 保存芯片IP
        SockAddr[3] = chipport;                                                  // 保存芯片端口

        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)                               // 初始化 WinSock2，失败则清理并返回
        {
            WSACleanup();
            UnLock();
            return false;
        }

        // ===== 创建socket =====
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);                         // 创建 UDP socket
        if (sock == INVALID_SOCKET)                                              // 创建失败
        {
            closesocket(sock);                                                   // 关闭 socket（虽然已经无效）
            WSACleanup();                                                        // 清理 WinSock
            UnLock();
            return false;
        }
        u_long iMode = 1;
        ioctlsocket(sock, FIONBIO, &iMode);                                      // 设置socket为非阻塞模式

        // 设置socket缓冲区大小
        int recvBufSize = 1024 * 1024; // 1MB接收缓冲区
        int sendBufSize = 512 * 1024; // 512KB发送缓冲区
        setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufSize, sizeof(recvBufSize));
        setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));

        localPort = localport.toUShort();                                        // 本地端口
        localAddr.sin_family = AF_INET;                                          // 地址族设为 IPv4
        localAddr.sin_port = htons(localPort);                                   // 本地端口号，主机字节序转网络字节序
        inet_pton(AF_INET, localip.toStdString().c_str(), &localAddr.sin_addr);  // 本地IP字符串转为网络地址

        if (bind(sock, (sockaddr*)&localAddr, sizeof(sockaddr)) == SOCKET_ERROR) // 绑定socket到本地端口
        {
            closesocket(sock);                                                   // 绑定失败，关闭 socket
            WSACleanup();                                                        // 清理 WinSock
            UnLock();
            return false;
        }

        // ===== 设置目标芯片地址 =====
        memset(&chipAddr, 0, sizeof(sockaddr));                                  // 清空芯片地址结构体
        chipPort = chipport.toUShort();                                          // 芯片端口字符串转为无符号短整型
        chipAddr.sin_family = AF_INET;                                           // 地址族设为 IPv4
        chipAddr.sin_port = htons(chipPort);                                     // 芯片端口号，主机字节序转网络字节序
        inet_pton(AF_INET, chipip.toStdString().c_str(), &chipAddr.sin_addr);    // 芯片IP字符串转为网络地址
    }
    UnLock();                                                                    // 解锁
    return true;                                                                 // 返回连接成功
}

void SOCKWZ::Socket::DisConnect()
{
    Lock();
    
    // 关闭socket
    if (sock != INVALID_SOCKET)
    {
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
    
    // 清理WinSock
    WSACleanup();
    
    UnLock();
}

int SOCKWZ::Socket::Send(char* buf, int len)
{
    if (sock == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    // 统计两次sendto调用的间隔时间
    static std::chrono::steady_clock::time_point last_send_time;
    auto now = std::chrono::steady_clock::now();
    if (last_send_time.time_since_epoch().count() != 0) {
        auto interval = std::chrono::duration_cast<std::chrono::microseconds>(now - last_send_time).count();
        WRITE_TASK_DATA_SEND_DBG("sendto 间隔时间: %lld us\n", interval);
    }
    last_send_time = now;

    auto t_send_start = std::chrono::steady_clock::now();
    int ret = sendto(sock, buf, len, 0, (sockaddr*)&chipAddr, sizeof(sockaddr));
    auto t_send_end = std::chrono::steady_clock::now();
    auto sendto_ms = std::chrono::duration_cast<std::chrono::microseconds>(t_send_end - t_send_start).count();
    WRITE_TASK_DATA_SEND_DBG("sendto 实际耗时: %lld us\n", sendto_ms);

    if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        // 对WSAEWOULDBLOCK错误给予更友好的处理，避免过多日志输出
        if (err == WSAEWOULDBLOCK) {
            // WSAEWOULDBLOCK (10035) 是非阻塞socket的正常行为，表示缓冲区已满
            // 只在调试模式下输出详细信息，减少正常运行时的日志噪音
            WRITE_TASK_DATA_SEND_DBG("[Socket::Send] Socket buffer full (WSAEWOULDBLOCK), this is normal for non-blocking sockets\n");
        } else {
            // 其他错误仍然输出到调试日志
            qDebug() << "[Socket::Send] sendto failed, WSAGetLastError=" << err;
        }
    }
    return ret;
}

//int SOCKWZ::Socket::Recv(char* buf, int len)
//{
//    int socklen = sizeof(sockaddr);
//    //Lock();
//    // std::unique_lock<std::mutex> lk(mtx);
//    if(sock == INVALID_SOCKET)
//    {
//        //UnLock();
//        return SOCKET_ERROR;
//    }
//    int ret = recvfrom(sock, buf, len, 0, (sockaddr*)&chipAddr, &socklen);
//    //UnLock();
//    return ret;
//}

int SOCKWZ::Socket::Recv(char* buf, int len)
{
    int socklen = sizeof(sockaddr);
    if (sock == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    auto t_recv_start = std::chrono::steady_clock::now(); // 记录开始时间

    // 设置8ms超时
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 8000; // 8ms

    auto t_select_start = std::chrono::steady_clock::now(); // select前时间
    int sel = select(0, &readfds, nullptr, nullptr, &timeout);
    auto t_select_end = std::chrono::steady_clock::now();   // select后时间
    auto select_cost = std::chrono::duration_cast<std::chrono::microseconds>(t_select_end - t_select_start).count();
    if (select_cost > 20000) {
        WRITE_TASK_DATA_SEND_DBG("!!!select调用耗时超20ms: %lld us\n", select_cost);
    }

    int ret = SOCKET_ERROR;
    auto t_if_check = std::chrono::steady_clock::now(); // select结束到if判断前时间

    // 新增：连续高延迟监控与优先级自适应调整
    static int high_latency_count = 0;
    static bool priority_adjusted = false;

    if (sel > 0 && FD_ISSET(sock, &readfds)) {
        auto t_recvfrom_start = std::chrono::steady_clock::now(); // 记录开始时间
        auto select_to_if_cost = std::chrono::duration_cast<std::chrono::microseconds>(t_recvfrom_start - t_if_check).count();
        if (select_to_if_cost > 20000) 
        {
            WRITE_TASK_DATA_SEND_DBG("!!!select结束到进入if判断间隔超20ms: %lld us\n", select_to_if_cost);
        }
        ret = recvfrom(sock, buf, len, 0, (sockaddr*)&chipAddr, &socklen);
        auto t_recvfrom_end = std::chrono::steady_clock::now(); // 记录结束时间
        auto recvfrom_cost = std::chrono::duration_cast<std::chrono::microseconds>(t_recvfrom_end - t_recvfrom_start).count();
        if (recvfrom_cost > 20000) {
            WRITE_TASK_DATA_SEND_DBG("!!!Socket Recv发送耗时超20ms: %lld us\n", recvfrom_cost);
        }
        WRITE_TASK_DATA_SEND_DBG("select1=%d, recvfrom ret=%d\n", sel, ret);
        // 检测连续高延迟，自动降低优先级
        if (recvfrom_cost > 20000) {
            high_latency_count++;
            if (high_latency_count > 3 && !priority_adjusted) {
                SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
                priority_adjusted = true;
                WRITE_TASK_DATA_SEND_DBG("检测到连续高延迟，降低线程优先级\n");
            }
        } else {
            high_latency_count = 0;
            if (priority_adjusted) {
                SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
                priority_adjusted = false;
            }
        }
    }
    else {
        WRITE_TASK_DATA_SEND_DBG("select0=%d, recvfrom ret=%d\n", sel, ret);
    }

    auto t_recv_end = std::chrono::steady_clock::now(); // 记录结束时间
    auto recv_cost= std::chrono::duration_cast<std::chrono::microseconds>(t_recv_end - t_recv_start).count();
    if (recv_cost > 20000) {
        WRITE_TASK_DATA_SEND_DBG("!!!Socket Recv整体耗时超20ms: %lld us\n", recv_cost);
    }

    return ret;
}

void SOCKWZ::Socket::Lock()
{
    while (LOCK.exchange(1));
}

void SOCKWZ::Socket::UnLock()
{
    LOCK.exchange(0);
}

const QString const* SOCKWZ::Socket::GetAddr() const
{
    return SockAddr;
}

//

bool SOCKWZ::SockGlob::Connect(QString localip,
    QString localport,
    QString chipip,
    QString chipport)
{
    std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        return Sock->Connect(localip, localport, chipip, chipport);
    }
    return false;
}

void SOCKWZ::SockGlob::DisConnect()
{
    std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        Sock->DisConnect();
    }
}

void SOCKWZ::SockGlob::Clear()
{
    std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        Sock->DisConnect();
        Sock.reset();
        Sock = nullptr;
    }
}

int SOCKWZ::SockGlob::Send(char* buf, int len)
{
    //std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        return Sock->Send(buf, len);
    }
    return SOCKET_ERROR;
}

int SOCKWZ::SockGlob::Recv(char* buf, int len)
{
    //std::unique_lock<std::mutex> lk(mtx);
    if (Sock != nullptr)
    {
        return Sock->Recv(buf, len);
    }
    return SOCKET_ERROR;
}

std::shared_ptr<SOCKWZ::Socket> SOCKWZ::SockGlob::Sock = std::make_shared<SOCKWZ::Socket>();
std::mutex SOCKWZ::SockGlob::mtx;