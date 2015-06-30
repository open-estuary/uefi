/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:64z4jYnYa5t1KtRL8a/vnMxg4uGttU/wzF06xcyNtiEfsIe4UpyXkUSy93j7U7XZDdqx2rNx
p+25Dla32ZW7onUkKu5TWq4Q5zG307zi/cnai6B1wCItBbdinZKFc8xhcVSamK3UgdVdRbMX
ZVq1lOS7yHOYu1ExODZDpXgVT/14Le+hnmBNQq7NyJS4ikhO828ClmIQM2xDxyKSgAJl+Ehm
YEaszbmtKFojak2xSUnMNEdTDf0oCGK+Y2jCbifo093+xF03IzufiBAME0HszA==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/** @file
  Implement EFI RealTimeClock runtime services via RTC Lib.

  Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>

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
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/RealTimeClockLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>

#include <Protocol/RealTimeClock.h>

#include <Guid/GlobalVariable.h>
#include <Guid/EventGroup.h>

#include <Library/ArmArchTimer.h>

// Define EPOCH (1970-JANUARY-01) in the Julian Date representation
#define EPOCH_JULIAN_DATE                               2440588

// Seconds per unit
#define SEC_PER_MIN                                     ((UINTN)    60)
#define SEC_PER_HOUR                                    ((UINTN)  3600)
#define SEC_PER_DAY                                     ((UINTN) 86400)

#define SEC_PER_MONTH                                   ((UINTN)  2,592,000)
#define SEC_PER_YEAR                                    ((UINTN) 31,536,000)


STATIC CONST CHAR16           mTimeZoneVariableName[] = L"PV660VirtualRtcTimeZone";
STATIC CONST CHAR16           mDaylightVariableName[] = L"PV660VirtualRtcDaylight";
STATIC EFI_EVENT              mRtcVirtualAddrChangeEvent;
STATIC EFI_RUNTIME_SERVICES   *mRT;

// g00179230: 支持虚拟的settime接口
STATIC INTN mEpochDiff = 0;

/**
  Converts Epoch seconds (elapsed since 1970 JANUARY 01, 00:00:00 UTC) to EFI_TIME
 **/
VOID
EpochToEfiTime (
  IN  UINTN     EpochSeconds,
  OUT EFI_TIME  *Time
  )
{
  UINTN         a;
  UINTN         b;
  UINTN         c;
  UINTN         d;
  UINTN         g;
  UINTN         j;
  UINTN         m;
  UINTN         y;
  UINTN         da;
  UINTN         db;
  UINTN         dc;
  UINTN         dg;
  UINTN         hh;
  UINTN         mm;
  UINTN         ss;
  UINTN         J;

  J  = (EpochSeconds / 86400) + 2440588;
  j  = J + 32044;
  g  = j / 146097;
  dg = j % 146097;
  c  = (((dg / 36524) + 1) * 3) / 4;
  dc = dg - (c * 36524);
  b  = dc / 1461;
  db = dc % 1461;
  a  = (((db / 365) + 1) * 3) / 4;
  da = db - (a * 365);
  y  = (g * 400) + (c * 100) + (b * 4) + a;
  m  = (((da * 5) + 308) / 153) - 2;
  d  = da - (((m + 4) * 153) / 5) + 122;

  Time->Year  = y - 4800 + ((m + 2) / 12);
  Time->Month = ((m + 2) % 12) + 1;
  Time->Day   = d + 1;

  ss = EpochSeconds % 60;
  a  = (EpochSeconds - ss) / 60;
  mm = a % 60;
  b = (a - mm) / 60;
  hh = b % 24;

  Time->Hour        = hh;
  Time->Minute      = mm;
  Time->Second      = ss;
  Time->Nanosecond  = 0;

}

/**
  Converts EFI_TIME to Epoch seconds (elapsed since 1970 JANUARY 01, 00:00:00 UTC)
 **/
UINTN
EfiTimeToEpoch (
  IN  EFI_TIME  *Time
  )
{
  UINTN a;
  UINTN y;
  UINTN m;
  UINTN JulianDate;  // Absolute Julian Date representation of the supplied Time
  UINTN EpochDays;   // Number of days elapsed since EPOCH_JULIAN_DAY
  UINTN EpochSeconds;

  a = (14 - Time->Month) / 12 ;
  y = Time->Year + 4800 - a;
  m = Time->Month + (12*a) - 3;

  JulianDate = Time->Day + ((153*m + 2)/5) + (365*y) + (y/4) - (y/100) + (y/400) - 32045;

  ASSERT (JulianDate >= EPOCH_JULIAN_DATE);
  EpochDays = JulianDate - EPOCH_JULIAN_DATE;

  EpochSeconds = (EpochDays * SEC_PER_DAY) + ((UINTN)Time->Hour * SEC_PER_HOUR) + (Time->Minute * SEC_PER_MIN) + Time->Second;

  return EpochSeconds;
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
  UINT64      Temp;
  UINT32      EpochSeconds;
  INT16       TimeZone = 0;
  UINT8       Daylight = 0;
  UINTN       Size;

  // Ensure Time is a valid pointer
  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ArmArchTimerReadReg(CntPct,&Temp); /*by z00201473 获取寄存器cntpct_el0的值，并计算得出秒数*/
  // 通过PCD指定核内timer的频率
  // UINT32 force convertion for PC-LINT
  EpochSeconds = mEpochDiff + Temp / (UINT32) PcdGet32(PcdArmArchTimerFreqInHz);  /*by z00201473 转换成秒*/

  // Get the current time zone information from non-volatile storage
  Size = sizeof (TimeZone);
  Status = mRT->GetVariable (
                  (CHAR16 *)mTimeZoneVariableName,
                  &gEfiCallerIdGuid,
                  NULL,
                  &Size,
                  (VOID *)&TimeZone
                  );

  if (EFI_ERROR (Status)) {
    ASSERT(Status != EFI_INVALID_PARAMETER);
    ASSERT(Status != EFI_BUFFER_TOO_SMALL);

    if (Status != EFI_NOT_FOUND)
      goto EXIT;

    // The time zone variable does not exist in non-volatile storage, so create it.
    Time->TimeZone = EFI_UNSPECIFIED_TIMEZONE;
    // Store it
    Status = mRT->SetVariable (
                    (CHAR16 *)mTimeZoneVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    Size,
                    (VOID *)&(Time->TimeZone)
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_ERROR,
        "LibGetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
        mTimeZoneVariableName,
        Status
        ));
      goto EXIT;
    }
  } else {
    // Got the time zone
    Time->TimeZone = TimeZone;

    // Check TimeZone bounds:   -1440 to 1440 or 2047
    if (((Time->TimeZone < -1440) || (Time->TimeZone > 1440))
        && (Time->TimeZone != EFI_UNSPECIFIED_TIMEZONE)) {
      Time->TimeZone = EFI_UNSPECIFIED_TIMEZONE;
    }

    // Adjust for the correct time zone
    if (Time->TimeZone != EFI_UNSPECIFIED_TIMEZONE) {
      EpochSeconds += Time->TimeZone * SEC_PER_MIN;
    }
  }

  // Get the current daylight information from non-volatile storage
  Size = sizeof (Daylight);
  Status = mRT->GetVariable (
                  (CHAR16 *)mDaylightVariableName,
                  &gEfiCallerIdGuid,
                  NULL,
                  &Size,
                  (VOID *)&Daylight
                  );

  if (EFI_ERROR (Status)) {
    ASSERT(Status != EFI_INVALID_PARAMETER);
    ASSERT(Status != EFI_BUFFER_TOO_SMALL);

    if (Status != EFI_NOT_FOUND)
      goto EXIT;

    // The daylight variable does not exist in non-volatile storage, so create it.
    Time->Daylight = 0;
    // Store it
    Status = mRT->SetVariable (
                    (CHAR16 *)mDaylightVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    Size,
                    (VOID *)&(Time->Daylight)
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_ERROR,
        "LibGetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
        mDaylightVariableName,
        Status
        ));
      goto EXIT;
    }
  } else {
    // Got the daylight information
    Time->Daylight = Daylight;

    // Adjust for the correct period
    if ((Time->Daylight & EFI_TIME_IN_DAYLIGHT) == EFI_TIME_IN_DAYLIGHT) {
      // Convert to adjusted time, i.e. spring forwards one hour
      EpochSeconds += SEC_PER_HOUR;
    }
  }

  // Convert from internal 32-bit time to UEFI time
  EpochToEfiTime (EpochSeconds, Time);

  // Update the Capabilities info
  if (Capabilities != NULL) {
    Capabilities->Resolution  = 1;
    // Accuracy in ppm multiplied by 1,000,000, e.g. for 50ppm set 50,000,000
    // g00179230: PPM的含义还不是太清楚, pl031是300,000,000，此处直接用时钟频率
    Capabilities->Accuracy    = PcdGet32 (PcdArmArchTimerFreqInHz);
    // FALSE: Setting the time does not clear the values below the resolution level
    Capabilities->SetsToZero  = FALSE;
  }

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
  EFI_STATUS  Status;
  UINTN       EpochSeconds;
  UINTN       Temp;

  // Check the input parameters are within the range specified by UEFI
  if ((Time->Year   < 1900) ||
       (Time->Year   > 9999) ||
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

  // Because the PL031 is a 32-bit counter counting seconds,
  // the maximum time span is just over 136 years.
  // Time is stored in Unix Epoch format, so it starts in 1970,
  // Therefore it can not exceed the year 2106.
  if ((Time->Year < 1970) || (Time->Year >= 2106)) {
    Status = EFI_UNSUPPORTED;
    goto EXIT;
  }

  EpochSeconds = EfiTimeToEpoch (Time);

  // Adjust for the correct time zone, i.e. convert to UTC time zone
  if (Time->TimeZone != EFI_UNSPECIFIED_TIMEZONE) {
    EpochSeconds -= Time->TimeZone * SEC_PER_MIN;
  }

  // TODO: Automatic Daylight activation

  // Adjust for the correct period
  if ((Time->Daylight & EFI_TIME_IN_DAYLIGHT) == EFI_TIME_IN_DAYLIGHT) {
    // Convert to un-adjusted time, i.e. fall back one hour
    EpochSeconds -= SEC_PER_HOUR;
  }

  ArmArchTimerReadReg(CntPct,&Temp); /*by z00201473 获取寄存器cntpct_el0的值，并计算得出秒数*/
  // 通过PCD指定核内timer的频率
  // UINT32 force convertion for PC-LINT
  mEpochDiff = EpochSeconds - Temp / (UINT32) PcdGet32(PcdArmArchTimerFreqInHz);  /*by z00201473 转换成秒*/

  // The accesses to Variable Services can be very slow, because we may be writing to Flash.
  // Do this after having set the RTC.

  // Save the current time zone information into non-volatile storage
  Status = mRT->SetVariable (
                  (CHAR16 *)mTimeZoneVariableName,
                  &gEfiCallerIdGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (Time->TimeZone),
                  (VOID *)&(Time->TimeZone)
                  );
  if (EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_ERROR,
        "LibSetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
        mTimeZoneVariableName,
        Status
        ));
    goto EXIT;
  }

  // Save the current daylight information into non-volatile storage
  Status = mRT->SetVariable (
                  (CHAR16 *)mDaylightVariableName,
                  &gEfiCallerIdGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(Time->Daylight),
                  (VOID *)&(Time->Daylight)
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      EFI_D_ERROR,
      "LibSetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
      mDaylightVariableName,
      Status
      ));
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
  EfiConvertPointer (0x0, (VOID**)&mRT);
  return;
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

  mRT = gRT;

  // Install the protocol
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiRealTimeClockArchProtocolGuid,  NULL,
                  NULL
                 );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  LibRtcVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mRtcVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
