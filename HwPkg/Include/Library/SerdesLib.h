#ifndef _SERDES_LIB_H_
#define _SERDES_LIB_H_

/*************************************************************
       新增加的宏定义begin
***********************************************************/
/*l00290354 add serdes struct
注意当hilink0为EM_HILINK0_PCIE1_4LANE_PCIE2_4LANE模式
hilink2必须为EM_HILINK2_SAS0_8LANE
*/
typedef enum hilink0_mode_type
{
	EM_HILINK0_PCIE1_8LANE = 0,
	EM_HILINK0_PCIE1_4LANE_PCIE2_4LANE = 1,
}hilink0_mode_type_e;

typedef enum hilink1_mode_type
{
	EM_HILINK1_PCIE0_8LANE = 0,
	EM_HILINK1_HCCS_8LANE = 1,
}hilink1_mode_type_e;

typedef enum hilink2_mode_type
{
	EM_HILINK2_PCIE2_8LANE = 0,
	EM_HILINK2_SAS0_8LANE = 1,
}hilink2_mode_type_e;

typedef enum hilink3_mode_type
{
	EM_HILINK3_GE_4LANE = 0,
	EM_HILINK3_GE_2LANE_XGE_2LANE = 1, //lane0,lane1-ge,lane2,lane3 xge
}hilink3_mode_type_e;


typedef enum hilink4_mode_type
{
	EM_HILINK4_GE_4LANE = 0,
	EM_HILINK4_XGE_4LANE = 1, 
}hilink4_mode_type_e;

typedef enum hilink5_mode_type
{
	EM_HILINK5_SAS1_4LANE = 0,
	EM_HILINK5_PCIE3_4LANE = 1, 
}hilink5_mode_type_e;


typedef struct serdes_param
{
	hilink0_mode_type_e hilink0_mode;
	hilink1_mode_type_e hilink1_mode;
	hilink2_mode_type_e hilink2_mode;
	hilink3_mode_type_e hilink3_mode;
	hilink4_mode_type_e hilink4_mode;
	hilink5_mode_type_e hilink5_mode;
}serdes_param_t;


#define SERDES_INVALID_MACRO_ID  0xFFFFFFFF
#define SERDES_INVALID_LANE_NUM  0xFFFFFFFF

typedef struct {
    UINT32 MacroId;
    UINT32 DsNum;
} SERDES_POLARITY_INVERT;

// 以下并非SerdesLib所产生，而是SerdesLib需要使用的外部接口，在OemMiscLib里生成
EFI_STATUS OemGetSerdesParam (serdes_param_t *Param);
extern SERDES_POLARITY_INVERT gSerdesPolarityTxDesc[];
extern SERDES_POLARITY_INVERT gSerdesPolarityRxDesc[];
UINT32 GetEthType(UINT8 EthChannel);

EFI_STATUS
EfiSerdesInitWrap (VOID);

void serdes_state_show();
void SRE_SerdesEnableCTLEDFE(UINT32 macro, UINT32 lane, UINT32 ulDsCfg);

UINT32 serdes_eye_test(UINT32 uwMacroId, UINT32 uwDsNum, UINT32 eyemode, UINT32 scanwindowvalue, UINT32 uwRateData);

UINT32 Serdes_ReadBert(UINT32   ulMacroId , UINT32   ulDsNum);

//PRBS test
UINT32 serdes_prbs_test(UINT32   ulMacroId , UINT32   ulDsNum,UINT32 PrbsType);

UINT32 serdes_prbs_test_cancle(UINT32  ulMacroId,UINT32 ulDsNum);

//CTLE/DFE
void serdes_ctle_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_ctle_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_close(UINT32 macro,UINT32 lane);

void serdes_dfe_adaptation_open(UINT32 macro,UINT32 lane);

void serdes_ctle_dfe_reset(UINT32 macro,UINT32 lane);
//uniBIOS_l00306713_000_end   2015-3-19 17:37:06

#endif
