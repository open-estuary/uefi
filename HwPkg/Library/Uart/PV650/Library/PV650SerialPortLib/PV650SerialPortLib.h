/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpat0TH65jPMlUENBvGXufQCm4LaY1Scm2hXUYwJOrTRw72N6o5uLoxSzomTX
lSLrv4MJuO9Rs9UpcpOwuiRDUa+wXPrrfKHb0yf/8p5LP/GmN+5SVXT02xnlWPegL0jY6bGK
KtB21iYdwjOHnQNKEeuanocfCfYWwqRUiK4LNOZvsICa89ydiuWeNR/6Y7bCBQ==*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#ifndef __PV650_SERIALPORTLIB_H__
#define __PV650_SERIALPORTLIB_H__


#define UART_USED_CHANNELS          1           /*使用的串口数量*/
#define TCXO_CLK_FREQ               26000000    /*时钟频率*/

// 寄存器基地址通过PCD来指定
#define SERIAL_0_BASE_ADR     (PcdGet64(PcdSerialRegisterBase)) /*uart0 基址*/

// 波特率从PCD获取
#define UART_SEND_DELAY      (PcdGet32(PcdSerialPortSendDelay))     /*发送等待时延*/  //modified by t00216239
#define BAUDRATE             (PcdGet64(PcdUartDefaultBaudRate))

/*串口寄存器地址定义*/
#define UART_THR_REG         (SERIAL_0_BASE_ADR + UART_THR)
#define UART_RBR_REG         (SERIAL_0_BASE_ADR + UART_RBR)
#define UART_DLL_REG         (SERIAL_0_BASE_ADR + UART_DLL)
#define UART_DLH_REG         (SERIAL_0_BASE_ADR + UART_DLH)
#define UART_IEL_REG         (SERIAL_0_BASE_ADR + UART_IEL)
#define UART_IIR_REG         (SERIAL_0_BASE_ADR + UART_IIR)
#define UART_FCR_REG         (SERIAL_0_BASE_ADR + UART_FCR)
#define UART_LCR_REG         (SERIAL_0_BASE_ADR + UART_LCR)
#define UART_LSR_REG         (SERIAL_0_BASE_ADR + UART_LSR)
#define UART_USR_REG         (SERIAL_0_BASE_ADR + UART_USR)

/* 寄存器地址偏移定义 */
#define UART_RBR     0x00            /* 接收数据缓存寄存器。*/
#define UART_THR     0x00            /* 发送数据缓存寄存器*/
#define UART_DLL     0x00            /* 波特率低位分频因子寄存器*/
#define UART_DLH     0x04            /* 波特率高位分频因子寄存器*/
#define UART_IEL     0x04            /* 中断使能寄存器*/
#define UART_IIR     0x08            /* 中断标识控制寄存器*/
#define UART_FCR     0x08            /* FIFO控制寄存器*/
#define UART_LCR     0x0C            /* 线性控制寄存器*/
#define UART_MCR     0x10            /*  Modem控制寄存器*/
#define UART_LSR     0x14            /* 线性状态寄存器*/
#define UART_USR     0x7C            /* 状态寄存器*/

/* register definitions */

#define UART_FCR_EN		     0x01		/* FIFO使能 */
#define UART_FCR_RXCLR       0x02		/* 清接收FIFO */
#define UART_FCR_TXCLR       0x04		/* 清发送FIFO */
#define UART_FCR_CLEARFIFO   0x00      /* 清FIFO出发水线*/
#define UART_FCR_RXL1        0x00
#define UART_FCR_RXL4        0x40
#define UART_FCR_RXL8        0x80
#define UART_FCR_RXL14       0xc0
#define UART_FCR_TXL0        0x00
#define UART_FCR_TXL4        0x20
#define UART_FCR_TXL8        0x30
#define UART_FCR_TXL14       0x10

/*LCR ● Name: Line Control Register fields*/
#define UART_LCR_DLAB   0x80     /*0－禁止；1－使能访问频因子锁存器*/
#define UART_LCR_EPS    0x10     /*0-奇校验;1-偶校验*/
#define UART_LCR_PEN    0x08     /*0－关闭；1－使能奇偶校验；*/
#define UART_LCR_STOP   0x04     /* 0 = 1 停止位;1=1.5或2个停止位*/
#define UART_LCR_DLS8   0x03    /* 00=5bit;01=6bit;10=7bit;11=8bit 数据*/
#define UART_LCR_DLS7   0x02    /* 00=5bit;01=6bit;10=7bit;11=8bit 数据*/
#define UART_LCR_DLS6   0x01    /* 00=5bit;01=6bit;10=7bit;11=8bit 数据*/
#define UART_LCR_DLS5   0x00    /* 00=5bit;01=6bit;10=7bit;11=8bit 数据*/

/*DLL DLH ● Name: 波特率分频因子寄存器区域*/
#define UART_DLH_AND_DLL_WIDTH 0xFF /* 高低位所存器用于保存分频因子的bit位宽*/

/*IER ● Name: 中断使能寄存器区域*/
#define UART_IER_PTIME  0x80     /*发送FIFO空中断使能*/
#define UART_IER_ELSI   0x04    /* 接收错误中断使能*/
#define UART_IER_ETBEI  0x02    /* 发送缓冲寄存器空中断使能*/
#define UART_IER_ERBFI  0x01    /* 接收数据有效中断使能*/

/*IIR ● Name: 中断标识寄存器区域*/
#define UART_IIR_FIFOSE         0xC0    /* FIFOs 使能: 00 = 禁止; 11 = 使能;*/
/*中断类型指示:
0x1：无中断；
0x2：发送空中断，包括FIFO空和发送缓冲寄存器空（FIFO不使能）两种中断；
0x4：接收数据有效中断，接收FIFO满和接收缓冲寄存器满（FIFO不使能）两种情况下触发该中断；
0x6：接收错误中断，包括数据溢出、奇偶校验错误、帧错误以及接收异常中断；
0x7：忙中断。当UART收发数据，处于忙状态（USR[0]=1）时，如果处理器操作LCR寄存器，将产生该中断；
0xC：超时中断。在连续的4个字符收发时间内没有数据输入或输出接收FIFO将产生该中断。*/

#define UART_IIR_InterruptID   0x01    
#define UART_IIR_INTIDTE       0x02
#define UART_IIR_INTIDRA       0x04
#define UART_IIR_INTIDRLS      0x06
#define UART_IIR_INTMASK       0x0f
#define UART_IIR_RDA           0x04
#define UART_IIR_TE            0x02
/*LSR ● Name: 线性状态寄存器区域*/
#define UART_LSR_TEMT       0x40    /* 发送清空位*/
#define UART_LSR_THRE       0x20    /* 发送缓冲寄存器清空位*/
#define UART_LSR_BI         0x10    /* Break中断位*/
#define UART_LSR_FE         0x08    /* 帧错误位*/
#define UART_LSR_PE         0x04    /* 校验错误位*/
#define UART_LSR_R          0x02    /* 溢出错误位*/
#define UART_LSR_DR         0x01    /* 数据准备位*/

/*USR● Name: 串口状态寄存器*/
#define UART_USR_BUSY  0x01    /*串口忙*/

#define FIFO_MAXSIZE    32      /* FIFO最大深度 */

extern UINT32 UART_UartClkFreq(VOID);
extern UINT8 UART_ChkSndEnd(VOID);
extern UINT8 SerialPortReadChar(VOID);
extern VOID SerialPortWriteChar(UINT8 scShowChar);

#endif

