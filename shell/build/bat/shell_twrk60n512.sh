#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/shell/source/include/sh_mfs.h" "${OUTPUTDIR}/sh_mfs.h"
cp -f "${ROOTDIR}/shell/source/include/sh_enet.h" "${OUTPUTDIR}/sh_enet.h"
cp -f "${ROOTDIR}/shell/source/include/sh_rtcs.h" "${OUTPUTDIR}/sh_rtcs.h"
cp -f "${ROOTDIR}/shell/source/include/shell.h" "${OUTPUTDIR}/shell.h"


# kds files
if [ "${TOOL}" = "kds" ]; then
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
:
fi


