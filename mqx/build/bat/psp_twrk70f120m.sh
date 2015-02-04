#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mqx/source/include/mqx_str.h" "${OUTPUTDIR}/mqx_str.h"
cp -f "${ROOTDIR}/mqx/source/include/watchdog.h" "${OUTPUTDIR}/watchdog.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/cpu/MK70F12.h" "${OUTPUTDIR}/MK70F12.h"
cp -f "${ROOTDIR}/mqx/source/include/edserial.h" "${OUTPUTDIR}/edserial.h"
cp -f "${ROOTDIR}/mqx/source/include/name.h" "${OUTPUTDIR}/name.h"
cp -f "${ROOTDIR}/mqx/source/include/ipc.h" "${OUTPUTDIR}/ipc.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_macros.h" "${OUTPUTDIR}/mqx_macros.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_math.h" "${OUTPUTDIR}/psp_math.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_cpudef.h" "${OUTPUTDIR}/mqx_cpudef.h"
cp -f "${ROOTDIR}/mqx/source/include/fio.h" "${OUTPUTDIR}/fio.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_comp.h" "${OUTPUTDIR}/psp_comp.h"
cp -f "${ROOTDIR}/mqx/source/include/klog.h" "${OUTPUTDIR}/klog.h"
cp -f "${ROOTDIR}/mqx/source/include/lwmsgq.h" "${OUTPUTDIR}/lwmsgq.h"
cp -f "${ROOTDIR}/config/common/small_ram_config.h" "${OUTPUTDIR}/small_ram_config.h"
cp -f "${ROOTDIR}/mqx/source/include/lwsem.h" "${OUTPUTDIR}/lwsem.h"
cp -f "${ROOTDIR}/mqx/source/include/timer.h" "${OUTPUTDIR}/timer.h"
cp -f "${ROOTDIR}/mqx/source/include/sem.h" "${OUTPUTDIR}/sem.h"
cp -f "${ROOTDIR}/mqx/source/include/psptypes_legacy.h" "${OUTPUTDIR}/psptypes_legacy.h"
cp -f "${ROOTDIR}/mqx/source/include/ioctl.h" "${OUTPUTDIR}/ioctl.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_inc.h" "${OUTPUTDIR}/mqx_inc.h"
cp -f "${ROOTDIR}/mqx/source/include/ipc_pcb.h" "${OUTPUTDIR}/ipc_pcb.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp.h" "${OUTPUTDIR}/psp.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx.h" "${OUTPUTDIR}/mqx.h"
cp -f "${ROOTDIR}/mqx/source/include/io_rev.h" "${OUTPUTDIR}/io_rev.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/kinetis_mpu.h" "${OUTPUTDIR}/kinetis_mpu.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_supp.h" "${OUTPUTDIR}/psp_supp.h"
cp -f "${ROOTDIR}/mqx/source/tad/tad.h" "${OUTPUTDIR}/tad.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/cpu/kinetis_fcan.h" "${OUTPUTDIR}/kinetis_fcan.h"
cp -f "${ROOTDIR}/mqx/source/include/unicode.h" "${OUTPUTDIR}/unicode.h"
cp -f "${ROOTDIR}/mqx/source/include/partition.h" "${OUTPUTDIR}/partition.h"
cp -f "${ROOTDIR}/mqx/source/include/lwtimer.h" "${OUTPUTDIR}/lwtimer.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/cortex.h" "${OUTPUTDIR}/cortex.h"
cp -f "${ROOTDIR}/mqx/source/include/ipc_pcbv.h" "${OUTPUTDIR}/ipc_pcbv.h"
cp -f "${ROOTDIR}/mqx/source/include/event.h" "${OUTPUTDIR}/event.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_cpu.h" "${OUTPUTDIR}/psp_cpu.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_cpudef.h" "${OUTPUTDIR}/psp_cpudef.h"
cp -f "${ROOTDIR}/mqx/source/include/posix.h" "${OUTPUTDIR}/posix.h"
cp -f "${ROOTDIR}/mqx/source/include/log.h" "${OUTPUTDIR}/log.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_time.h" "${OUTPUTDIR}/psp_time.h"
cp -f "${ROOTDIR}/mqx/source/include/queue.h" "${OUTPUTDIR}/queue.h"
cp -f "${ROOTDIR}/mqx/source/include/mutex.h" "${OUTPUTDIR}/mutex.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/nvic.h" "${OUTPUTDIR}/nvic.h"
cp -f "${ROOTDIR}/mqx/source/include/io.h" "${OUTPUTDIR}/io.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_rev.h" "${OUTPUTDIR}/psp_rev.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psp_abi.h" "${OUTPUTDIR}/psp_abi.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/psptypes.h" "${OUTPUTDIR}/psptypes.h"
cp -f "${ROOTDIR}/mqx/source/include/mmu.h" "${OUTPUTDIR}/mmu.h"
cp -f "${ROOTDIR}/mqx/source/include/lwevent.h" "${OUTPUTDIR}/lwevent.h"
cp -f "${ROOTDIR}/mqx/source/include/charq.h" "${OUTPUTDIR}/charq.h"
cp -f "${ROOTDIR}/mqx/source/include/lwlog.h" "${OUTPUTDIR}/lwlog.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_cnfg.h" "${OUTPUTDIR}/mqx_cnfg.h"
cp -f "${ROOTDIR}/mqx/source/include/gen_rev.h" "${OUTPUTDIR}/gen_rev.h"
cp -f "${ROOTDIR}/mqx/source/include/lwmem.h" "${OUTPUTDIR}/lwmem.h"
cp -f "${ROOTDIR}/mqx/source/include/eds.h" "${OUTPUTDIR}/eds.h"
cp -f "${ROOTDIR}/mqx/source/include/message.h" "${OUTPUTDIR}/message.h"
cp -f "${ROOTDIR}/mqx/source/include/pcb.h" "${OUTPUTDIR}/pcb.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_ioc.h" "${OUTPUTDIR}/mqx_ioc.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/kinetis.h" "${OUTPUTDIR}/kinetis.h"


# kds files
if [ "${TOOL}" = "kds" ]; then
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/compiler/gcc_arm/asm_mac.h" "${OUTPUTDIR}/asm_mac.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/compiler/gcc_arm/comp.h" "${OUTPUTDIR}/comp.h"
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/compiler/gcc_arm/asm_mac.h" "${OUTPUTDIR}/asm_mac.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_m/compiler/gcc_arm/comp.h" "${OUTPUTDIR}/comp.h"
:
fi


