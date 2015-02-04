#!/usr/bin/env bash

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/rtcs/source/include/telnet.h" "${OUTPUTDIR}/telnet.h"
cp -f "${ROOTDIR}/rtcs/source/include/fcs16.h" "${OUTPUTDIR}/fcs16.h"
cp -f "${ROOTDIR}/rtcs/source/include/dhcpsrv.h" "${OUTPUTDIR}/dhcpsrv.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs_base64.h" "${OUTPUTDIR}/rtcs_base64.h"
cp -f "${ROOTDIR}/rtcs/source/include/igmp.h" "${OUTPUTDIR}/igmp.h"
cp -f "${ROOTDIR}/rtcs/source/include/httpsrv.h" "${OUTPUTDIR}/httpsrv.h"
cp -f "${ROOTDIR}/rtcs/source/include/ppphdlc.h" "${OUTPUTDIR}/ppphdlc.h"
cp -f "${ROOTDIR}/rtcs/source/include/iwcfg.h" "${OUTPUTDIR}/iwcfg.h"
cp -f "${ROOTDIR}/rtcs/source/include/dhcp.h" "${OUTPUTDIR}/dhcp.h"
cp -f "${ROOTDIR}/rtcs/source/include/ppp.h" "${OUTPUTDIR}/ppp.h"
cp -f "${ROOTDIR}/rtcs/source/include/mld.h" "${OUTPUTDIR}/mld.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcscfg.h" "${OUTPUTDIR}/rtcscfg.h"
cp -f "${ROOTDIR}/rtcs/source/include/rip.h" "${OUTPUTDIR}/rip.h"
cp -f "${ROOTDIR}/rtcs/source/include/addrinfo.h" "${OUTPUTDIR}/addrinfo.h"
cp -f "${ROOTDIR}/rtcs/source/include/nat.h" "${OUTPUTDIR}/nat.h"
cp -f "${ROOTDIR}/rtcs/source/include/ip6.h" "${OUTPUTDIR}/ip6.h"
cp -f "${ROOTDIR}/rtcs/source/include/asn1.h" "${OUTPUTDIR}/asn1.h"
cp -f "${ROOTDIR}/rtcs/source/include/dns.h" "${OUTPUTDIR}/dns.h"
cp -f "${ROOTDIR}/rtcs/source/include/chap.h" "${OUTPUTDIR}/chap.h"
cp -f "${ROOTDIR}/rtcs/source/include/nat_session.h" "${OUTPUTDIR}/nat_session.h"
cp -f "${ROOTDIR}/rtcs/source/include/arp.h" "${OUTPUTDIR}/arp.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs_sock.h" "${OUTPUTDIR}/rtcs_sock.h"
cp -f "${ROOTDIR}/rtcs/source/include/bootp.h" "${OUTPUTDIR}/bootp.h"
cp -f "${ROOTDIR}/rtcs/source/include/icmp.h" "${OUTPUTDIR}/icmp.h"
cp -f "${ROOTDIR}/rtcs/source/include/snmp.h" "${OUTPUTDIR}/snmp.h"
cp -f "${ROOTDIR}/rtcs/source/include/arpif.h" "${OUTPUTDIR}/arpif.h"
cp -f "${ROOTDIR}/rtcs/source/include/ip-e.h" "${OUTPUTDIR}/ip-e.h"
cp -f "${ROOTDIR}/rtcs/source/include/httpsrv_supp.h" "${OUTPUTDIR}/httpsrv_supp.h"
cp -f "${ROOTDIR}/rtcs/source/include/pap.h" "${OUTPUTDIR}/pap.h"
cp -f "${ROOTDIR}/rtcs/source/include/ftpsrv.h" "${OUTPUTDIR}/ftpsrv.h"
cp -f "${ROOTDIR}/rtcs/source/include/dnscln.h" "${OUTPUTDIR}/dnscln.h"
cp -f "${ROOTDIR}/rtcs/source/include/snmpcfg.h" "${OUTPUTDIR}/snmpcfg.h"
cp -f "${ROOTDIR}/rtcs/source/include/ip6_trace.h" "${OUTPUTDIR}/ip6_trace.h"
cp -f "${ROOTDIR}/rtcs/source/include/pppfsm.h" "${OUTPUTDIR}/pppfsm.h"
cp -f "${ROOTDIR}/rtcs/source/include/icmp6.h" "${OUTPUTDIR}/icmp6.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcstime.h" "${OUTPUTDIR}/rtcstime.h"
cp -f "${ROOTDIR}/rtcs/source/include/tftp.h" "${OUTPUTDIR}/tftp.h"
cp -f "${ROOTDIR}/rtcs/source/include/ftpsrv_msg.h" "${OUTPUTDIR}/ftpsrv_msg.h"
cp -f "${ROOTDIR}/rtcs/source/include/route.h" "${OUTPUTDIR}/route.h"
cp -f "${ROOTDIR}/rtcs/source/include/ip6_if.h" "${OUTPUTDIR}/ip6_if.h"
cp -f "${ROOTDIR}/rtcs/source/include/md5.h" "${OUTPUTDIR}/md5.h"
cp -f "${ROOTDIR}/rtcs/source/include/ipcp.h" "${OUTPUTDIR}/ipcp.h"
cp -f "${ROOTDIR}/rtcs/source/include/checksum.h" "${OUTPUTDIR}/checksum.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcspcb.h" "${OUTPUTDIR}/rtcspcb.h"
cp -f "${ROOTDIR}/rtcs/source/include/nd6.h" "${OUTPUTDIR}/nd6.h"
cp -f "${ROOTDIR}/rtcs/source/include/ip6_prv.h" "${OUTPUTDIR}/ip6_prv.h"
cp -f "${ROOTDIR}/rtcs/source/include/ip.h" "${OUTPUTDIR}/ip.h"
cp -f "${ROOTDIR}/rtcs/source/include/socket.h" "${OUTPUTDIR}/socket.h"
cp -f "${ROOTDIR}/rtcs/source/include/icmp6_prv.h" "${OUTPUTDIR}/icmp6_prv.h"
cp -f "${ROOTDIR}/rtcs/source/include/ccp.h" "${OUTPUTDIR}/ccp.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs.h" "${OUTPUTDIR}/rtcs.h"
cp -f "${ROOTDIR}/rtcs/source/include/ipc_udp.h" "${OUTPUTDIR}/ipc_udp.h"
cp -f "${ROOTDIR}/rtcs/source/include/natvers.h" "${OUTPUTDIR}/natvers.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs_smtp.h" "${OUTPUTDIR}/rtcs_smtp.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs_if.h" "${OUTPUTDIR}/rtcs_if.h"
cp -f "${ROOTDIR}/rtcs/source/include/tcp.h" "${OUTPUTDIR}/tcp.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs_stats.h" "${OUTPUTDIR}/rtcs_stats.h"
cp -f "${ROOTDIR}/rtcs/source/include/udp.h" "${OUTPUTDIR}/udp.h"
cp -f "${ROOTDIR}/rtcs/source/include/tcpip.h" "${OUTPUTDIR}/tcpip.h"
cp -f "${ROOTDIR}/rtcs/source/include/ipcfg.h" "${OUTPUTDIR}/ipcfg.h"
cp -f "${ROOTDIR}/rtcs/source/include/iptunnel.h" "${OUTPUTDIR}/iptunnel.h"
cp -f "${ROOTDIR}/rtcs/source/include/lcp.h" "${OUTPUTDIR}/lcp.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcsrtos.h" "${OUTPUTDIR}/rtcsrtos.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs_err.h" "${OUTPUTDIR}/rtcs_err.h"
cp -f "${ROOTDIR}/rtcs/source/include/ftpc.h" "${OUTPUTDIR}/ftpc.h"
cp -f "${ROOTDIR}/rtcs/source/include/ipradix.h" "${OUTPUTDIR}/ipradix.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcslog.h" "${OUTPUTDIR}/rtcslog.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcsvers.h" "${OUTPUTDIR}/rtcsvers.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs25x.h" "${OUTPUTDIR}/rtcs25x.h"
cp -f "${ROOTDIR}/rtcs/source/include/ticker.h" "${OUTPUTDIR}/ticker.h"
cp -f "${ROOTDIR}/rtcs/source/include/rtcs_in.h" "${OUTPUTDIR}/rtcs_in.h"


# ds5 files
if [ "${TOOL}" = "ds5" ]; then
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
:
fi


