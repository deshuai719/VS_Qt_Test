#ifndef _DATAFRAMEWZ_HPP_
#define _DATAFRAMEWZ_HPP_

#define NUM_MNICx4_BOARD 8

// #define TEST_WITHOUT_BOARD
#undef TEST_WITHOUT_BOARD

namespace DF{

    typedef struct{
        unsigned char msgID;//消息ID
        unsigned char rev : 4;
        unsigned char ctlFractstop : 1;
        unsigned char ctlFractStrt : 1;
        unsigned char ctlFractMode : 1;
        unsigned char reserved_base : 1;
        unsigned short siglen;
    }FRAME_HEAD;

    #define HEAD_DATA_LEN 4
    #define FRAME_HEAD_STAR(data)  (reinterpret_cast<DF::FRAME_HEAD*>(data))

    //0xa0

    typedef struct{
        unsigned char data_bound_num8b:   5; // 数据边界，以字节为单位，表示一个数据占几个字节，1~31，
        unsigned char read_flash2ram :    1; // 读 flash 到 ram 使能，高有效，与read_ram 同时有效时，先 read_flash 后 read_ram
        unsigned char read_ram :          1; // 读 ram 使能，高有效
        unsigned char write_ram :         1; // 写使能，高有效，读 ram 写 ram 使能同时有效时，先写后读，暂不支持读 flash 写 ram 同时有效
        unsigned char chksum_head; // 校验和，8bit 累加计数，仅包含 8 字节头
        unsigned short data_depth; // 数据深度，以数据位宽为单位，1~65535，0 表示 65536
        unsigned short addr_start; // 起始地址，以数据位宽为单位
        unsigned char ram_id; // RAM 的 ID
        unsigned char reserved3; // 
    }DN_ARG;

    #define PTR_DN_ARG_A0(ptr)  (reinterpret_cast<DF::DN_ARG*>(ptr))

    //下发数据包

    typedef struct{
        unsigned short  channel;//指定为0
        unsigned short  reserved;   // used as counter, to check if is continuous   // 每次下发通道0的数据包时加1
    }DN_MNIC_64K;

    #define DN_MNIC_64k_DATA_LEN 4
    #define DN_MNIC_64K_STAR(buf)    (reinterpret_cast<DF::DN_MNIC_64K*>(buf))

    //上报数据包

    typedef struct {    // 8 bytes
        unsigned short  sinad   :12;    // postive dB, unit i
        unsigned short  reserved: 4;
        unsigned short  freqInHz;       // aud freq, unit is 
        unsigned short  vpp     ;       // peak to peak volta
        unsigned short  rms     ;       // root-mean-square v
    }AUD_INFO;
    
    typedef struct {//子板上各个芯片各通道信息
        AUD_INFO    Info[12];
    }MNICx4_ADC_INFO;

    typedef struct {    // (16 + N*12*8) bytes
        unsigned short  timeStamp_8ms   ;   // counter of 8ms, 512 points sample    // 每次加1的递增数，用作时标
        unsigned short  free_codec_dac  ;   // freew of codec dac, 64KHz, unit is 16-bit    // 下发64K音频的流控反馈
        unsigned int  mnic_online     ;   // 1, online, 0, offline, [3:0], board 0 mnic 0~3, [7:4], board 1 mnic 0~3, ..., [31:28], board 7 mnic 0~3
        short  ds18b20_16b     ;   // temperature sensor
        unsigned short  fpga_heat       ;   // fpga inner temperature sensor
        unsigned short  dbg_dn_aic      ;   // debug counter of dn_ch0_aic packet if packet counter is not continuous
        unsigned char cnt_dn_empty;
        unsigned char cnt_dn_full;
        //unsigned short  cnt_dn_aic      ;   // counter of dn_ch0_aic packet
        MNICx4_ADC_INFO MNICx4_ADC_INFO_ARR[NUM_MNICx4_BOARD]; // multi 
    }UP_MNIC_STA;   // 上报小芯片状态，定时8ms一包

    #define PTR_UP_MNIC_STA(buf)    (reinterpret_cast<DF::UP_MNIC_STA*>((buf)))

    //流控反馈0x10

    class RPT_PTT_ETC {
    public:
        unsigned int timeStamp_1S0;
        unsigned short timeStamp_4K8;
        unsigned short tabPeak_ii;
        unsigned short ptt;
        unsigned short fifoRdCmdFreeW;
        unsigned short free_qsym[8];
        unsigned short free_qsig[8];
        unsigned short free_qaic[8];
    };
        
    #define PTR_RPT_PTT_ETC(buf) (reinterpret_cast<DF::RPT_PTT_ETC*>((buf)))
        
    //0x11
    
    class RPT_IF_FREE {
    public:
        unsigned short rxRam_freew[16];
        unsigned short turnRam_freew[8];
        unsigned char rxRam_writeWhenFull[16];
        unsigned char txRam_WriteWhenFull[16];
        unsigned char txFifo_catchCutFrame;
        unsigned char txArbit_timeoutTxFrame;
        unsigned short reserved2;
    };
    
    #define PTR_RPT_IF_FREE(buf)    (reinterpret_cast<DF::RPT_IF_FREE*>((buf)))

    //0xa0 up

    typedef struct{
        unsigned char data_bound_num8b: 5; // 数据边界，以字节为单位，表示一个数据占几个字节
        unsigned char reserved : 3; // 
        unsigned char chksum_head; // 校验和，8bit 累加计数，仅包含 8 字节头
        unsigned short data_depth; // 数据深度，以数据位宽为单位，1~65535，110 表示 65536
        unsigned short addr_start; // 起始地址，以数据位宽为单位
        unsigned char ram_id; // RAM 的 ID
        unsigned char reserved0 : 6; //
        unsigned char chk_fail : 1; // 校验失败
        unsigned char cmd_ack : 1; // 指令应答，当为 1 时下发的指令有效
    }ARG_UP;

    #define PTR_ARG_UP_A0(ptr)  (reinterpret_cast<DF::ARG_UP*>(ptr))

    //上报版本号信息

    typedef struct{
        unsigned long long SEC: 8;
        unsigned long long MIN: 8;
        unsigned long long HOUR: 8;
        unsigned long long DAY: 8;
        unsigned long long MONTH: 8;
        unsigned long long YEAR: 8;
        unsigned long long VER: 16;
    }VERSION_UP;
    
    #define PTR_VERSION_UP(ptr) (reinterpret_cast<DF::VERSION_UP*>(ptr))
}

#endif