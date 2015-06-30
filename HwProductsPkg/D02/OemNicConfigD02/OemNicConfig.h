/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: OemNicConfig.h


*************************************************/

#ifndef __OEM_NIC_CONFIG_H__
#define __OEM_NIC_CONFIG_H__

#define I2C_SLAVEADDR_EEPROM     (0x52)

#define I2C_OFFSET_EEPROM_ETH0   (0xc00)
#define I2C_OFFSET_EEPROM_ETH1   (I2C_OFFSET_EEPROM_ETH0 + 6)
#define I2C_OFFSET_EEPROM_ETH2   (I2C_OFFSET_EEPROM_ETH1 + 6)
#define I2C_OFFSET_EEPROM_ETH3   (I2C_OFFSET_EEPROM_ETH2 + 6)
#define I2C_OFFSET_EEPROM_ETH4   (I2C_OFFSET_EEPROM_ETH3 + 6)
#define I2C_OFFSET_EEPROM_ETH5   (I2C_OFFSET_EEPROM_ETH4 + 6)
#define I2C_OFFSET_EEPROM_ETH6   (I2C_OFFSET_EEPROM_ETH5 + 6)
#define I2C_OFFSET_EEPROM_ETH7   (I2C_OFFSET_EEPROM_ETH6 + 6)


#endif
