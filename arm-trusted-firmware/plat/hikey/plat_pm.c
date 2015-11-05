/*
 * Copyright (c) 2014-2015, Linaro Ltd and Contributors. All rights reserved.
 * Copyright (c) 2014-2015, Hisilicon Ltd and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <arch_helpers.h>
#include <arm_gic.h>
#include <debug.h>
#include <cci400.h>
#include <errno.h>
#include <gic_v2.h>
#include <hi6220.h>
#include <hisi_ipc.h>
#include <hisi_pwrc.h>
#include <mmio.h>
#include <platform.h>
#include <platform_def.h>
#include <psci.h>

#include "hikey_def.h"
#include "hikey_private.h"

#define PLAT_SOC_SUSPEND_STATE	0x4

static int32_t hikey_do_plat_actions(uint32_t afflvl, uint32_t state)
{
	assert(afflvl <= MPIDR_AFFLVL1);

	if (state != PSCI_STATE_OFF)
		return -EAGAIN;

	return 0;
}

int32_t hikey_affinst_on(uint64_t mpidr,
			 uint64_t sec_entrypoint,
			 uint32_t afflvl,
			 uint32_t state)
{
	int cpu, cluster;

	cluster = (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFF1_SHIFT;
	cpu = mpidr & MPIDR_CPU_MASK;

	VERBOSE("#%s, mpidr:%llx, afflvl:%x, state:%x\n", __func__, mpidr, afflvl, state);

	/* directly return for power on */
	if (state == PSCI_STATE_ON)
		return PSCI_E_SUCCESS;

	switch (afflvl) {
	case MPIDR_AFFLVL0:
		hisi_pwrc_set_core_bx_addr(cpu, cluster, sec_entrypoint);
		hisi_ipc_cpu_on(cpu, cluster);
		break;

	case MPIDR_AFFLVL1:
		hisi_ipc_cluster_on(cpu, cluster);
		break;
	}

	return PSCI_E_SUCCESS;
}


static void hikey_affinst_off(uint32_t afflvl, uint32_t state)
{
	unsigned int mpidr = read_mpidr_el1();
	int cpu, cluster;

	cluster = (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFF1_SHIFT;
	cpu = mpidr & MPIDR_CPU_MASK;

	if (hikey_do_plat_actions(afflvl, state) == -EAGAIN)
		return;

	switch (afflvl) {
	case MPIDR_AFFLVL1:
		hisi_ipc_spin_lock(HISI_IPC_SEM_CPUIDLE);
		cci_disable_cluster_coherency(mpidr);
		hisi_ipc_spin_unlock(HISI_IPC_SEM_CPUIDLE);

		hisi_ipc_cluster_off(cpu, cluster);
		break;

	case MPIDR_AFFLVL0:
		arm_gic_cpuif_deactivate();
		hisi_ipc_cpu_off(cpu, cluster);
		break;
	}

	return;
}

static void hikey_affinst_suspend(uint64_t sec_entrypoint,
				  uint32_t afflvl,
				  uint32_t state)
{
	unsigned int mpidr = read_mpidr_el1();
	int cpu, cluster;

	cluster = (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFF1_SHIFT;
	cpu = mpidr & MPIDR_CPU_MASK;

	if (hikey_do_plat_actions(afflvl, state) == -EAGAIN)
		return;

	switch (afflvl) {
	case MPIDR_AFFLVL1:

		hisi_ipc_spin_lock(HISI_IPC_SEM_CPUIDLE);
		cci_disable_cluster_coherency(mpidr);
		hisi_ipc_spin_unlock(HISI_IPC_SEM_CPUIDLE);

		if (psci_get_suspend_stateid() == PLAT_SOC_SUSPEND_STATE) {
			hisi_pwrc_set_cluster_wfi(1);
			hisi_pwrc_set_cluster_wfi(0);
			hisi_ipc_psci_system_off();
		} else
			hisi_ipc_cluster_suspend(cpu, cluster);

		break;

	case MPIDR_AFFLVL0:

		/* Program the jump address for the target cpu */
		hisi_pwrc_set_core_bx_addr(cpu, cluster, sec_entrypoint);

		arm_gic_cpuif_deactivate();

		if (psci_get_suspend_stateid() != PLAT_SOC_SUSPEND_STATE)
			hisi_ipc_cpu_suspend(cpu, cluster);
		break;
	}

	return;
}

void hikey_affinst_on_finish(uint32_t afflvl, uint32_t state)
{
	unsigned long mpidr;
	int cpu, cluster;

	if (hikey_do_plat_actions(afflvl, state) == -EAGAIN)
		return;

	/* Get the mpidr for this cpu */
	mpidr = read_mpidr_el1();
	cluster = (mpidr & MPIDR_CLUSTER_MASK) >> MPIDR_AFF1_SHIFT;
	cpu = mpidr & MPIDR_CPU_MASK;

	/* Perform the common cluster specific operations */
	if (afflvl != MPIDR_AFFLVL0)
		cci_enable_cluster_coherency(mpidr);

	/* Zero the jump address in the mailbox for this cpu */
	hisi_pwrc_set_core_bx_addr(cpu, cluster, 0);

	if (psci_get_suspend_stateid() == PLAT_SOC_SUSPEND_STATE) {
		arm_gic_setup();
	} else {
		/* Enable the gic cpu interface */
		arm_gic_cpuif_setup();

		/* TODO: This setup is needed only after a cold boot */
		arm_gic_pcpu_distif_setup();
	}

	return;
}

static void hikey_affinst_suspend_finish(uint32_t afflvl,
					 uint32_t state)
{
	hikey_affinst_on_finish(afflvl, state);
	return;
}

static void __dead2 hikey_system_reset(void)
{
	VERBOSE("%s: reset system\n", __func__);

	/* Send the system reset request */
	mmio_write_32(AO_SC_SYS_STAT0, 0x48698284);

	wfi();
	panic();
}

unsigned int hikey_get_sys_suspend_power_state(void)
{
	unsigned int power_state;

	power_state = psci_make_powerstate(PLAT_SOC_SUSPEND_STATE,
			PSTATE_TYPE_POWERDOWN, MPIDR_AFFLVL1);

	return power_state;
}

static const plat_pm_ops_t hikey_plat_pm_ops = {
	.affinst_on		     = hikey_affinst_on,
	.affinst_on_finish	     = hikey_affinst_on_finish,
	.affinst_off		     = hikey_affinst_off,
	.affinst_standby	     = NULL,
	.affinst_suspend	     = hikey_affinst_suspend,
	.affinst_suspend_finish	     = hikey_affinst_suspend_finish,
	.system_off		     = NULL,
	.system_reset		     = hikey_system_reset,
	.get_sys_suspend_power_state = hikey_get_sys_suspend_power_state,
};

int platform_setup_pm(const plat_pm_ops_t **plat_ops)
{
	*plat_ops = &hikey_plat_pm_ops;
	return 0;
}
