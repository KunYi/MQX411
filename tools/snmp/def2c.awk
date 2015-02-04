#HEADER**********************************************************************
#
# Copyright 2014 Freescale Semiconductor, Inc.
#
# This software is owned or controlled by Freescale Semiconductor.
# Use of this software is governed by the Freescale MQX RTOS License
# distributed with this Material.
# See the MQX_RTOS_LICENSE file distributed for more details.
#
# Brief License Summary:
# This software is provided in source form for you to use free of charge,
# but it is not open source software. You are allowed to use this software
# but you cannot redistribute it or derivative works of it in source form.
# The software may be used only in connection with a product containing
# a Freescale microprocessor, microcontroller, or digital signal processor.
# See license agreement file for full license terms including other
# restrictions.
#END************************************************************************


function indextype(type) {

#
# Returns:
#     0   if type is a 32-bit integer
#     n>0 if type is a length n octet string
#

    if (type == "IpAddress")      return 4;
    if (type == "NetworkAddress") return 4;

    return 0;

}

function printvarbind(varbind) {

    print "   /* VarBind for " varbind " */"
    print ""
    print "   varlen = 0;"
    print "   node = &MIBNODE_" varbind ";"
    if (varbind in mibtableroot) {
        print "   MIB_find_" mibtableroot[varbind] "(RTCSMIB_OP_GET, index_" varbind ", &instance);"
    } else {
        print "   instance = NULL;"
    }
    print ""

    if (mibtype[varbind] ~ /INTEGER/) {
        print "   { /* Scope */"
        print "      int_32   sinteger = 0;"
        print "      uint_32  uinteger = 0;"
        print "#if SNMPCFG_HAVE_INT64"
        print "      int_64   sinteger64 = 0;"
        print "      uint_64  uinteger64 = 0;"
        print "#endif"
        print ""
        print "      switch (node->TYPE) {"
        print "      case RTCSMIB_NODETYPE_INT_CONST:    sinteger = node->GET.INT_CONST; break;"
        print "      case RTCSMIB_NODETYPE_INT_PTR:      sinteger = *node->GET.INT_PTR; break;"
        print "      case RTCSMIB_NODETYPE_INT_FN:       sinteger = node->GET.INT_FN(instance); break;"
        print "      case RTCSMIB_NODETYPE_UINT_CONST:   uinteger = node->GET.UINT_CONST; break;"
        print "      case RTCSMIB_NODETYPE_UINT_PTR:     uinteger = *node->GET.UINT_PTR; break;"
        print "      case RTCSMIB_NODETYPE_UINT_FN:      uinteger = node->GET.UINT_FN(instance); break;"
        print "#if SNMPCFG_HAVE_INT64"
        print "      case RTCSMIB_NODETYPE_INT64_CONST:  sinteger64 = node->GET.INT64_CONST; break;"
        print "      case RTCSMIB_NODETYPE_INT64_PTR:    sinteger64 = *node->GET.INT64_PTR; break;"
        print "      case RTCSMIB_NODETYPE_INT64_FN:     sinteger64 = node->GET.INT64_FN(instance); break;"
        print "      case RTCSMIB_NODETYPE_UINT64_CONST: uinteger64 = node->GET.UINT64_CONST; break;"
        print "      case RTCSMIB_NODETYPE_UINT64_PTR:   uinteger64 = *node->GET.UINT64_PTR; break;"
        print "      case RTCSMIB_NODETYPE_UINT64_FN:    uinteger64 = node->GET.UINT64_FN(instance); break;"
        print "#endif"
        print "      } /* Endswitch */"
        print ""
        print "      switch (node->TYPE) {"
        print "      case RTCSMIB_NODETYPE_INT_CONST:"
        print "      case RTCSMIB_NODETYPE_INT_PTR:"
        print "      case RTCSMIB_NODETYPE_INT_FN:"
        print "         ASN1_BKWRITE_TYPELEN_BIGINT(&snmp, varlen, node->ASN1_TYPE, sinteger);"
        print "         break;"
        print "      case RTCSMIB_NODETYPE_UINT_CONST:"
        print "      case RTCSMIB_NODETYPE_UINT_PTR:"
        print "      case RTCSMIB_NODETYPE_UINT_FN:"
        print "         ASN1_BKWRITE_TYPELEN_BIGUINT(&snmp, varlen, node->ASN1_TYPE, uinteger);"
        print "         break;"
        print "#if SNMPCFG_HAVE_INT64"
        print "      case RTCSMIB_NODETYPE_INT64_CONST:"
        print "      case RTCSMIB_NODETYPE_INT64_PTR:"
        print "      case RTCSMIB_NODETYPE_INT64_FN:"
        print "         ASN1_BKWRITE_TYPELEN_BIGINT(&snmp, varlen, node->ASN1_TYPE, sinteger64);"
        print "         break;"
        print "      case RTCSMIB_NODETYPE_UINT64_CONST:"
        print "      case RTCSMIB_NODETYPE_UINT64_PTR:"
        print "      case RTCSMIB_NODETYPE_UINT64_FN:"
        print "         ASN1_BKWRITE_TYPELEN_BIGUINT(&snmp, varlen, node->ASN1_TYPE, uinteger64);"
        print "         break;"
        print "#endif"
        print "      } /* Endswitch */"
        print "   } /* Endscope */"

    } else if (mibtype[varbind] ~ /Counter|Gauge|TimeTicks|Counter64/) {
        print "   { /* Scope */"
        print "#if SNMPCFG_HAVE_INT64"
        print "      uint_64 uinteger = 0;"
        print "#else"
        print "      uint_32 uinteger = 0;"
        print "#endif"
        print ""
        print "      switch (node->TYPE) {"
        print "      case RTCSMIB_NODETYPE_UINT_CONST: uinteger = node->GET.UINT_CONST; break;"
        print "      case RTCSMIB_NODETYPE_UINT_PTR:   uinteger = *node->GET.UINT_PTR; break;"
        print "      case RTCSMIB_NODETYPE_UINT_FN:    uinteger = node->GET.UINT_FN(instance); break;"
        print "#if SNMPCFG_HAVE_INT64"
        print "      case RTCSMIB_NODETYPE_UINT64_CONST: uinteger = node->GET.UINT64_CONST; break;"
        print "      case RTCSMIB_NODETYPE_UINT64_PTR:   uinteger = *node->GET.UINT64_PTR; break;"
        print "      case RTCSMIB_NODETYPE_UINT64_FN:    uinteger = node->GET.UINT64_FN(instance); break;"
        print "#endif"
        print "      } /* Endswitch */"
        print ""
        print "      ASN1_BKWRITE_TYPELEN_BIGUINT(&snmp, varlen, node->ASN1_TYPE, uinteger);"
        print "   } /* Endscope */"

    } else if (mibtype[varbind] ~ /OCTET|NetworkAddress|IpAddress|Opaque/) {
        print "   { /* Scope */"
        print "      uint_32   octlen = 0, getoctlen = 0;"
        print "      uchar_ptr octstr = NULL;"
        print ""
        print "      switch (node->TYPE) {"
        print "      case RTCSMIB_NODETYPE_DISPSTR_PTR: octstr = node->GET.DISPSTR_PTR; getoctlen = 1; break;"
        print "      case RTCSMIB_NODETYPE_DISPSTR_FN:  octstr = node->GET.DISPSTR_FN(instance); getoctlen = 1; break;"
        print "      case RTCSMIB_NODETYPE_OCTSTR_FN:   octstr = node->GET.OCTSTR_FN(instance, &octlen); break;"
        print "      } /* Endswitch */"
        print ""
        print "      if (getoctlen) {"
        print "         octlen = 0;"
        print "         if (octstr) {"
        print "            while (octstr[octlen] != '\\0') {"
        print "               octlen++;"
        print "            } /* Endwhile */"
        print "         } /* Endif */"
        print "      } /* Endif */"
        print ""
        print "      ASN1_BKWRITE_TYPELEN_BYTES(&snmp, varlen, node->ASN1_TYPE, octstr, octlen);"
        print "   } /* Endscope */"

    } else if (mibtype[varbind] ~ /OBJECT/) {
        print "   { /* Scope */"
        print "      RTCSMIB_NODE_PTR oidptr = NULL;"
        print ""
        print "      switch (node->TYPE) {"
        print "      case RTCSMIB_NODETYPE_OID_PTR: oidptr = node->GET.OID_PTR; break;"
        print "      case RTCSMIB_NODETYPE_OID_FN:  oidptr = node->GET.OID_FN(instance); break;"
        print "      } /* Endswitch */"
        print ""
        print "      if (!oidptr) {"
        print "         ASN1_BKWRITE_ID(&snmp, varlen, 0);"
        print "      } else {"
        print "         while (oidptr) {"
        print "            ASN1_BKWRITE_ID(&snmp, varlen, oidptr->ID);"
        print "            oidptr = oidptr->PARENT;"
        print "         } /* Endwhile */"
        print "      } /* Endif */"
        print "      ASN1_BKWRITE_TYPELEN(&snmp, varlen, ASN1_TYPE_OBJECT, varlen);"
        print "   } /* Endscope */"

    }

    print ""
    print "   oidlen = 0;"
    print ""

    if (varbind in mibtableroot) {
        print "   { /* Scope */"
        print "      struct {"
        for (k=0; k<mibtablelen[mibtableroot[varbind]]; k++) {
            if (mibtabletype[mibtableroot[varbind],k] == 0) {
                print "         uint_32 " mibtable[mibtableroot[varbind],k] ";"
            } else {
                print "         uchar   " mibtable[mibtableroot[varbind],k] "[" mibtabletype[mibtableroot[varbind],k] "];"
            }
        }
        print "      } _PTR_ index = index_" varbind ";"
        for (k=mibtablelen[mibtableroot[varbind]]-1; k>=0; k--) {
            if (mibtabletype[mibtableroot[varbind],k] == 0) {
                print "      ASN1_BKWRITE_ID(&snmp, oidlen, index->" mibtable[mibtableroot[varbind],k] ");"
            } else {
                for (l=mibtabletype[mibtableroot[varbind],k]-1; l>=0; l--) {
                    print "      ASN1_BKWRITE_ID(&snmp, oidlen, index->" mibtable[mibtableroot[varbind],k] "[" l "]);"
                }
            }
        }
        print "   } /* Endscope */"
    } else {
        print "      ASN1_BKWRITE_ID(&snmp, oidlen, 0);"
    }

    print "   while (node) {"
    print "      ASN1_BKWRITE_ID(&snmp, oidlen, node->ID);"
    print "      node = node->PARENT;"
    print "   } /* Endwhile */"
    print "   ASN1_BKWRITE_TYPELEN(&snmp, oidlen, ASN1_TYPE_OBJECT, oidlen);"
    print "   traplen += oidlen + varlen;"
    print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_SEQUENCE, oidlen+varlen);"
    print ""

}

function printmib() {

    for (i in mibtablelen) {
        for (j=0; j<mibtablelen[i]; j++) {
            if (!(mibtable[i,j] in mibtype)) {
                print "Error:  " mibtable[i,j] " has unknown type" > "/dev/stderr"
                exit
            }
        }
    }

    for (i in trapent) {
        for (j=0; j<trapvarnum[i]; j++) {
            if (!length(mibtype[trapvar[i,j]])) {
                print "Error:  " trapvar[i,j] " has unknown type" > "/dev/stderr"
                exit
            }
        }
    }

    for (i in trap2ent) {
        for (j=0; j<trap2varnum[i]; j++) {
            if (!length(mibtype[trap2var[i,j]])) {
                print "Error:  " trap2var[i,j] " has unknown type" > "/dev/stderr"
                exit
            }
        }
    }

    print "/*HEADER****************************************************************"
    print "*"
    print "* Copyright (c) 2008-" strftime("%Y") " Freescale Semiconductor;"
    print "* All Rights Reserved"
    print "*"
    print "* Copyright (c) 2004-2009 Embedded Access Inc.;"
    print "* All Rights Reserved"                          
    print "*"
    print "* Copyright (c) 1989-2008 ARC International;"
    print "* All Rights Reserved"
    print "*"
    print "***************************************************************************** "
    print "*"
    print "* THIS SOFTWARE IS PROVIDED BY FREESCALE \"AS IS\" AND ANY EXPRESSED OR"
    print "* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES"
    print "* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED."
    print "* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,"
    print "* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES"
    print "* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR"
    print "* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)"
    print "* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,"
    print "* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING"
    print "* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF"
    print "* THE POSSIBILITY OF SUCH DAMAGE."
    print "*"
    print "***************************************************************************** "
    print "*"
    print "* Comments:  This file was generated by \"gawk -f def2c.awk\" script."
    print "*"
    print "*END*******************************************************************/"
    print ""
    print "#include <rtcsrtos.h>"
    print "#include <rtcs.h>"
    #print "#include <rtcs_prv.h>"
    #print "#include <psp.h>"
    #print "#include <fio.h>"
    #print "#include \"tcpip.h\""
    for (i in trapent) {
        print "#include \"ip_prv.h\""
        break
    }
    print "#include \"snmpcfg.h\""
    print "#include \"asn1.h\""
    print "#include \"snmp.h\""
    print ""
    print ""
    print "#if RTCSCFG_ENABLE_SNMP"
    print ""


    cnst = ""
    for (i in mibextern) {
        print "extern " cnst "RTCSMIB_NODE MIBNODE_" i ";"
    }

    print ""

    cnst = ""
    for (i=mibsortindexstart; i<=mibsortindexend; i++) {
        if (i in mibsortindex) {
            split (mibsortindex[i], s, SUBSEP)
            print "extern " cnst "RTCSMIB_NODE MIBNODE_" s[1] ";"
            cnst = "const "
        }
    }

    print ""

    for (i=mibsortindexstart; i<=mibsortindexend; i++) {
        if (i in mibsortindex) {
            split (mibsortindex[i], s, SUBSEP)
            print "extern const RTCSMIB_VALUE MIBVALUE_" s[1] ";"
        }
    }

    print ""

    for (i in trapent) {
        print "static uint_32 MIB_get_sysUpTime(pointer);"
        print ""
        break
    }

    for (i in mibtablelen) {
        print "boolean MIB_find_" i substr("                   ",length(i)) "(uint_32, pointer, pointer _PTR_);"
    }

    print ""

    for (i in mibtablelen) {
        print "boolean MIB_instance_" i substr("                   ",length(i)) "(RTCSMIB_WALK_PTR, uint_32, boolean (_CODE_PTR_)(uint_32, pointer, pointer _PTR_), boolean _PTR_, pointer _PTR_);"
    }

    print ""

    for (i=mibsortindexstart; i<=mibsortindexend; i++) {
        if (i in mibsortindex) {
            split(mibsortindex[i],s,SUBSEP)
            if (mibaccess[s[1]] ~ /WRITE/) {
                print "uint_32 MIB_set_" s[1] substr("                   ",length(s[1])) "(pointer, uchar_ptr, uint_32);"
            }
        }
    }

    print ""

    cnst = ""
    for (i=mibsortindexstart; i<=mibsortindexend; i++) {
        if (i in mibsortindex) {
            split(mibsortindex[i],s,SUBSEP)
            print cnst "RTCSMIB_NODE MIBNODE_" s[1] " = {"
            cnst = "const "
            print "   " s[2] ","
            print ""
            if (mibnext[s[1]] == "") {
               print "   NULL,"
            } else {
               print "   (RTCSMIB_NODE_PTR)&MIBNODE_" mibnext[s[1]] ","
            }
            if (mibchild[s[1]] == "") {
               print "   NULL,"
            } else {
               print "   (RTCSMIB_NODE_PTR)&MIBNODE_" mibchild[s[1]] ","
            }
            print "   (RTCSMIB_NODE_PTR)&MIBNODE_" mibparent[s[1]] ","
            print ""
            print "   " mibaccess[s[1]] ","
            if (s[1] in mibtablelen) {
                print "   MIB_find_" s[1] ","
            } else {
                print "   NULL,"
            }
            if (mibtype[s[1]] == "") {
                # print "   NULL, 0, 0, {0}, NULL"
                print "   NULL, 0, NULL, NULL"
            } else {
                if (s[1] in mibtableroot) {
                    print "   MIB_instance_" mibtableroot[s[1]] ", ASN1_TYPE_" mibtype[s[1]] ","
                } else {
                    print "   MIB_instance_zero, ASN1_TYPE_" mibtype[s[1]] ","
                }
                
                # old implementation used to have value union embedded in the RTCSMIB_NODE strcuture   
                # print "   RTCSMIB_NODETYPE_INT_CONST, {0},"
                
                # new implementation uses pointer to RTCSMIB_VALUE strucuture so RTCSMIB_NODE may be const
                print "   (RTCSMIB_VALUE_PTR)&MIBVALUE_" s[1] ","
                
                if (mibaccess[s[1]] ~ /WRITE/) {
                    print "   MIB_set_" s[1]
                } else {
                    print "   NULL"
                }
            }
            print "};"
            print ""
        }
    }

    print ""

    for (i in mibtablelen) {
        print "static boolean MIB_instance_" i
        print "   ("
        print "      RTCSMIB_WALK_PTR     mib,"
        print "      uint_32              op,"
        print "      boolean (_CODE_PTR_  find)(uint_32, pointer, pointer _PTR_),"
        print "      boolean _PTR_        found,"
        print "      pointer _PTR_        instance"
        print "   )"
        print "{ /* Body */"
        print "   struct {"
        for (j=0; j<mibtablelen[i]; j++) {
            mibtabletype[i,j] = indextype(mibtype[mibtable[i,j]])
            if (mibtabletype[i,j] == 0) {
                print "      uint_32 " mibtable[i,j] ";"
            } else {
                print "      uchar   " mibtable[i,j] "[" mibtabletype[i,j] "];"
            }
        }
        print "   } index;"
        print ""
        print "   *found = FALSE;"
        print "   *instance = NULL;"
        print ""
        print "   switch (op) {"
        print ""
        print "   case RTCSMIB_OP_GET:"
        print "   case RTCSMIB_OP_SET:"
        print ""
        for (j=0; j<mibtablelen[i]; j++) {
            if (mibtabletype[i,j] == 0) {
                print "      if (mib->inlen == 0) break;"
                print "      if (!RTCSMIB_id_read32(mib, &index." mibtable[i,j] ")) return FALSE;"
            } else {
                for (k=0; k<mibtabletype[i,j]; k++) {
                    print "      if (mib->inlen == 0) break;"
                    print "      if (!RTCSMIB_id_read8(mib, &index." mibtable[i,j] "[" k "])) return FALSE;"
                }
            }
        }
        print "      if (mib->inlen != 0) break;"
        print "      *found = find(op, &index, instance);"
        print "      break;"
        print ""
        print "   case RTCSMIB_OP_GETNEXT:"
        for (j=0; j<mibtablelen[i]; j++) {
            if (mibtabletype[i,j] == 0) {
                print "      if (mib->inlen == 0) {"
                print "         index." mibtable[i,j] " = 0;"
                print "      } else {"
                print "         if (!RTCSMIB_id_read32(mib, &index." mibtable[i,j] ")) return FALSE;"
                if (j == mibtablelen[i] - 1) {
                    print "         if(index." mibtable[i,j] " < 4294967295) /* 0xffffffff */"
                    print "            index." mibtable[i,j] "++;"
                }
                print "      } /* Endif */"
            } else {
                for (k=0; k<mibtabletype[i,j]; k++) {
                    print "      if (mib->inlen == 0) {"
                    print "         index." mibtable[i,j] "[" k "] = 0;"
                    print "      } else {"
                    print "         if (!RTCSMIB_id_read8(mib, &index." mibtable[i,j] "[" k "])) return FALSE;"
                    if (j == mibtablelen[i] - 1 && k == mibtabletype[i,j] - 1) {
                        print "         if(index." mibtable[i,j] "[" k "] < 255)"
                        print "            index." mibtable[i,j] "[" k "]++;"
                    }
                    print "      } /* Endif */"
                }
            }
        }
        print "      *found = find(op, &index, instance);"
        print "      break;"
        print ""
        print "   case RTCSMIB_OP_GETFIRST:"
        for (j=0; j<mibtablelen[i]; j++) {
            if (mibtabletype[i,j] == 0) {
                print "      index." mibtable[i,j] " = 0;"
            } else {
                for (k=0; k<mibtabletype[i,j]; k++) {
                    print "      index." mibtable[i,j] "[" k "] = 0;"
                }
            }
        }
        print "      *found = find(RTCSMIB_OP_GETNEXT, &index, instance);"
        print "      break;"
        print "   } /* Endswitch */"
        print ""
        print "   if (*found && (op != RTCSMIB_OP_SET)) {"
        for (j=0; j<mibtablelen[i]; j++) {
            if (mibtabletype[i,j] == 0) {
                print "      if (!RTCSMIB_id_write32 (mib, index." mibtable[i,j] ")) return FALSE;"
            } else {
                for (k=0; k<mibtabletype[i,j]; k++) {
                    print "      if (!RTCSMIB_id_write8  (mib, index." mibtable[i,j] "[" k "])) return FALSE;"
                }
            }
        }
        print "   } /* Endif */"
        print ""
        print "   return TRUE;"
        print "} /* Endbody */"
        print ""
    }

    print ""

    for (i in trapent) {
        print "static void SNMP_trap_" i
        print "   ("
        print "      void"
        for (j=0; j<trapvarnum[i]; j++) {
            if (trapvar[i,j] in mibtableroot) {
                print "    , pointer           index_" trapvar[i,j]
            }
        }
        print "   )"
        print "{ /* Body */"
        print "   SNMP_PARSE_PTR    snmpcfg = SNMP_get_data();"
        print "   uint_32           traplen, oidlen, varlen;"
        print "   RTCSMIB_NODE_PTR  node;"
        print "   pointer           instance;"
        print "   uint_32           i;"
        print "   uchar_ptr         ipsrcp;"
        print "   _ip_address       ipsrc;"
        print "   sockaddr_in       addr;"
        print ""
        print "   uchar outbuf[SNMPCFG_BUFFER_SIZE];"
        print "   struct {"
        print "      uint_32     outlen;"
        print "      uchar_ptr   outbuf;"
        print "   } snmp;"
        print ""
        print "   traplen = 0;"
        print "   snmp.outlen = sizeof(outbuf);"
        print "   snmp.outbuf = outbuf + sizeof(outbuf);"
        print ""
        for (j=trapvarnum[i]-1; j>=0; j--) {
            printvarbind(trapvar[i,j])
        }
        print "   /* complete variable-bindings */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_SEQUENCE, traplen);"
        print "   /* time-stamp */"
        print "   i = MIB_get_sysUpTime(NULL);"
        print "   ASN1_BKWRITE_TYPELEN_BIGUINT(&snmp, traplen, ASN1_TYPE_TimeTicks, i);"
        print "   /* specific-trap */"
        print "   ASN1_BKWRITE_TYPELEN_INT(&snmp, traplen, " trapspec[i] ");"
        print "   /* generic-trap */"
        print "   ASN1_BKWRITE_TYPELEN_INT(&snmp, traplen, " trapgen[i] ");"
        print "   /* skip agent-addr */"
        print "   traplen     += 4;"
        print "   snmp.outlen -= 4;"
        print "   snmp.outbuf -= 4;"
        print "   ipsrcp = snmp.outbuf;"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_IpAddress, 4);"
        print ""
        print "   /* enterprise */"
        print "   node = &MIBNODE_" trapent[i] ";"
        print "   oidlen = 0;"
        print "   while (node) {"
        print "      ASN1_BKWRITE_ID(&snmp, oidlen, node->ID);"
        print "      node = node->PARENT;"
        print "   } /* Endwhile */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, oidlen, ASN1_TYPE_OBJECT, oidlen);"
        print "   traplen += oidlen;"
        print ""
        print "   /* complete Trap-PDU */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_PDU_TRAP, traplen);"
        print "   /* community */"
        print "   ASN1_BKWRITE_TYPELEN_BYTES(&snmp, traplen, ASN1_TYPE_OCTET, snmpcfg->community[snmpcfg->currentcommunity], snmpcfg->communitylen[snmpcfg->currentcommunity]);"
        print "   /* version, 0=v1 */"
        print "   ASN1_BKWRITE_TYPELEN_INT(&snmp, traplen, 0);"
        print "   /* complete Message */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_SEQUENCE, traplen);"
        print ""
        print "   addr.sin_family = AF_INET;"
        print "   addr.sin_port   = IPPORT_SNMPTRAP;"
        print "   for (i=0; i<SNMPCFG_MAX_TRAP_RECEIVERS; i++) {"
        print "      if (snmpcfg->trap_receiver_list[i] != 0] {"
        print "         ipsrc = IP_route_find(snmpcfg->trap_receiver_list[i], 0);"
        print "         htonl(ipsrcp, ipsrc);"
        print "         addr.sin_addr.s_addr = snmpcfg->trap_receiver_list[i];"
        print "         sendto(snmpcfg->trapsock, snmp.outbuf, traplen, 0, &addr, sizeof(addr));"
        print "      } /* Endif */"
        print "   } /* Endfor */"
        print ""
        print "} /* Endbody */"
        print ""
    }

    for (i in trap2ent) {
        print "static void SNMPv2_trap_" i
        print "   ("
        print "      void"
        for (j=0; j<trap2varnum[i]; j++) {
            if (trap2var[i,j] in mibtableroot) {
                print "    , pointer           index_" trap2var[i,j]
            }
        }
        print "   )"
        print "{ /* Body */"
        print "   SNMP_PARSE_PTR    snmpcfg = SNMP_get_data();"
        print "   uint_32           traplen, oidlen, varlen;"
        print "   RTCSMIB_NODE_PTR  node;"
        print "   pointer           instance;"
        print "   uint_32           i;"
        print "   sockaddr_in       addr;"
        print ""
        print "   uchar outbuf[SNMPCFG_BUFFER_SIZE];"
        print "   struct {"
        print "      uint_32     outlen;"
        print "      uchar_ptr   outbuf;"
        print "   } snmp;"
        print ""
        print "   traplen = 0;"
        print "   snmp.outlen = sizeof(outbuf);"
        print "   snmp.outbuf = outbuf + sizeof(outbuf);"
        print ""
        for (j=trap2varnum[i]-1; j>=0; j--) {
            printvarbind(trap2var[i,j])
        }

        print "   /* VarBind for snmpTrapOID */"
        print ""
        print "   varlen = 0;"
        print "   node = &MIBNODE_" trap2ent[i] ";"
        print "   ASN1_BKWRITE_ID(&snmp, varlen, " trap2entchild[i] ");"
        print "   while (node) {"
        print "      ASN1_BKWRITE_ID(&snmp, varlen, node->ID);"
        print "      node = node->PARENT;"
        print "   } /* Endwhile */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, varlen, ASN1_TYPE_OBJECT, varlen);"
        print ""
        print "   oidlen = 0;"
        print "   node = &MIBNODE_snmpTrapOID;"
        print "   ASN1_BKWRITE_ID(&snmp, oidlen, 0);"
        print "   while (node) {"
        print "      ASN1_BKWRITE_ID(&snmp, oidlen, node->ID);"
        print "      node = node->PARENT;"
        print "   } /* Endwhile */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, oidlen, ASN1_TYPE_OBJECT, oidlen);"
        print ""
        print "   traplen += oidlen + varlen;"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_SEQUENCE, oidlen+varlen);"
        print ""

        print "   /* VarBind for sysUpTime */"
        print ""
        print "   varlen = 0;"
        print "   i = MIB_get_sysUpTime(NULL);"
        print "   ASN1_BKWRITE_TYPELEN_BIGUINT(&snmp, varlen, ASN1_TYPE_TimeTicks, i);"
        print ""
        print "   oidlen = 0;"
        print "   node = &MIBNODE_sysUpTime;"
        print "   ASN1_BKWRITE_ID(&snmp, oidlen, 0);"
        print "   while (node) {"
        print "      ASN1_BKWRITE_ID(&snmp, oidlen, node->ID);"
        print "      node = node->PARENT;"
        print "   } /* Endwhile */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, oidlen, ASN1_TYPE_OBJECT, oidlen);"
        print ""
        print "   traplen += oidlen + varlen;"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_SEQUENCE, oidlen+varlen);"
        print ""

        print "   /* complete variable-bindings */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_SEQUENCE, traplen);"
        print "   /* error-index */"
        print "   ASN1_BKWRITE_TYPELEN_INT(&snmp, traplen, 0);"
        print "   /* error-status */"
        print "   ASN1_BKWRITE_TYPELEN_INT(&snmp, traplen, 0);"
        print "   /* request-id */"
        print "   ASN1_BKWRITE_TYPELEN_INT(&snmp, traplen, 0);"
        print ""
        print "   /* complete SNMPv2-Trap-PDU */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_PDU_TRAPV2, traplen);"
        print "   /* community */"
        print "   ASN1_BKWRITE_TYPELEN_BYTES(&snmp, traplen, ASN1_TYPE_OCTET, snmpcfg->community[snmpcfg->currentcommunity], snmpcfg->communitylen[snmpcfg->currentcommunity]);"
        print "   /* version, 1=v2 */"
        print "   ASN1_BKWRITE_TYPELEN_INT(&snmp, traplen, 1);"
        print "   /* complete Message */"
        print "   ASN1_BKWRITE_TYPELEN(&snmp, traplen, ASN1_TYPE_SEQUENCE, traplen);"
        print ""
        print "   addr.sin_family = AF_INET;"
        print "   addr.sin_port   = IPPORT_SNMPTRAP;"
        print "   for (i=0; i<SNMPCFG_MAX_TRAP_RECEIVERS; i++) {"
        print "      if (snmpcfg->trap_receiver_list[i] != 0] {"
        print "         addr.sin_addr.s_addr = snmpcfg->trap_receiver_list[i];"
        print "         sendto(snmpcfg->trapsock, snmp.outbuf, traplen, 0, &addr, sizeof(addr));"
        print "      } /* Endif */"
        print "   } /* Endfor */"
        print ""
        print "} /* Endbody */"
        print ""
    }

    print "#endif"
}

BEGIN {
   mibtype["INTEGER"]        = "INTEGER"
   mibtype["Integer32"]      = "INTEGER"
   mibtype["OCTET"]          = "OCTET"
   mibtype["OBJECT"]         = "OBJECT"
   mibtype["SEQUENCE"]       = "SEQUENCE"
   mibtype["NetworkAddress"] = "NetworkAddress"
   mibtype["IpAddress"]      = "IpAddress"
   mibtype["Counter"]        = "Counter"
   mibtype["Counter32"]      = "Counter"
   mibtype["Gauge"]          = "Gauge"
   mibtype["Gauge32"]        = "Gauge"
   mibtype["Unsigned32"]     = "Gauge"
   mibtype["TimeTicks"]      = "TimeTicks"
   mibtype["Opaque"]         = "Opaque"
   mibtype["Counter64"]      = "Counter64"
   mibtype["BITS"]           = "OCTET"
   mibsortindexstart = 0
   mibsortindexend = 0
}

(state == 1)
(state == 1) {next}
(NF == 0) {next}

($1 ~ /^[^-%]/) {

    split($2,a,/\./)
    gsub(/-/,"",$1)
    gsub(/-/,"",a[1])

    #
    # figure out the lowest and highest NR so we can use it to order 
    # our mibsortindex
    #
    if (mibsortindexstart == 0) {
        mibsortindexstart = NR
    }
    mibsortindexend = NR

    #
    # $1 is a new MIB node, a[1] is its parent, and a[2] is its index
    #
    mibsortindex[NR] = $1 SUBSEP a[2]
    mibindex[$1] = a[2]
    mibparent[$1] = a[1]
    mibnext[$1] = ""
    mibchild[$1] = ""
    mibtype[$1] = mibtype[$3]

    if ($4 ~ /read/) {
        mibaccess[$1] = "RTCSMIB_ACCESS_READ"
    }
    if ($4 ~ /write|create/) {
        if ($1 in mibaccess) {
            mibaccess[$1] = mibaccess[$1] " | "
        }
        mibaccess[$1] = mibaccess[$1] "RTCSMIB_ACCESS_WRITE"
    }
    if (mibaccess[$1] == "") {
        mibaccess[$1] = "0"
    }

    if (a[1] in mibindex) {
        if (a[1] in lastchild) {
            mibnext[lastchild[a[1]]] = $1
        }
        lastchild[a[1]] = $1
    } else {
        mibextern[a[1]] = "";
    }

    if ((a[1] in mibindex) && (mibchild[a[1]] == "")) {
        mibchild[a[1]] = $1
    }

    if (a[1] in mibtableroot) {
        mibtableroot[$1] = mibtableroot[a[1]]
    }

    #
    # Now record index information for tables
    #

    if (NF > 5) {
        mibtableroot[$1] = $1
        if ($6 ~ /IMPLIED/) {
            firstfield = 7
        	mibtablelen[$1] = NF - 6
	  } else { 
            firstfield = 6
        	mibtablelen[$1] = NF - 5 
	  }
        for (i=0; i<mibtablelen[$1] ; i++) {
            mibtable[$1,i] = $(i+firstfield)
        }
    }

}

($1 == "%trap") {

    #
    # $2 is a trap
    #

    if ($3 >= 0) {
        trapgen[$2] = $3
        trapspec[$2] = $4
        trapent[$2] = $5
        trapvarnum[$2] = NF - 5
        for (i=0; i<NF-5; i++) {
            trapvar[$2,i] = $(i+6)
        }
    } else {
        split($4,a,/\./)
        gsub(/-/,"",a[1])
        trap2ent[$2] = a[1]
        trap2entchild[$2] = a[2]
        trap2varnum[$2] = NF - 4
        for (i=0; i<NF-4; i++) {
            trap2var[$2,i] = $(i+5)
        }
    }

}

($1 == "%%" && NF > 1) {
    mibtype[$2] = mibtype[$3]
    next
}

($1 == "%%") {state=1; printmib()}

END {if (state==0) printmib()}

# EOF
