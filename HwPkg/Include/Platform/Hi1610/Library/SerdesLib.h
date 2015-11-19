#ifndef _SERDES_LIB_H_
#define _SERDES_LIB_H_


typedef enum hilink0_mode_type
{
    EM_HILINK0_HCCS1_8LANE = 0,    
    EM_HILINK0_PCIE1_8LANE = 2,
    EM_HILINK0_PCIE1_4LANE_PCIE2_4LANE = 3,
    EM_HILINK0_SAS2_8LANE = 4,
    EM_HILINK0_HCCS1_8LANE_16,
    EM_HILINK0_HCCS1_8LANE_32,
}hilink0_mode_type_e;

typedef enum hilink1_mode_type
{
    EM_HILINK1_SAS2_1LANE = 0,
    EM_HILINK1_HCCS0_8LANE = 1,
	EM_HILINK1_PCIE0_8LANE = 2,	
	EM_HILINK1_HCCS0_8LANE_16,
	EM_HILINK1_HCCS0_8LANE_32,
}hilink1_mode_type_e;

typedef enum hilink2_mode_type
{
	EM_HILINK2_PCIE2_8LANE = 0,
	EM_HILINK2_SAS0_8LANE = 2,
}hilink2_mode_type_e;

typedef enum hilink5_mode_type
{
    EM_HILINK5_PCIE3_4LANE = 0,
    EM_HILINK5_PCIE2_2LANE_PCIE3_2LANE = 1,
	EM_HILINK5_SAS1_4LANE = 2,
	
}hilink5_mode_type_e;

typedef enum board_type_em
{
	EM_32CORE_EVB_BOARD = 0,
	EM_16CORE_EVB_BOARD = 1,
	EM_V2R1CO5_BORAD = 2,
	EM_OTHER_BORAD			
}board_type_e;


typedef struct serdes_param
{
	hilink0_mode_type_e hilink0_mode;
	hilink1_mode_type_e hilink1_mode;
	hilink2_mode_type_e hilink2_mode;
	UINT32 hilink3_mode;
	UINT32 hilink4_mode;
	hilink5_mode_type_e hilink5_mode;
    UINT32 hilink6_mode;
    UINT32 use_ssc;
    //board_type_e board_type;
}serdes_param_t;


#define SERDES_INVALID_MACRO_ID  0xFFFFFFFF
#define SERDES_INVALID_LANE_NUM  0xFFFFFFFF

typedef struct {
    UINT32 MacroId;
    UINT32 DsNum;
} SERDES_POLARITY_INVERT;


EFI_STATUS OemGetSerdesParam (serdes_param_t *Param);
extern SERDES_POLARITY_INVERT gSerdesPolarityTxDesc[];
extern SERDES_POLARITY_INVERT gSerdesPolarityRxDesc[];
UINT32 GetEthType(UINT8 EthChannel);

EFI_STATUS
EfiSerdesInitWrap (VOID);

//uniBIOS__l00228991_start DTS2015042210118 2015-4-22 20:06:34

void SRE_SerdesEnableCTLEDFE(UINT32 macro, UINT32 lane, UINT32 ulDsCfg);
//uniBIOS__l00228991_end DTS2015042210118 2015-4-22 20:06:34

//uniBIOS_l00306713_000_start 2015-3-19 17:37:06

//EYE test
UINT32 serdes_eye_test(UINT32 uwMacroId, UINT32 uwDsNum, UINT32 eyemode, UINT32 scanwindowvalue, UINT32 uwRateData);

UINT32 Serdes_ReadBert(UINT32   ulMacroId , UINT32   ulDsNum);

//PRBS test
int serdes_prbs_test(UINT8 macro, UINT8 lane, UINT8 prbstype);

int serdes_prbs_test_cancle(UINT8 macro,UINT8 lane);

//CTLE/DFE
void serdes_ctle_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_ctle_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_ctle_dfe_reset(UINT32 macro,UINT32 lane);
//uniBIOS_l00306713_000_end   2015-3-19 17:37:06
int serdes_reset(UINT32 macro);
int serdes_release_reset(UINT32 macro);
void Custom_Wave(UINT32 macro,UINT32 lane,UINT32 mode);
void serdes_ffe_show(UINT32 macro,UINT32 lane);
void serdes_dfe_show(UINT32 macro,UINT32 lane);
int  serdes_read_bert(UINT8 macro, UINT8 lane);
void  serdes_clean_bert(UINT8 macro, UINT8 lane);
int  serdes_get_four_point_eye_diagram(UINT32 macro, UINT32 lane,UINT32 eyemode, UINT32 data_rate);
void serdes_release_mcu(UINT32 macro,UINT32 val);
int hilink_write(UINT32 macro, UINT32 reg, UINT32 value);
int hilink_read(UINT32 macro, UINT32 reg, UINT32 *value);
int serdes_tx_to_rx_parallel_loopback(UINT8 macro,UINT8 lane,UINT8 val);//TXRXPARLPBKEN
int serdes_rx_to_tx_parallel_loopback(UINT8 macro,UINT8 lane,UINT8 val);
int serdes_tx_to_rx_serial_loopback(UINT8 macro,UINT8 lane,UINT8 val);
void serdes_ctle_show(UINT32 macro,UINT32 lane);
int serdes_cs_write(UINT32 macro,UINT32 cs_num,UINT32 reg_num,UINT32 bit_high,UINT32 bit_low,UINT32 value);
UINT32 serdes_cs_read(UINT32 macro,UINT32 cs_num,UINT32 reg_num);
int serdes_ds_write(UINT32 macro,UINT32 ds_num,UINT32 ds_index,UINT32 reg_num,UINT32 bit_high,UINT32 bit_low,UINT32 value);
int serdes_ds_read(UINT32 macro,UINT32 ds_num,UINT32 ds_index,UINT32 reg_num);
int report_serdes_mux(void);
int serdes_key_reg_show(UINT32 macro);
void serdes_state_show(UINT32 macro);
UINT32 Serdes_ReadBert(UINT32   ulMacroId , UINT32   ulDsNum);

#endif
