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
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp.h" "${OUTPUTDIR}/psp.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/core/A5/dispatch_sch.S" "${OUTPUTDIR}/dispatch_sch.S"
cp -f "${ROOTDIR}/mqx/source/include/edserial.h" "${OUTPUTDIR}/edserial.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/cpu/MVF50GS10MK50.h" "${OUTPUTDIR}/MVF50GS10MK50.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/core/A5/boot.S" "${OUTPUTDIR}/boot.S"
cp -f "${ROOTDIR}/mqx/source/include/name.h" "${OUTPUTDIR}/name.h"
cp -f "${ROOTDIR}/mqx/source/include/ipc.h" "${OUTPUTDIR}/ipc.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/core/A5/dispatch_gic.S" "${OUTPUTDIR}/dispatch_gic.S"
cp -f "${ROOTDIR}/mqx/source/include/mqx_macros.h" "${OUTPUTDIR}/mqx_macros.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_cpudef.h" "${OUTPUTDIR}/mqx_cpudef.h"
cp -f "${ROOTDIR}/mqx/source/include/fio.h" "${OUTPUTDIR}/fio.h"
cp -f "${ROOTDIR}/mqx/source/include/klog.h" "${OUTPUTDIR}/klog.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/cortexa5.h" "${OUTPUTDIR}/cortexa5.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/vybrid.h" "${OUTPUTDIR}/vybrid.h"
cp -f "${ROOTDIR}/mqx/source/include/lwmsgq.h" "${OUTPUTDIR}/lwmsgq.h"
cp -f "${ROOTDIR}/config/common/small_ram_config.h" "${OUTPUTDIR}/small_ram_config.h"
cp -f "${ROOTDIR}/mqx/source/include/lwsem.h" "${OUTPUTDIR}/lwsem.h"
cp -f "${ROOTDIR}/mqx/source/include/timer.h" "${OUTPUTDIR}/timer.h"
cp -f "${ROOTDIR}/mqx/source/include/sem.h" "${OUTPUTDIR}/sem.h"
cp -f "${ROOTDIR}/mqx/source/include/psptypes_legacy.h" "${OUTPUTDIR}/psptypes_legacy.h"
cp -f "${ROOTDIR}/mqx/source/include/ioctl.h" "${OUTPUTDIR}/ioctl.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/gic.h" "${OUTPUTDIR}/gic.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_inc.h" "${OUTPUTDIR}/mqx_inc.h"
cp -f "${ROOTDIR}/mqx/source/include/ipc_pcb.h" "${OUTPUTDIR}/ipc_pcb.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx.h" "${OUTPUTDIR}/mqx.h"
cp -f "${ROOTDIR}/mqx/source/include/io_rev.h" "${OUTPUTDIR}/io_rev.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp_comp.h" "${OUTPUTDIR}/psp_comp.h"
cp -f "${ROOTDIR}/mqx/source/tad/tad.h" "${OUTPUTDIR}/tad.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/armv7a.h" "${OUTPUTDIR}/armv7a.h"
cp -f "${ROOTDIR}/mqx/source/include/unicode.h" "${OUTPUTDIR}/unicode.h"
cp -f "${ROOTDIR}/mqx/source/include/partition.h" "${OUTPUTDIR}/partition.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp_cpu.h" "${OUTPUTDIR}/psp_cpu.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/irouter_vybrid.h" "${OUTPUTDIR}/irouter_vybrid.h"
cp -f "${ROOTDIR}/mqx/source/include/lwtimer.h" "${OUTPUTDIR}/lwtimer.h"
cp -f "${ROOTDIR}/mqx/source/include/ipc_pcbv.h" "${OUTPUTDIR}/ipc_pcbv.h"
cp -f "${ROOTDIR}/mqx/source/include/event.h" "${OUTPUTDIR}/event.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp_supp.h" "${OUTPUTDIR}/psp_supp.h"
cp -f "${ROOTDIR}/mqx/source/include/posix.h" "${OUTPUTDIR}/posix.h"
cp -f "${ROOTDIR}/mqx/source/include/log.h" "${OUTPUTDIR}/log.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp_math.h" "${OUTPUTDIR}/psp_math.h"
cp -f "${ROOTDIR}/mqx/source/include/queue.h" "${OUTPUTDIR}/queue.h"
cp -f "${ROOTDIR}/mqx/source/include/mutex.h" "${OUTPUTDIR}/mutex.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psptypes.h" "${OUTPUTDIR}/psptypes.h"
cp -f "${ROOTDIR}/mqx/source/include/io.h" "${OUTPUTDIR}/io.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp_time.h" "${OUTPUTDIR}/psp_time.h"
cp -f "${ROOTDIR}/mqx/source/include/mmu.h" "${OUTPUTDIR}/mmu.h"
cp -f "${ROOTDIR}/mqx/source/include/lwevent.h" "${OUTPUTDIR}/lwevent.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp_cpudef.h" "${OUTPUTDIR}/psp_cpudef.h"
cp -f "${ROOTDIR}/mqx/source/include/charq.h" "${OUTPUTDIR}/charq.h"
cp -f "${ROOTDIR}/mqx/source/include/lwlog.h" "${OUTPUTDIR}/lwlog.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_cnfg.h" "${OUTPUTDIR}/mqx_cnfg.h"
cp -f "${ROOTDIR}/mqx/source/include/gen_rev.h" "${OUTPUTDIR}/gen_rev.h"
cp -f "${ROOTDIR}/mqx/source/include/lwmem.h" "${OUTPUTDIR}/lwmem.h"
cp -f "${ROOTDIR}/mqx/source/include/eds.h" "${OUTPUTDIR}/eds.h"
cp -f "${ROOTDIR}/mqx/source/include/message.h" "${OUTPUTDIR}/message.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/psp_abi.h" "${OUTPUTDIR}/psp_abi.h"
cp -f "${ROOTDIR}/mqx/source/include/pcb.h" "${OUTPUTDIR}/pcb.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx_ioc.h" "${OUTPUTDIR}/mqx_ioc.h"


# ds5 files
if [ "${TOOL}" = "ds5" ]; then
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/compiler/rv_ds5/asm_mac.h" "${OUTPUTDIR}/asm_mac.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/compiler/rv_ds5/comp.h" "${OUTPUTDIR}/comp.h"
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/compiler/gcc_arm/asm_mac.h" "${OUTPUTDIR}/asm_mac.h"
cp -f "${ROOTDIR}/mqx/source/psp/cortex_a/compiler/gcc_arm/comp.h" "${OUTPUTDIR}/comp.h"
:
fi


