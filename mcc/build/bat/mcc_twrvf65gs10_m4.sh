#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/mcc/source/include/mcc_vf600.h" "${OUTPUTDIR}/mcc_vf600.h"
cp -f "${ROOTDIR}/mcc/source/include/mcc_api.h" "${OUTPUTDIR}/mcc_api.h"
cp -f "${ROOTDIR}/mcc/source/include/mcc_common.h" "${OUTPUTDIR}/mcc_common.h"
cp -f "${ROOTDIR}/mcc/source/include/mcc_mqx.h" "${OUTPUTDIR}/mcc_mqx.h"
cp -f "${ROOTDIR}/mcc/source/include/mcc_config.h" "${OUTPUTDIR}/mcc_config.h"


# ds5 files
if [ "${TOOL}" = "ds5" ]; then
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
:
fi


