#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
mkdir -p "${OUTPUTDIR}/.."
mkdir -p "${OUTPUTDIR}/Generated_Code"
mkdir -p "${OUTPUTDIR}/tss"
cp -f "${ROOTDIR}/mqx/source/io/lwgpio/lwgpio.h" "${OUTPUTDIR}/lwgpio.h"
cp -f "${ROOTDIR}/mqx/source/io/hwtimer/hwtimer_pit.h" "${OUTPUTDIR}/hwtimer_pit.h"
cp -f "${ROOTDIR}/mqx/source/io/spi/spi_dspi_common.h" "${OUTPUTDIR}/spi_dspi_common.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/init_lpm.h" "${OUTPUTDIR}/init_lpm.h"
cp -f "${ROOTDIR}/mqx/source/io/i2c/i2c_ki2c.h" "${OUTPUTDIR}/i2c_ki2c.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_SensorGPIO_def.h" "${OUTPUTDIR}/tss/TSS_SensorGPIO_def.h"
cp -f "${ROOTDIR}/mqx/source/io/lcd/slcd_kwikstik-k40x256.h" "${OUTPUTDIR}/slcd_kwikstik-k40x256.h"
cp -f "${ROOTDIR}/mqx/source/io/hwtimer/hwtimer_lpt.h" "${OUTPUTDIR}/hwtimer_lpt.h"
cp -f "${ROOTDIR}/mqx/source/io/usb_dcd/usb_dcd_kn_prv.h" "${OUTPUTDIR}/usb_dcd_kn_prv.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/hmi_tss_provider.h" "${OUTPUTDIR}/hmi_tss_provider.h"
cp -f "${ROOTDIR}/config/kwikstikk40x256/user_config.h" "${OUTPUTDIR}/../user_config.h"
cp -f "${ROOTDIR}/mqx/source/io/debug/iodebug.h" "${OUTPUTDIR}/iodebug.h"
cp -f "${ROOTDIR}/mqx/source/io/lcd/Fonts_LCD.h" "${OUTPUTDIR}/Fonts_LCD.h"
cp -f "${ROOTDIR}/mqx/source/include/mqx.h" "${OUTPUTDIR}/mqx.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/hmi_provider.h" "${OUTPUTDIR}/hmi_provider.h"
cp -f "${ROOTDIR}/mqx/source/io/cm/cm_kinetis.h" "${OUTPUTDIR}/cm_kinetis.h"
cp -f "${ROOTDIR}/mqx/source/io/spi/spi_dspi_dma.h" "${OUTPUTDIR}/spi_dspi_dma.h"
cp -f "${ROOTDIR}/mqx/source/io/enet/enet.h" "${OUTPUTDIR}/enet.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/bsp.h" "${OUTPUTDIR}/bsp.h"
cp -f "${ROOTDIR}/mqx/source/io/cm/cm.h" "${OUTPUTDIR}/cm.h"
cp -f "${ROOTDIR}/mqx/source/io/flashx/flashx.h" "${OUTPUTDIR}/flashx.h"
cp -f "${ROOTDIR}/mqx/source/io/timer/timer_qpit.h" "${OUTPUTDIR}/timer_qpit.h"
cp -f "${ROOTDIR}/mqx/source/io/usb/if_dev_khci.h" "${OUTPUTDIR}/if_dev_khci.h"
cp -f "${ROOTDIR}/mqx/source/io/tfs/tfs.h" "${OUTPUTDIR}/tfs.h"
cp -f "${ROOTDIR}/mqx/source/io/serial/serl_kuart.h" "${OUTPUTDIR}/serl_kuart.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/hmi_client.h" "${OUTPUTDIR}/hmi_client.h"
cp -f "${ROOTDIR}/mqx/source/io/gpio/io_gpio.h" "${OUTPUTDIR}/io_gpio.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_SensorTSI_def.h" "${OUTPUTDIR}/tss/TSS_SensorTSI_def.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/PE_LDD.h" "${OUTPUTDIR}/Generated_Code/PE_LDD.h"
cp -f "${ROOTDIR}/config/common/small_ram_config.h" "${OUTPUTDIR}/../small_ram_config.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_GPIO.h" "${OUTPUTDIR}/tss/TSS_GPIO.h"
cp -f "${ROOTDIR}/mqx/source/io/pcb/io_pcb.h" "${OUTPUTDIR}/io_pcb.h"
cp -f "${ROOTDIR}/config/common/maximum_config.h" "${OUTPUTDIR}/../maximum_config.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_API.h" "${OUTPUTDIR}/tss/TSS_API.h"
cp -f "${ROOTDIR}/mqx/source/io/usb/usb_bsp.h" "${OUTPUTDIR}/usb_bsp.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/TSS_SystemSetup.h" "${OUTPUTDIR}/TSS_SystemSetup.h"
cp -f "${ROOTDIR}/mqx/source/io/adc/adc.h" "${OUTPUTDIR}/adc.h"
cp -f "${ROOTDIR}/mqx/source/io/can/flexcan/kflexcan.h" "${OUTPUTDIR}/kflexcan.h"
cp -f "${ROOTDIR}/mqx/source/io/lpm/lpm_kinetis.h" "${OUTPUTDIR}/lpm_kinetis.h"
cp -f "${ROOTDIR}/mqx/source/io/enet/ethernet.h" "${OUTPUTDIR}/ethernet.h"
cp -f "${ROOTDIR}/mqx/source/io/flashx/freescale/flash_ftfl.h" "${OUTPUTDIR}/flash_ftfl.h"
cp -f "${ROOTDIR}/mqx/source/io/usb_dcd/usb_dcd.h" "${OUTPUTDIR}/usb_dcd.h"
cp -f "${ROOTDIR}/mqx/source/io/lpm/lpm.h" "${OUTPUTDIR}/lpm.h"
cp -f "${ROOTDIR}/mqx/source/io/sdcard/sdcard_esdhc/sdcard_esdhc.h" "${OUTPUTDIR}/sdcard_esdhc.h"
cp -f "${ROOTDIR}/config/common/verif_enabled_config.h" "${OUTPUTDIR}/../verif_enabled_config.h"
cp -f "${ROOTDIR}/mqx/source/io/dma/dma.h" "${OUTPUTDIR}/dma.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_StatusCodes.h" "${OUTPUTDIR}/tss/TSS_StatusCodes.h"
cp -f "${ROOTDIR}/mqx/source/io/dma/edma.h" "${OUTPUTDIR}/edma.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_Sensor.h" "${OUTPUTDIR}/tss/TSS_Sensor.h"
cp -f "${ROOTDIR}/mqx/source/io/hwtimer/hwtimer.h" "${OUTPUTDIR}/hwtimer.h"
cp -f "${ROOTDIR}/mqx/source/io/adc/kadc/adc_kadc.h" "${OUTPUTDIR}/adc_kadc.h"
cp -f "${ROOTDIR}/mqx/source/io/esdhc/esdhc.h" "${OUTPUTDIR}/esdhc.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_SensorTSIL.h" "${OUTPUTDIR}/tss/TSS_SensorTSIL.h"
cp -f "${ROOTDIR}/mqx/source/io/lwadc/lwadc_kadc.h" "${OUTPUTDIR}/lwadc_kadc.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_Timer.h" "${OUTPUTDIR}/tss/TSS_Timer.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/bsp_rev.h" "${OUTPUTDIR}/bsp_rev.h"
cp -f "${ROOTDIR}/mqx/source/io/io_mem/io_mem.h" "${OUTPUTDIR}/io_mem.h"
cp -f "${ROOTDIR}/mqx/source/io/spi/spi_dspi.h" "${OUTPUTDIR}/spi_dspi.h"
cp -f "${ROOTDIR}/mqx/source/io/hwtimer/hwtimer_systick.h" "${OUTPUTDIR}/hwtimer_systick.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/hmi_uids.h" "${OUTPUTDIR}/hmi_uids.h"
cp -f "${ROOTDIR}/mqx/source/io/sdcard/sdcard_spi/sdcard_spi.h" "${OUTPUTDIR}/sdcard_spi.h"
cp -f "${ROOTDIR}/mqx/source/io/lwadc/lwadc.h" "${OUTPUTDIR}/lwadc.h"
cp -f "${ROOTDIR}/mqx/source/io/usb/if_host_khci.h" "${OUTPUTDIR}/if_host_khci.h"
cp -f "${ROOTDIR}/mqx/source/io/dma/edma_prv.h" "${OUTPUTDIR}/edma_prv.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_SystemSetupVal.h" "${OUTPUTDIR}/tss/TSS_SystemSetupVal.h"
cp -f "${ROOTDIR}/mqx/source/io/rtc/krtc.h" "${OUTPUTDIR}/krtc.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/kwikstik-k40x256.h" "${OUTPUTDIR}/kwikstik-k40x256.h"
cp -f "${ROOTDIR}/mqx/source/io/i2s/i2s_ki2s.h" "${OUTPUTDIR}/i2s_ki2s.h"
cp -f "${ROOTDIR}/mqx/source/io/spi/spi.h" "${OUTPUTDIR}/spi.h"
cp -f "${ROOTDIR}/mqx/source/io/serial/serial.h" "${OUTPUTDIR}/serial.h"
cp -f "${ROOTDIR}/mqx/source/io/i2s/i2s_audio.h" "${OUTPUTDIR}/i2s_audio.h"
cp -f "${ROOTDIR}/mqx/source/io/pcb/mqxa/pcbmqxav.h" "${OUTPUTDIR}/pcbmqxav.h"
cp -f "${ROOTDIR}/mqx/source/io/usb/if_host_ehci.h" "${OUTPUTDIR}/if_host_ehci.h"
cp -f "${ROOTDIR}/mqx/source/io/pcb/mqxa/pcb_mqxa.h" "${OUTPUTDIR}/pcb_mqxa.h"
cp -f "${ROOTDIR}/mqx/source/io/flashx/freescale/flash_mk40.h" "${OUTPUTDIR}/flash_mk40.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_SensorGPIO.h" "${OUTPUTDIR}/tss/TSS_SensorGPIO.h"
cp -f "${ROOTDIR}/mqx/source/io/usb/if_dev_ehci.h" "${OUTPUTDIR}/if_dev_ehci.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/derivative.h" "${OUTPUTDIR}/derivative.h"
cp -f "${ROOTDIR}/mqx/source/io/i2s/i2s.h" "${OUTPUTDIR}/i2s.h"
cp -f "${ROOTDIR}/mqx/source/io/usb_dcd/usb_dcd_kn.h" "${OUTPUTDIR}/usb_dcd_kn.h"
cp -f "${ROOTDIR}/mqx/source/io/rtc/rtc.h" "${OUTPUTDIR}/rtc.h"
cp -f "${ROOTDIR}/mqx/source/io/pipe/io_pipe.h" "${OUTPUTDIR}/io_pipe.h"
cp -f "${ROOTDIR}/mqx/source/io/adc/kadc/adc_mk40.h" "${OUTPUTDIR}/adc_mk40.h"
cp -f "${ROOTDIR}/mqx/source/io/timer/qpit.h" "${OUTPUTDIR}/qpit.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/btnled.h" "${OUTPUTDIR}/btnled.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/hmi_lwgpio_provider.h" "${OUTPUTDIR}/hmi_lwgpio_provider.h"
cp -f "${ROOTDIR}/mqx/source/io/lwgpio/lwgpio_kgpio.h" "${OUTPUTDIR}/lwgpio_kgpio.h"
cp -f "${ROOTDIR}/mqx/source/io/enet/enet_wifi.h" "${OUTPUTDIR}/enet_wifi.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_SensorTSI.h" "${OUTPUTDIR}/tss/TSS_SensorTSI.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/bsp_cm.h" "${OUTPUTDIR}/bsp_cm.h"
cp -f "${ROOTDIR}/mqx/source/io/io_null/io_null.h" "${OUTPUTDIR}/io_null.h"
cp -f "${ROOTDIR}/mqx/source/io/lcd/PounceTypes_v2.h" "${OUTPUTDIR}/PounceTypes_v2.h"
cp -f "${ROOTDIR}/mqx/source/io/gpio/kgpio/io_gpio_kgpio.h" "${OUTPUTDIR}/io_gpio_kgpio.h"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/PE_Types.h" "${OUTPUTDIR}/Generated_Code/PE_Types.h"
cp -f "${ROOTDIR}/mqx/source/io/i2c/i2c.h" "${OUTPUTDIR}/i2c.h"
cp -f "${ROOTDIR}/mqx/source/io/sdcard/sdcard.h" "${OUTPUTDIR}/sdcard.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_DataTypes.h" "${OUTPUTDIR}/tss/TSS_DataTypes.h"
cp -f "${ROOTDIR}/mqx/source/io/adc/adc_conf.h" "${OUTPUTDIR}/adc_conf.h"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/shared/TSS_SensorTSIL_def.h" "${OUTPUTDIR}/tss/TSS_SensorTSIL_def.h"
cp -f "${ROOTDIR}/config/common/smallest_config.h" "${OUTPUTDIR}/../smallest_config.h"


# kds files
if [ "${TOOL}" = "kds" ]; then
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/gcc_arm/intflash.ld" "${OUTPUTDIR}/intflash.ld"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/lib_cw_gcc/libTSS_KXX_M4.a" "${OUTPUTDIR}/tss.a"
mkdir -p "${OUTPUTDIR}/Generated_Code"
mkdir -p "${OUTPUTDIR}/Sources"
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mqx/source/bsp/kwikstikk40x256/gcc_arm/intflash.ld" "${OUTPUTDIR}/intflash.ld"
cp -f "${ROOTDIR}/mqx/source/io/hmi/TSS/lib_cw_gcc/libTSS_KXX_M4.a" "${OUTPUTDIR}/tss.a"
mkdir -p "${OUTPUTDIR}/Generated_Code"
mkdir -p "${OUTPUTDIR}/Sources"
:
fi

