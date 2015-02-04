#!/usr/bin/env bash
awk -f ../../../tools/snmp/def2c.awk snmp_demo.def > snmp_demo.c
awk -f ../../../tools/snmp/def2mib.awk snmp_demo.def > snmp_demo.mib
