#ifndef _SOCKETWZ_HPP_
#define _SOCKETWZ_HPP_

#include <QString>
#include <memory>
#include <mutex>

#ifdef _WIN32

#include <Winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

namespace SOCKWZ {
    class Socket {
        WSADATA wsa;
        sockaddr_in localAddr, chipAddr;
        uint16_t localPort, chipPort;
        SOCKET sock{ INVALID_SOCKET };  // 单一socket，既用于发送也用于接收
        QString SockAddr[4];        // 回到4个：本地IP、本地端口、芯片IP、芯片端口
        std::atomic_bool LOCK;
        std::mutex mtx;
    public:
        Socket();
        ~Socket();
        bool Connect(QString localip,
            QString localport,       // 本地端口参数
            QString chipip,
            QString chipport);
        void DisConnect();
        int Send(char* buf, int len);
        int Recv(char* buf, int len);
        void Lock();
        void UnLock();

        const QString const* GetAddr() const;
    };

    class SockGlob {
    public:
        static std::shared_ptr<Socket> Sock;
        static std::mutex mtx;

        static bool Connect(QString localip,
            QString localport,       // 本地端口参数
            QString chipip,
            QString chipport);
        static void DisConnect();
        static void Clear();
        static int Send(char* buf, int len);
        static int Recv(char* buf, int len);
    };
};

#endif

#endif // _SOCKETWZ_HPP_