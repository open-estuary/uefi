
#ifndef _TIMERDXE_SOC_H_
#define _TIMERDXE_SOC_H_

#define TIMER_SUBCTRL_BASE   PcdGet64(PcdPeriSubctrlAddress)
#define SC_TM_CLKEN0_REG     (0x2050)

#define TIMER_SOC_FREQ       200000000


/****************** Timer32 register addresses offset start ***********************/
#define SRE_HITIMER32_LOAD_OFFSET              (0x0)     
#define SRE_HITIMER32_VALUE_OFFSET             (0x4)     
#define SRE_HITIMER32_CNTL_OFFSET              (0x8)    
#define SRE_HITIMER32_INTC_OFFSET              (0xC)     
#define SRE_HITIMER32_RIS_OFFSET               (0x10)     
#define SRE_HITIMER32_MIS_OFFSET               (0x0014)   
#define SRE_HITIMER32_BGLOAD_OFFSET            (0x18)     
/****************** end ******************************/


#define SRE_HITIMER_ENCLK_SEL_BIT              (1 << 7)   /* Timer enable flag */



#define SRE_HITIMER_DEFAULT_TICKS              100       
#define SRE_HITIMER_RELOAD_TICKS               1         

#define SRE_HITIMER_INT_CLEAR                  (0x01)    
#define SRE_HITIMER_CNTL_ENABLE                (0x80)    
#define SRE_HITIMER_CNTL_MODE                  (0x40)     
#define SRE_HITIMER_CNTL_IRQ_ENABLE            (0x20)    
#define SRE_HITIMER_CNTL_SIZEMODE              (0x2)     

UINT32 gRegBase ;

#define TMERSOC_REG_READ(addr,value)    ((value) = (*(volatile unsigned int *)(UINTN)(addr)))    

#define TMERSOC_REG_WRITE(addr,value)   ((*(volatile unsigned int *)(UINTN)(addr)) = (value))  



#endif

