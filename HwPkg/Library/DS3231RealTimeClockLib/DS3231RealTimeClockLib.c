/*-----------------------------------------------------------------------*/
/*!!Warning: Huawei key information asset. No spread without permission. */
/*CODEMARK:EG4uRhTwMmgcVFBsBnYHCDadN5jJKSuVyxmmaCmKFU6eJEbB2fyHF9weu4/jer/hxLHb+S1e
E0zVg4C3NiZh4bPwYvCdjdVpqIFpLgARMXYjreBf//8/f5Q3OZWBrJs7rTqjWIcFzf/y20MJ
X2UYcj5srUU/fhHsxwVVYBNyX3kjh6JdgjA1nQQ6grY3U3PdPFOdcKemjyxehqS1VTf9Gd4k
5yCXqUqGJwyzyPlE4BTPblK/bo8eU945po+UhDQn+SgJlhpj43QPf4TmWWRnBA==#*/
/*--!!Warning: Deleting or modifying the preceding information is prohibited.--*/

/** @file
  Implement EFI RealTimeClock runtime services via RTC Lib.

  Currently this driver does not support runtime virtual calling.

  Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2011-2013, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
// Use EfiAtRuntime to check stage
#include <Library/UefiRuntimeLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TimerLib.h>
#include <Protocol/RealTimeClock.h>
#include <Library/I2CLib.h>
#include <Library/HwSafeOperationLib.h>
#include "DS3231RealTimeClock.h"

extern I2C_DEVICE gDS3231RtcDevice;

STATIC BOOLEAN       mDS3231Initialized = FALSE;

EFI_STATUS
IdentifyDS3231 (
  VOID
  )
{
  EFI_STATUS    Status;

  Status = EFI_SUCCESS;
  return Status;
}

EFI_STATUS
InitializeDS3231 (
  VOID
  )
{
  EFI_STATUS    Status;
  I2C_DEVICE    Dev;
  RTC_DS3231_CONTROL Temp;
  RTC_DS3231_HOURS   Hours;

  // Prepare the hardware
  (VOID)IdentifyDS3231();

  (VOID) memcpy_s(&Dev, sizeof(Dev), &gDS3231RtcDevice, sizeof(Dev));

  Status = I2CInit(Dev.Socket,Dev.Port,Normal);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  } 
  // Ensure interrupts are masked. We do not want RTC interrupts in UEFI
  Status = I2CRead(&Dev,DS3231_REGADDR_CONTROL,1,&Temp.u8);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  } 
  Temp.bits.INTCN = 0; 
  Status = I2CWrite(&Dev,DS3231_REGADDR_CONTROL,1,&Temp.u8);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  } 
  
  MicroSecondDelay(2000);
  Status = I2CRead(&Dev,DS3231_REGADDR_HOURS,1,&Hours.u8);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  } 
  Hours.bits.Hour24_n = 0;  
  Status = I2CWrite(&Dev,DS3231_REGADDR_HOURS,1,&Hours.u8);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  } 


  mDS3231Initialized = TRUE;

  EXIT:
  return Status;
}

BOOLEAN
IsLeapYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year % 4 == 0) {
    if (Time->Year % 100 == 0) {
      if (Time->Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

BOOLEAN
DayValid (
  IN  EFI_TIME  *Time
  )
{
  INTN  DayOfMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if (Time->Day < 1 ||
      Time->Day > DayOfMonth[Time->Month - 1] ||
      (Time->Month == 2 && (!IsLeapYear (Time) && Time->Day > 28))
     ) {
    return FALSE;
  }

  return TRUE;
}

/**
  Returns the current time and date information, and the time-keeping capabilities
  of the hardware platform.

  @param  Time                   A pointer to storage to receive a snapshot of the current time.
  @param  Capabilities           An optional pointer to a buffer to receive the real time clock
                                 device's capabilities.

  @retval EFI_SUCCESS            The operation completed successfully.
  @retval EFI_INVALID_PARAMETER  Time is NULL.
  @retval EFI_DEVICE_ERROR       The time could not be retrieved due to hardware error.
  @retval EFI_SECURITY_VIOLATION The time could not be retrieved due to an authentication failure.
**/
EFI_STATUS
EFIAPI
LibGetTime (
  OUT EFI_TIME                *Time,
  OUT EFI_TIME_CAPABILITIES   *Capabilities
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       Temp;
  UINT8       BaseHour = 0;
  UINT16      BaseYear = 2000;
  
  I2C_DEVICE    Dev;

  if(EfiAtRuntime()){
    return EFI_UNSUPPORTED;
  }

  // Initialize the hardware if not already done
  if (!mDS3231Initialized) {
    Status = InitializeDS3231 ();
    if (EFI_ERROR (Status)) {
      goto EXIT;
    }
  }

  // Ensure Time is a valid pointer
  if (NULL == Time) {
    Status = EFI_INVALID_PARAMETER;
    goto EXIT;
  }
  
  (VOID) memcpy_s(&Dev, sizeof(Dev), &gDS3231RtcDevice, sizeof(Dev));

  //获取月份
  Status = I2CRead(&Dev,DS3231_REGADDR_MONTH,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  Time->Month = ((Temp>>4)&1)*10+(Temp&0x0F);
  if(Time->Month > 12){
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }
  if(Temp&0x80){
    BaseYear = 2100;   //跨过了一个世纪
  }
  //获取年份
  Status = I2CRead(&Dev,DS3231_REGADDR_YEAR,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  Time->Year  = BaseYear+(Temp>>4) *10 + (Temp&0x0F);
  if((Time->Year - BaseYear) > 99){
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }
  //获取日期
  Status = I2CRead(&Dev,DS3231_REGADDR_DATE,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  Time->Day   = ((Temp>>4)&3) *10 + (Temp&0x0F);;
  if(!DayValid(Time)){
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }
  //获取小时
  Status = I2CRead(&Dev,DS3231_REGADDR_HOURS,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  //BaseHour = 0;
  if((Temp&0x20)&&(Temp&0x10)){
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }else if(Temp&0x20){
    BaseHour = 20;
  }else if(Temp&0x10){
    BaseHour = 10;
  }else {
    BaseHour = 0;
  }
  Time->Hour        = BaseHour + (Temp&0x0F);
  if(Time->Hour > 23){
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }
  //获取分钟
  Status = I2CRead(&Dev,DS3231_REGADDR_MIUTES,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  Time->Minute      = ((Temp>>4)&7) * 10 + (Temp&0x0F);
  if(Time->Minute > 59){
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }
  //获取秒数
  Status = I2CRead(&Dev,DS3231_REGADDR_SECONDS,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  Time->Second      = (Temp>>4) * 10 + (Temp&0x0F);
  if(Time->Second > 59){
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }
  Time->Nanosecond  = 0;

  EXIT:
  return Status;
}


/**
  Sets the current local time and date information.

  @param  Time                  A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
LibSetTime (
  IN  EFI_TIME                *Time
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  I2C_DEVICE    Dev;
  UINT8 Temp;
  UINT16 BaseYear = 2000;

  if(EfiAtRuntime()){
    return EFI_UNSUPPORTED;
  }

  // Check the input parameters are within the range specified by UEFI
  if ((Time->Year   < 2000) ||
       (Time->Year   > 2199) ||
       (Time->Month  < 1   ) ||
       (Time->Month  > 12  ) ||
       (!DayValid (Time)    ) ||
       (Time->Hour   > 23  ) ||
       (Time->Minute > 59  ) ||
       (Time->Second > 59  ) ||
       (Time->Nanosecond > 999999999) ||
       (!((Time->TimeZone == EFI_UNSPECIFIED_TIMEZONE) || ((Time->TimeZone >= -1440) && (Time->TimeZone <= 1440)))) ||
       (Time->Daylight & (~(EFI_TIME_ADJUST_DAYLIGHT | EFI_TIME_IN_DAYLIGHT)))
    ) {
    Status = EFI_INVALID_PARAMETER;
    goto EXIT;
  }

  if ((Time->Year < 2000) || (Time->Year >= 2199)) {
    Status = EFI_UNSUPPORTED;
    goto EXIT;
  }

  // Initialize the hardware if not already done
  if (!mDS3231Initialized) {
    Status = InitializeDS3231 ();
    if (EFI_ERROR (Status)) {
      goto EXIT;
    }
  }
  
  (VOID) memcpy_s(&Dev, sizeof(Dev), &gDS3231RtcDevice, sizeof(Dev));

  Temp = ((Time->Second/10)<<4) | (Time->Second%10);
  MicroSecondDelay(1000);
  Status = I2CWrite(&Dev,DS3231_REGADDR_SECONDS,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }

  Temp = ((Time->Minute/10)<<4) | (Time->Minute%10);
  MicroSecondDelay(1000);
  Status = I2CWrite(&Dev,DS3231_REGADDR_MIUTES,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }

  if(Time->Hour > 19){
    Temp = 2;
  } else if(Time->Hour > 9){
    Temp = 1;
  } else {
    Temp = 0;
  }
  Temp = (Temp << 4) | (Time->Hour%10); //采用24小时制，不必把原有值读出来
  MicroSecondDelay(1000);
  Status = I2CWrite(&Dev,DS3231_REGADDR_HOURS,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  
  Temp = ((Time->Day/10)<<4) | (Time->Day%10);
  MicroSecondDelay(1000);
  Status = I2CWrite(&Dev,DS3231_REGADDR_DATE,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }

  Temp = 0;
  if(Time->Year > 2099){
    Temp = 0x8;
    BaseYear = 2100;
  }
  if(Time->Month > 9){
    Temp |= 0x1;
  }
  Temp = (Temp<<4) | (Time->Month%10);
  MicroSecondDelay(1000);
  Status = I2CWrite(&Dev,DS3231_REGADDR_MONTH,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }

  Temp = (((Time->Year-BaseYear)/10)<<4) | (Time->Year%10);
  MicroSecondDelay(1000);
  Status = I2CWrite(&Dev,DS3231_REGADDR_YEAR,1,&Temp);
  if(EFI_ERROR (Status)){
    goto EXIT;
  }
  
  EXIT:
  return Status;
}


/**
  Returns the current wakeup alarm clock setting.

  @param  Enabled               Indicates if the alarm is currently enabled or disabled.
  @param  Pending               Indicates if the alarm signal is pending and requires acknowledgement.
  @param  Time                  The current alarm setting.

  @retval EFI_SUCCESS           The alarm settings were returned.
  @retval EFI_INVALID_PARAMETER Any parameter is NULL.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a hardware error.

**/
EFI_STATUS
EFIAPI
LibGetWakeupTime (
  OUT BOOLEAN     *Enabled,
  OUT BOOLEAN     *Pending,
  OUT EFI_TIME    *Time
  )
{
  // Not a required feature
  return EFI_UNSUPPORTED;
}


/**
  Sets the system wakeup alarm clock time.

  @param  Enabled               Enable or disable the wakeup alarm.
  @param  Time                  If Enable is TRUE, the time to set the wakeup alarm for.

  @retval EFI_SUCCESS           If Enable is TRUE, then the wakeup alarm was enabled. If
                                Enable is FALSE, then the wakeup alarm was disabled.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be set due to a hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this platform.

**/
EFI_STATUS
EFIAPI
LibSetWakeupTime (
  IN BOOLEAN      Enabled,
  OUT EFI_TIME    *Time
  )
{
  // Not a required feature
  return EFI_UNSUPPORTED;
}



/**
  This is the declaration of an EFI image entry point. This can be the entry point to an application
  written to this specification, an EFI boot service driver, or an EFI runtime driver.

  @param  ImageHandle           Handle that identifies the loaded image.
  @param  SystemTable           System Table for this image.

  @retval EFI_SUCCESS           The operation completed successfully.

**/
EFI_STATUS
EFIAPI
LibRtcInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_HANDLE    Handle;

  // Setup the setters and getters
  gRT->GetTime       = LibGetTime;
  gRT->SetTime       = LibSetTime;
  gRT->GetWakeupTime = LibGetWakeupTime;
  gRT->SetWakeupTime = LibSetWakeupTime;

  // Install the protocol
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiRealTimeClockArchProtocolGuid,  NULL,
                  NULL
                 );

  return Status;
}


/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
LibRtcVirtualNotifyEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  //
  // Only needed if you are going to support the OS calling RTC functions in virtual mode.
  // You will need to call EfiConvertPointer (). To convert any stored physical addresses
  // to virtual address. After the OS transitions to calling in virtual mode, all future
  // runtime calls will be made in virtual mode.
  //
  return;
}
