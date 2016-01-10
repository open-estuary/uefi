/** @file
*
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ArmLib.h>

#include "Smmu.h"

/* Maximum number of context banks per SMMU */
#define SMMU_MAX_CBS		256

#ifdef CONFIG_MM_OUTTER_SHAREABLE
#define SH_DOMAIN               2       /* outer shareable */
#else
#define SH_DOMAIN               3       /* inner shareable */
#endif

#define SMMU_OS_VMID		0
#define SMMU_CB_NUMIRPT		8
#define SMMU_S1CBT_SIZE		0x10000
#define SMMU_S2CBT_SIZE		0x2000
#define SMMU_S1CBT_SHIFT	16
#define SMMU_S2CBT_SHIFT	12


#define SMMU_CTRL_CR0			0x0
#define SMMU_CTRL_ACR			0x8
#define SMMU_CFG_S2CTBAR		0xc
#define SMMU_IDR0			0x10
#define SMMU_IDR1			0x14
#define SMMU_IDR2			0x18
#define SMMU_HIS_GFAR_LOW		0x20
#define SMMU_HIS_GFAR_HIGH		0x24
#define SMMU_RINT_GFSR			0x28
#define SMMU_RINT_GFSYNR		0x2c
#define SMMU_CFG_GFIM			0x30
#define SMMU_CFG_CBF			0x34
#define SMMU_TLBIALL			0x40
#define SMMU_TLBIVMID			0x44
#define SMMU_TLBISID			0x48
#define SMMU_TLBIVA_LOW			0x4c
#define SMMU_TLBIVA_HIGH		0x50
#define SMMU_TLBGSYNC			0x54
#define SMMU_TLBGSTATUS			0x58
#define SMMU_CXTIALL			0x60
#define SMMU_CXTIVMID			0x64
#define SMMU_CXTISID			0x68
#define SMMU_CXTGSYNC			0x6c
#define SMMU_CXTGSTATUS			0x70
#define SMMU_RINT_CB_FSR(n)		(0x100 + ((n) << 2))
#define SMMU_RINT_CB_FSYNR(n)		(0x120 + ((n) << 2))
#define SMMU_HIS_CB_FAR_LOW(n)		(0x140 + ((n) << 3))
#define SMMU_HIS_CB_FAR_HIGH(n)		(0x144 + ((n) << 3))
#define SMMU_CTRL_CB_RESUME(n)		(0x180 + ((n) << 2))
#define SMMU_RINT_CB_FSYNR_MSTID	0x1a0

#define SMMU_CB_S2CR(n)			(0x0  + ((n) << 5))
#define SMMU_CB_CBAR(n)			(0x4  + ((n) << 5))
#define SMMU_CB_S1CTBAR(n)		(0x18 + ((n) << 5))

#define SMMU_S1_MAIR0			0x0
#define SMMU_S1_MAIR1			0x4
#define SMMU_S1_TTBR0_L			0x8
#define SMMU_S1_TTBR0_H			0xc
#define SMMU_S1_TTBR1_L			0x10
#define SMMU_S1_TTBR1_H			0x14
#define SMMU_S1_TTBCR			0x18
#define SMMU_S1_SCTLR			0x1c

#define CFG_CBF_S1_ORGN_WA		(1 << 12)
#define CFG_CBF_S1_IRGN_WA		(1 << 10)
#define CFG_CBF_S1_SHCFG		(SH_DOMAIN << 8)
#define CFG_CBF_S2_ORGN_WA		(1 << 4)
#define CFG_CBF_S2_IRGN_WA		(1 << 2)
#define CFG_CBF_S2_SHCFG		(SH_DOMAIN << 0)

/* Configuration registers */
#define sCR0_CLIENTPD			(1 << 0)
#define sCR0_GFRE			(1 << 1)
#define sCR0_GFIE			(1 << 2)
#define sCR0_GCFGFRE			(1 << 4)
#define sCR0_GCFGFIE			(1 << 5)

#define sACR_WC_EN			(7 << 0)
/*
#if (PAGE_SIZE == SZ_4K)
#define sACR_WC_EN			(7 << 0)
#elif (PAGE_SIZE == SZ_64K)
#define sACR_WC_EN			(3 << 5)
#else
#define sACR_WC_EN			0
#endif
*/

#define ID0_S1TS			(1 << 30)
#define ID0_S2TS			(1 << 29)
#define ID0_NTS				(1 << 28)
#define ID0_PTFS_SHIFT			24
#define ID0_PTFS_MASK			0x2
#define ID0_PTFS_V8_ONLY		0x2
#define ID0_CTTW			(1 << 14)

#define ID2_OAS_SHIFT			8
#define ID2_OAS_MASK			0xff
#define ID2_IAS_SHIFT			0
#define ID2_IAS_MASK			0xff

#define S2CR_TYPE_SHIFT			16
#define S2CR_TYPE_MASK			0x3
#define S2CR_TYPE_TRANS			(0 << S2CR_TYPE_SHIFT)
#define S2CR_TYPE_BYPASS		(1 << S2CR_TYPE_SHIFT)
#define S2CR_TYPE_FAULT			(2 << S2CR_TYPE_SHIFT)
#define S2CR_SHCFG_NS			(3 << 8)
#define S2CR_MTCFG			(1 << 11)
#define S2CR_MEMATTR_OIWB		(0xf << 12)
#define S2CR_MTSH_WEAKEST		(S2CR_SHCFG_NS | \
				S2CR_MTCFG | S2CR_MEMATTR_OIWB)

/* Context bank attribute registers */
#define CBAR_VMID_SHIFT			0
#define CBAR_VMID_MASK			0xff
#define CBAR_S1_BPSHCFG_SHIFT		8
#define CBAR_S1_BPSHCFG_MASK		3
#define CBAR_S1_BPSHCFG_NSH		3
#define CBAR_S1_MEMATTR_SHIFT		12
#define CBAR_S1_MEMATTR_MASK		0xf
#define CBAR_S1_MEMATTR_WB		0xf
#define CBAR_TYPE_SHIFT			16
#define CBAR_TYPE_MASK			0x3
#define CBAR_TYPE_S2_TRANS		(0 << CBAR_TYPE_SHIFT)
#define CBAR_TYPE_S1_TRANS_S2_BYPASS	(1 << CBAR_TYPE_SHIFT)
#define CBAR_TYPE_S1_TRANS_S2_FAULT	(2 << CBAR_TYPE_SHIFT)
#define CBAR_TYPE_S1_TRANS_S2_TRANS	(3 << CBAR_TYPE_SHIFT)
#define CBAR_IRPTNDX_SHIFT		24
#define CBAR_IRPTNDX_MASK		0xff

#define SMMU_CB_BASE(smmu)		((smmu)->s1cbt)
#define SMMU_CB(n)		((n) << 5)

#define sTLBGSTATUS_GSACTIVE		(1 << 0)
#define TLB_LOOP_TIMEOUT		1000000	/* 1s! */

#define SCTLR_WACFG_WA		(2 << 26)
#define SCTLR_RACFG_RA		(2 << 24)
#ifdef CONFIG_P660_2P
#define SCTLR_SHCFG		(1 << 22)
#else
#define SCTLR_SHCFG		(2 << 22)
#endif
#define SCTLR_MTCFG		(1 << 20)
#define SCTLR_MEMATTR_WB	(0xf << 16)
#define SCTLR_MEMATTR_NC	(0x5 << 16)
#define SCTLR_MEMATTR_NGNRE	(0x1 << 16)
#define SCTLR_CACHE_WBRAWA	(SCTLR_WACFG_WA | SCTLR_RACFG_RA | \
			SCTLR_SHCFG | SCTLR_MTCFG | SCTLR_MEMATTR_WB)
#define SCTLR_CACHE_NC		(SCTLR_SHCFG | \
			SCTLR_MTCFG | SCTLR_MEMATTR_NC)
#define SCTLR_CACHE_NGNRE	(SCTLR_SHCFG | \
			SCTLR_MTCFG | SCTLR_MEMATTR_NGNRE)

#define SCTLR_CFCFG			(1 << 7)
#define SCTLR_CFIE			(1 << 6)
#define SCTLR_CFRE			(1 << 5)
#define SCTLR_E				(1 << 4)
#define SCTLR_AFED			(1 << 3)
#define SCTLR_M				(1 << 0)
#define SCTLR_EAE_SBOP			(SCTLR_AFED)

#define RESUME_RETRY			(0 << 0)
#define RESUME_TERMINATE		(1 << 0)

#define TTBCR_TG0_4K			(0 << 14)
#define TTBCR_TG0_64K			(3 << 14)

#define TTBCR_SH0_SHIFT			12
#define TTBCR_SH0_MASK			0x3
#define TTBCR_SH_NS			0
#define TTBCR_SH_OS			2
#define TTBCR_SH_IS			3
#define TTBCR_ORGN0_SHIFT		10
#define TTBCR_IRGN0_SHIFT		8
#define TTBCR_RGN_MASK			0x3
#define TTBCR_RGN_NC			0
#define TTBCR_RGN_WBWA			1
#define TTBCR_RGN_WT			2
#define TTBCR_RGN_WB			3
#define TTBCR_T1SZ_SHIFT		16
#define TTBCR_T0SZ_SHIFT		0
#define TTBCR_SZ_MASK			0xf

#define MAIR_ATTR_SHIFT(n)		((n) << 3)
#define MAIR_ATTR_MASK			0xff
#define MAIR_ATTR_DEVICE		0x04
#define MAIR_ATTR_NC			0x44
#define MAIR_ATTR_WBRWA			0xff
#define MAIR_ATTR_IDX_NC		0
#define MAIR_ATTR_IDX_CACHE		1
#define MAIR_ATTR_IDX_DEV		2

#define FSR_MULTI		(1 << 31)
#define FSR_EF			(1 << 4)
#define FSR_PF			(1 << 3)
#define FSR_AFF			(1 << 2)
#define FSR_TF			(1 << 1)
#define FSR_IGN			(FSR_AFF)
#define FSR_FAULT		(FSR_MULTI | FSR_EF | FSR_PF | FSR_TF | FSR_IGN)

#define FSYNR0_ASID(n)			(0xff & ((n) >> 24))
#define FSYNR0_VMID(n)			(0xff & ((n) >> 16))
#define FSYNR0_WNR			(1 << 4)
#define FSYNR0_SS			(1 << 2)
#define FSYNR0_CF			(1 << 0)

#define SMMU_FEAT_COHERENT_WALK		(1 << 0)
#define SMMU_FEAT_STREAM_MATCH		(1 << 1)
#define SMMU_FEAT_TRANS_S1		(1 << 2)
#define SMMU_FEAT_TRANS_S2		(1 << 3)
#define SMMU_FEAT_TRANS_NESTED		(1 << 4)

static UINT32 hisi_bypass_vmid = 0xff;

VOID writel_relaxed (UINT32 Value, UINTN Base)
{
  MmioWrite32 (Base, Value);
}

UINT32 readl_relaxed (UINTN Base)
{
  return MmioRead32 (Base);
}

/* Wait for any pending TLB invalidations to complete */
static void hisi_smmu_tlb_sync(SMMU_DEVICE *smmu)
{
	int count = 0;
	UINTN gr0_base = smmu->Base;

	writel_relaxed(0, gr0_base + SMMU_TLBGSYNC);
	while (readl_relaxed(gr0_base + SMMU_TLBGSTATUS)
	       & sTLBGSTATUS_GSACTIVE) {
		if (++count == TLB_LOOP_TIMEOUT) {
			DEBUG ((EFI_D_ERROR, "TLB sync timed out -- SMMU (0x%p) may be deadlocked\n", gr0_base));
			return;
		}
		MicroSecondDelay (1);
	}
}


VOID *
SmmuAllocateTable (
  UINTN Size,
  UINTN Alignment
  )
{
  return AllocateAlignedReservedPages (EFI_SIZE_TO_PAGES (Size), Alignment);
}


EFI_STATUS
SmmuInit (
  SMMU_DEVICE       *Smmu
  )
{
  UINT32 Value;
  UINTN  Base = Smmu->Base;
  UINTN  Index;

	/* Clear Global FSR */
	Value = MmioRead32 (Base + SMMU_RINT_GFSR);
	MmioWrite32 (Base + SMMU_RINT_GFSR, Value);

	/* mask all global interrupt */
	MmioWrite32 (Base + SMMU_CFG_GFIM, 0xFFFFFFFF);

	Value  = CFG_CBF_S1_ORGN_WA | CFG_CBF_S1_IRGN_WA | CFG_CBF_S1_SHCFG;
	Value |= CFG_CBF_S2_ORGN_WA | CFG_CBF_S2_IRGN_WA | CFG_CBF_S2_SHCFG;
	MmioWrite32 (Base + SMMU_CFG_CBF, Value);

	/* Clear CB_FSR  */
	for (Index = 0; Index < SMMU_CB_NUMIRPT; Index++) {
		MmioWrite32 (Base + SMMU_RINT_CB_FSR(Index), FSR_FAULT);
	}

  return EFI_SUCCESS;
}

VOID *
SmmuCreateS2Cbt (VOID)
{
  VOID  *Table;
  UINTN  Index;

  Table = SmmuAllocateTable (SMMU_S2CBT_SIZE, LShiftU64 (1, SMMU_S2CBT_SHIFT));
  if (Table == NULL) {
    DEBUG ((EFI_D_ERROR, "[%a]:[%dL] Allocate table failed!\n", __FUNCTION__, __LINE__));
    return NULL;
  }
  ZeroMem (Table, SMMU_S2CBT_SIZE);

  for (Index = 0; Index < SMMU_MAX_CBS; Index++) {
    MmioWrite32 ((UINTN)Table + SMMU_CB_S1CTBAR(Index), 0);
    MmioWrite32 ((UINTN)Table + SMMU_CB_S2CR(Index), S2CR_TYPE_BYPASS);
  }
  return Table;
}

VOID *
SmmuCreateS1Cbt (VOID)
{
  VOID  *Table;
  //UINTN  i;
  //UINT32 DefaultS1Attr = SCTLR_CACHE_WBRAWA;

  Table = SmmuAllocateTable (SMMU_S1CBT_SIZE, LShiftU64 (1, SMMU_S1CBT_SHIFT));
  if (Table == NULL) {
    DEBUG ((EFI_D_ERROR, "[%a]:[%dL] Allocate table failed!\n", __FUNCTION__, __LINE__));
    return NULL;
  }
  ZeroMem (Table, SMMU_S1CBT_SIZE);

  // Use zero as default
/*
  for (i = 0; i < SMMU_MAX_CBS; i++) {
    MmioWrite32 ((UINTN)Table + SMMU_CB(i) + SMMU_S1_SCTLR, DefaultS1Attr);
  }
*/
  return Table;
}

/*
EFI_STATUS
SmmuSetVmidEntry (
  SMMU_DEVICE       *Smmu,
  UINT8             Vmid
  )
{
  UINT32 *Table = Smmu->S2Cbt;

  if (Table == NULL) {
    Table = SmmuCreateS2Cbt ();
    if (Table == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Smmu->S2Cbt = Table;
  }

  
  return EFI_SUCCESS;
}

EFI_STATUS
SmmuSetAsidEntry (
  SMMU_DEVICE       *Smmu,
  UINT8             Vmid,
  UINT8             Asid
  )
{
  return EFI_SUCCESS;
}
*/

EFI_STATUS
SmmuConfigForOS (
  SMMU_DEVICE       *Smmu
  )
{
  VOID*  S2;
  VOID*  S1;
  UINT32 reg;

  S2 = SmmuCreateS2Cbt ();
  if (S2 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Smmu->S2Cbt = (UINTN) S2;

  S1 = SmmuCreateS1Cbt ();
  if (S1 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  MmioWrite32 (Smmu->S2Cbt + SMMU_CB_S1CTBAR(SMMU_OS_VMID), (UINT32) RShiftU64 ((UINT64)S1, SMMU_S1CBT_SHIFT));

  // Force device for VMID 0 ASID 0
  MmioWrite32 ((UINTN)S1 + SMMU_CB(0) + SMMU_S1_SCTLR, SCTLR_CACHE_WBRAWA);
  // Force device for VMID 0 ASID 1
  MmioWrite32 ((UINTN)S1 + SMMU_CB(1) + SMMU_S1_SCTLR, SCTLR_CACHE_NGNRE);

  /*
   * Use the weakest attribute, so no impact stage 1 output attribute.
   */
  reg = CBAR_TYPE_S1_TRANS_S2_BYPASS |
    (CBAR_S1_BPSHCFG_NSH << CBAR_S1_BPSHCFG_SHIFT) |
    (CBAR_S1_MEMATTR_WB << CBAR_S1_MEMATTR_SHIFT);
  MmioWrite32 (Smmu->S2Cbt + SMMU_CB_CBAR(SMMU_OS_VMID), reg);

  /* Mark S2CR as translation */
  reg = S2CR_TYPE_TRANS | S2CR_MTSH_WEAKEST;
  MmioWrite32 (Smmu->S2Cbt + SMMU_CB_S2CR(SMMU_OS_VMID), reg);

  /* Bypass need use another S2CR */
  reg = S2CR_TYPE_BYPASS;
  MmioWrite32 (Smmu->S2Cbt + SMMU_CB_S2CR(hisi_bypass_vmid), reg);

  return EFI_SUCCESS;
}

EFI_STATUS
SmmuFlushCbt (
  SMMU_DEVICE       *Smmu
  )
{
  UINTN Index;

  if (Smmu->S2Cbt == 0) {
    DEBUG ((EFI_D_ERROR, "[%a]:[%dL] S2Cbt is null!\n", __FUNCTION__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }
    
  WriteBackInvalidateDataCacheRange ((VOID *)Smmu->S2Cbt, SMMU_S2CBT_SIZE);
  for (Index = 0; Index < SMMU_MAX_CBS; Index++) {
    UINTN S1Ctb = MmioRead32 (Smmu->S2Cbt + SMMU_CB_S1CTBAR(SMMU_OS_VMID));
    if (S1Ctb) {
      // TODO: shall we really need to flush 64KB? Or 8KB is enough?
      WriteBackInvalidateDataCacheRange ((VOID *)LShiftU64 (S1Ctb, SMMU_S1CBT_SHIFT), SMMU_S1CBT_SIZE);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SmmuEnableTable (
  SMMU_DEVICE       *Smmu
  )
{
  UINT32 reg;
  UINTN  gr0_base = Smmu->Base;

  (VOID) SmmuFlushCbt (Smmu);

	/* Clear Global FSR */
	reg = readl_relaxed(gr0_base + SMMU_RINT_GFSR);
	writel_relaxed(reg, gr0_base + SMMU_RINT_GFSR);

	/* unmask all global interrupt */
	writel_relaxed(0, gr0_base + SMMU_CFG_GFIM);

	reg  = CFG_CBF_S1_ORGN_WA | CFG_CBF_S1_IRGN_WA | CFG_CBF_S1_SHCFG;
	reg |= CFG_CBF_S2_ORGN_WA | CFG_CBF_S2_IRGN_WA | CFG_CBF_S2_SHCFG;
	writel_relaxed(reg, gr0_base + SMMU_CFG_CBF);

  reg = (UINT32) RShiftU64 (Smmu->S2Cbt, SMMU_S2CBT_SHIFT);
  writel_relaxed(reg, gr0_base + SMMU_CFG_S2CTBAR);

  /* Invalidate all TLB, just in case */
  writel_relaxed(0, gr0_base + SMMU_TLBIALL);
  hisi_smmu_tlb_sync(Smmu);

  writel_relaxed(sACR_WC_EN, gr0_base + SMMU_CTRL_ACR);

	/* Enable fault reporting */
	reg  = (sCR0_GFRE | sCR0_GFIE | sCR0_GCFGFRE | sCR0_GCFGFIE);
	reg &= ~sCR0_CLIENTPD;

	writel_relaxed(reg, gr0_base + SMMU_CTRL_CR0);
	ArmDataSyncronizationBarrier ();

  return EFI_SUCCESS;
};

