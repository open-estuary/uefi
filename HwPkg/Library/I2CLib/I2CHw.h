/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWuVy7vy/wDnq7gJfHBOj2pBXFF9pJtpDLt9sw5WJiMsUkN5d7jr7
aK5J3kmlnl+vpat0TH65jPMlUENBvGXufQCTRBL0qDbp15usAFL4sRUNaa9PQnehq7qerrn2
LiJ41omhITDeCH/C4foR79uiWZBenn+D71khwAjSMtqE06geZFKbTvn0y5Ny8Gb0pRReuPVw
PohMSutCKQUqUscz5cZTh7VMSm/lzVfxl78+2sOPB7qtPbNytjaTz2/X+3ivwA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/******************************************************************************

                  版权所有 (C), 2009-2019, 华为技术有限公司

 ******************************************************************************
  文 件 名   : I2CHw.h
  版 本 号   : v1.0
  作    者   : C00213799
  生成日期   : 2013年03月15日
  最近修改   :
  功能描述   : 头文件
  修改历史   :
1.   日	  期   : 
     作	  者   :
     修改内容  :
******************************************************************************/

#ifndef _I2C_HW_H_
#define _I2C_HW_H_

#define I2C_READ_TIMEOUT             500
#define I2C_DRV_ONCE_WRITE_BYTES_NUM 8        /*I2C驱动中,每次写操作最多写8bytes*/
#define I2C_DRV_ONCE_READ_BYTES_NUM  8        /*I2C驱动中,每次读操作最多读8bytes*/
#define I2C_READ_SIGNAL              0x0100               /*读信号*/
#define I2C_TXRX_THRESHOLD           0x7
#define I2C_SS_SCLHCNT               0x493      /*标准速度模式下SCL时钟高电平周期数*/
#define I2C_SS_SCLLCNT               0x4fe      /*标准速度模式下SCL时钟低电平时间*/

#define I2C_REG_WRITE(reg,data) \
     (*((volatile UINT32 *)(reg)) = (data))

#define I2C_REG_READ(reg,result) \
     (result) = *((volatile UINT32 *)(UINTN)(reg))

 /*寄存器表项名的枚举定义*/
 #define    I2C_CON_OFFSET                 0x0 /* I2C0_CON为I2C0控制寄存器。注意：只在I2C0被禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 #define    I2C_TAR_OFFSET                 0x4 /* I2C0_TAR为I2C0访问Slave地址寄存器。注意：只在I2C0被禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 #define    I2C_SAR_OFFSET                 0x8 /* I2C0_SAR为I2C0访问源设备地址寄存器。注意：只在I2C0被禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 #define    I2C_DATA_CMD_OFFSET            0x10 /* I2C0_DATA_CMD为I2C0数据通道寄存器。 */
 #define    I2C_SS_SCL_HCNT_OFFSET         0x14 /* I2C0_SS_SCL_HCNT为标准速度下的SCL时钟高电平时间寄存器。注意：只在I2C0被禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 #define    I2C_SS_SCL_LCNT_OFFSET         0x18 /* I2C0_SS_SCL_LCNT为标准速度下的SCL时钟低电平时间寄存器。注意：只在I2C0禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 #define    I2C_FS_SCL_HCNT_OFFSET         0x1c /* I2C0_FS_SCL_HCNT为快速速度下的SCL 时钟高电平时间寄存器。注意：只在I2C0禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 #define    I2C_FS_SCL_LCNT_OFFSET         0x20 /* I2C0_FS_SCL_LCNT为快速速度下的SCL 时钟低电平时间寄存器。I2C0_FS_SCL_LCNT只在I2C0禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 #define    I2C_INTR_STAT_OFFSET           0x2c /* I2C0_INTR_STAT为I2C0中断状态寄存器。 */
 #define    I2C_INTR_MASK_OFFSET           0x30 /* I2C0_INTR_MASK为I2C0中断屏蔽寄存器。 */
 #define    I2C_RAW_INTR_STAT_OFFSET       0x34 /* I2C0_RAW_INTR_STAT为I2C0原始中断状态寄存器。 */
 #define    I2C_RX_TL_OFFSET               0x38 /* I2C0_RX_TL为RX_FIFO阈值寄存器。 */
 #define    I2C_TX_TL_OFFSET               0x3c /* I2C0_TX_TL为TX_FIFO阈值寄存器。 */
 #define    I2C_CLR_INTR_OFFSET            0x40 /* I2C0_CLR_INTR为I2C0组合及独立中断清除寄存器。 */
 #define    I2C_CLR_RX_UNDER_OFFSET        0x44 /* I2C0_CLR_RX_UNDER为RX_UNDER中断清除寄存器。 */
 #define    I2C_CLR_RX_OVER_OFFSET         0x48 /* I2C0_CLR_RX_OVER为RX_OVER中断清除寄存器。 */
 #define    I2C_CLR_TX_OVER_OFFSET         0x4c /* I2C0_CLR_TX_OVER为TX_OVER中断清除寄存器。 */
 #define    I2C_CLR_RD_REQ_OFFSET          0x50 /* I2C0_CLR_RD_REQ为RD_REQ中断清除寄存器。 */
 #define    I2C_CLR_TX_ABRT_OFFSET         0x54 /* I2C0_CLR_TX_ABRT为TX_ABRT中断清除寄存器。 */
 #define    I2C_CLR_RX_DONE_OFFSET         0x58 /* I2C0_CLR_RX_DONE为RX_DONE中断清除寄存器。 */
 #define    I2C_CLR_ACTIVITY_OFFSET        0x5c /* I2C0_CLR_activity为activity状态寄存器。 */
 #define    I2C_CLR_STOP_DET_OFFSET        0x60 /* I2C0_CLR_STOP_DET为STOP_DET中断清除寄存器。 */
 #define    I2C_CLR_START_DET_OFFSET       0x64 /* I2C0_CLR_START_DET为START_DET中断清除寄存器。 */
 #define    I2C_CLR_GEN_CALL_OFFSET        0x68 /* I2C0_CLR_GEN_CALL为General Call中断清除寄存器。 */
 #define    I2C_ENABLE_OFFSET              0x6c /* I2C0_ENABLE为I2C0工作模式使能寄存器。 */
 #define    I2C_STATUS_OFFSET              0x70 /* I2C0_STATUS为I2C0状态寄存器。 */
 #define    I2C_TXFLR_OFFSET               0x74 /* I2C0_TXFLR为TX_FIFO中的数据个数指示寄存器。 */
 #define    I2C_RXFLR_OFFSET               0x78 /* I2C0_RXFLR为RX_FIFO中的数据个数指示寄存器。 */
 #define    I2C_SDA_HOLD                   0x7c /* I2C_SDA_HOLD为SDA信号在SCL的下降沿后的hold延迟时间。 */
 #define    I2C_TX_ABRT_SOURCE_OFFSET      0x80 /* I2C0_TX_ABRT_SOURCE为发送失败中断源头寄存器。读I2C0_CLR_INTR或I2C0_CLR_TX_ABRT时，I2C0_TX_ABRT_SOURCE清零。 */
 #define    I2C_SLV_DATA_ONLY_OFFSET       0x84 /* I2C0_SLV_DATA_ONLY为slave数据NACK产生控制寄存器。 */
 #define    I2C_DMA_CR_OFFSET              0x88 /* I2C0_DMA_CR为I2C0的DMA通道开启控制寄存器。 */
 #define    I2C_DMA_TDLR_OFFSET            0x8c /* I2C0_DMA_TDLR为TX_FIFO DMA操作阈值寄存器。 */
 #define    I2C_DMA_RDLR_OFFSET            0x90 /* I2C0_DMA_RDLR为RX_FIFO DMA操作阈值寄存器。 */
 #define    I2C_SDA_SETUP_OFFSET           0x94 /* I2C0_SDA_SETUP为SDA建立时间设置寄存器。 */
 #define    I2C_ACK_GENERAL_CALL_OFFSET    0x98 /* I2C0_ACK_GENERAL_CALL为General Call应答设置寄存器。 */
 #define    I2C_ENABLE_STATUS_OFFSET       0x9c /* I2C0_ENABLE_STATUS为I2C0状态寄存器。 */
 
 /* I2C0_CON为I2C0控制寄存器。注意：只在I2C0被禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 typedef union tagI2c0Con
 {
     struct
     {
        UINT32      master                : 1   ; /* [0] I2C0主模式使能。 */
        UINT32      spedd                 : 2   ; /* [2..1] 速度选择。 */
        UINT32      slave_10bit           : 1   ; /* [3] I2C0从模式使能时，从模式下响应地址长度。 */
        UINT32      master_10bit          : 1   ; /* [4] 主模式下地址长度。 */
        UINT32      restart_en            : 1   ; /* [5] 主模式下Restart指令发送使能。 */
        UINT32      slave_disable         : 1   ; /* [6] I2C0从模式使能。 */
        UINT32      Reserved_0            : 25  ; /* [31..7] 保留。 */
     } bits;
     UINT32     Val32;
 } I2C0_CON_U;
 
 /* I2C0_TAR为I2C0访问Slave地址寄存器。注意：只在I2C0被禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 typedef union tagI2c0Tar
 {
     struct
     {
         UINT32      ic_tar                : 10  ; /* [9..0] I2C0作为Master时要访问的Slave的地址。 */
         UINT32      gc_or_start           : 1   ; /* [10] 当special位为1时，该位定义执行的I2C0命令。 */
         UINT32      special               : 1   ; /* [11] General Call或Start Byte功能使能。 */
         UINT32      ic_10bitaddr_master   : 1   ; /* [12] 主模式下地址长度。 */
         UINT32      Reserved_1            : 19  ; /* [31..13] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_TAR_U;
 
 /* I2C0_DATA_CMD为I2C0数据通道寄存器。 */
 typedef union tagI2c0DataCmd
 {
     struct
     {
         UINT32      dat                   : 8   ; /* [7..0] 将要在I2C0总线上发送/接收的数据。 */
         UINT32      cmd                   : 1   ; /* [8] I2C0作为Master时的读写方向控制。 */
         UINT32      Reserved_5            : 23  ; /* [31..9] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_DATA_CMD_U;
 
 /* I2C0_SS_SCL_HCNT为标准速度下的SCL时钟高电平时间寄存器。注意：只在I2C0被禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 typedef union tagI2c0SsSclHcnt
 {
     struct
     {
         UINT32      ic_ss_scl_hcnt        : 16  ; /* [15..0] 标准速度模式下SCL时钟高电平周期数。 */
         UINT32      Reserved_7            : 16  ; /* [31..16] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_SS_SCL_HCNT_U;
 
 /* I2C0_SS_SCL_LCNT为标准速度下的SCL时钟低电平时间寄存器。注意：只在I2C0禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 typedef union tagI2c0SsSclLcnt
 {
     struct
     {
         UINT32      ic_ss_scl_lcnt        : 16  ; /* [15..0] 标准速度模式下SCL时钟低电平时间。 */
         UINT32      Reserved_9            : 16  ; /* [31..16] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_SS_SCL_LCNT_U;
 
 /* I2C0_FS_SCL_HCNT为快速速度下的SCL 时钟高电平时间寄存器。注意：只在I2C0禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 typedef union tagI2c0FsSclHcnt
 {
     struct
     {
         UINT32      ic_fs_scl_hcnt        : 16  ; /* [15..0] 快速模式下SCL时钟高电平时间，用来发送Master Code和 Start Byte或General Call。 */
         UINT32      Reserved_11           : 16  ; /* [31..16] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_FS_SCL_HCNT_U;
 
 /* I2C0_FS_SCL_LCNT为快速速度下的SCL 时钟低电平时间寄存器。I2C0_FS_SCL_LCNT只在I2C0禁止（即I2C0_ENABLE[enable]为0）时才可配置。 */
 typedef union tagI2c0FsSclLcnt
 {
     struct
     {
         UINT32      ic_fs_scl_lcnt        : 16  ; /* [15..0] 快速模式下SCL时钟低电平时间，用来发送Master Code和 Start Byte 或General Call。 */
         UINT32      Reserved_13           : 16  ; /* [31..16] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_FS_SCL_LCNT_U;
 
 /* I2C0_INTR_MASK为I2C0中断屏蔽寄存器。 */
 typedef union tagI2c0IntrMask
 {
     struct
     {
         UINT32      m_rx_under            : 1   ; /* [0] 读数据溢出中断屏蔽。 */
         UINT32      m_rx_over             : 1   ; /* [1] 接收FIFO溢出中断屏蔽。 */
         UINT32      m_rx_full             : 1   ; /* [2] 接收FIFO达到或超过阈值中断屏蔽。 */
         UINT32      m_tx_over             : 1   ; /* [3] 发送FIFO溢出中断屏蔽。 */
         UINT32      m_tx_empty            : 1   ; /* [4] 发送FIFO到达或者低于阈值中断屏蔽。 */
         UINT32      m_rd_req              : 1   ; /* [5] 接收请求中断屏蔽。 */
         UINT32      m_tx_abrt             : 1   ; /* [6] 传输中止中断屏蔽。 */
         UINT32      m_rx_done             : 1   ; /* [7] 接收完成中断屏蔽。 */
         UINT32      m_activity            : 1   ; /* [8] activity中断屏蔽。 */
         UINT32      m_stop_det            : 1   ; /* [9] stop detect中断屏蔽。 */
         UINT32      m_start_det           : 1   ; /* [10] start detect中断屏蔽。 */
         UINT32      m_gen_call            : 1   ; /* [11] 一次General Call请求被接收中断屏蔽。 */
         UINT32      Reserved_17           : 20  ; /* [31..12] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_INTR_MASK_U;
 
 /* I2C0_RX_TL为RX_FIFO阈值寄存器。 */
 typedef union tagI2c0RxTl
 {
     struct
     {
         UINT32      rx_tl                 : 8   ; /* [7..0] 接收FIFO阈值。 */
         UINT32      Reserved_21           : 24  ; /* [31..8] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_RX_TL_U;
 
 /* I2C0_TX_TL为TX_FIFO阈值寄存器。 */
 typedef union tagI2c0TxTl
 {
     struct
     {
         UINT32      tx_tl                 : 8   ; /* [7..0] 发送FIFO阈值。 */
         UINT32      Reserved_23           : 24  ; /* [31..8] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_TX_TL_U;
 
 /* I2C0_ENABLE为I2C0工作模式使能寄存器。 */
 typedef union tagI2c0Enable
 {
     struct
     {
         UINT32      enable                : 1   ; /* [0] I2C0工作模式使能。 */
         UINT32      Reserved_47           : 31  ; /* [31..1] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_ENABLE_U;
 
 /* I2C0_STATUS为I2C0状态寄存器。 */
 typedef union tagI2c0Status
 {
     struct
     {
         UINT32      activity              : 1   ; /* [0] I2C0活跃状态。 */
         UINT32      tfnf                  : 1   ; /* [1] 发送FIFO未满标志。 */
         UINT32      tfe                   : 1   ; /* [2] 发送FIFO空标志。 */
         UINT32      rfne                  : 1   ; /* [3] 接收FIFO非空标志。 */
         UINT32      rff                   : 1   ; /* [4] 接收FIFO满标志。 */
         UINT32      mst_activity          : 1   ; /* [5] Master FSM活动状态。 */
         UINT32      slv_activity          : 1   ; /* [6] I2C0 Slave功能活动状态。 */
         UINT32      Reserved_49           : 25  ; /* [31..7] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_STATUS_U;
 
 /* I2C0_TXFLR为TX_FIFO中的数据个数指示寄存器。 */
 typedef union tagI2c0Txflr
 {
     struct
     {
         UINT32      txflr                 : 4   ; /* [3..0] 发送FIFO中的有效数据个数。 */
         UINT32      Reserved_51           : 28  ; /* [31..4] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_TXFLR_U;
 
 /* I2C0_RXFLR为RX_FIFO中的数据个数指示寄存器。 */
 typedef union tagI2c0Rxflr
 {
     struct
     {
         UINT32      rxflr                 : 4   ; /* [3..0] 接收FIFO数据个数寄存器，该寄存器包含当前接收FIFO中的有效数据个数。 */
         UINT32      Reserved_53           : 28  ; /* [31..4] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_RXFLR_U;
 
 /* I2C0_ENABLE_STATUS为I2C0状态寄存器。 */
 typedef union tagI2c0EnableStatus
 {
     struct
     {
         UINT32      ic_en                 : 1   ; /* [0] I2C0使能。 */
         UINT32      slv_disable_while_busy: 1   ; /* [1] 当发送或接收忙的时候Slave禁止。 */
         UINT32      slv_rx_data_lost      : 1   ; /* [2] Slave接收数据丢失状态。 */
         UINT32      Reserved_69           : 29  ; /* [31..3] 保留。 */
     } bits;
     UINT32      Val32;
 } I2C0_ENABLE_STATUS_U;
 

#endif
