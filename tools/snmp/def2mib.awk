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


function parsetype(type) {

    if (type == "OCTET") {
        return "OCTET STRING"
    } else if (type == "OBJECT") {
        return "OBJECT IDENTIFIER"
    } else {
        return type
    }
}

function parsesyntax(object) {

    if (mibsyntax[object] == "SEQUENCE") {
        if ((mibchildren[object] == 1) && (mibsyntax[mibfirstchild[object]] == "SEQUENCE")) {
            return "SEQUENCE OF " parsesyntax(mibfirstchild[object])
        } else {
            return toupper(substr(object,1,1)) substr(object,2)
        }
    } else {
        return parsetype(mibsyntax[object])
    }
}

END {

    if (length(defn) > 0) {
        print "    " defn " DEFINITIONS ::= BEGIN"
        print ""
    }

    for (type = firsttype; type in mibtype; type = mibnexttype[type]) {
        print "        " type " ::= " parsetype(mibtype[type])
    }

    for (object = firstobject; object in mibparent; object = mibnextobject[object]) {
        if (!(object in mibsyntax)) {
            print ""
            print "        " object substr("              ",length(object)) " OBJECT IDENTIFIER ::= { " mibparent[object] " }"
        } else {

            print ""
            print "        " object " OBJECT-TYPE"
            print "                SYNTAX " parsesyntax(object)
            print "                ACCESS " mibaccess[object]
            print "                STATUS " mibstatus[object]
            print "                DESCRIPTION"
            print "                        \"\""
            if (object in mibindex) {
            print "                INDEX { " mibindex[object] " }"
            }
            print "                ::= { " mibparent[object] " }"

            if ((mibsyntax[object] == "SEQUENCE") && ((mibchildren[object] > 1) || (mibsyntax[mibfirstchild[object]] != "SEQUENCE"))) {
            print ""
            print "        " parsesyntax(object) " ::= SEQUENCE { "
            for (i=1; i<mibchildren[object]; i++) {
            print "            " mibsequence[object,i] " " parsesyntax(mibsequence[object,i]) ","
            }
            print "            " mibsequence[object,i] " " parsesyntax(mibsequence[object,i])
            print "        }"
            }

        }
    }

    if (length(defn) > 0) {
        print ""
        print "    END"
    }
}

(state == 1) {next}

($0 ~ /^-- Definition/) {
    defn = $NF
}

($1 == "%%") && (NF == 1) {
    state = 1
}

($1 == "%%") && (NF == 3) {

    if (!(firsttype in mibtype)) {
        firsttype = $2
    }

    mibtype[$2] = $3

    if (lasttype in mibtype) {
        mibnexttype[lasttype] = $2
    }
    lasttype = $2
}

($1 ~ /^[a-z]/) {

    if (!(firstobject in mibparent)) {
        firstobject = $1
    }

    split($2,a,/\./)

    #
    # $1 is a new MIB node, a[1] is its parent, and a[2] is its index
    #

    mibparent[$1] = a[1] " " a[2]

    if (NF >= 5) {

        mibsyntax[$1] = $3
        mibaccess[$1] = $4
        mibstatus[$1] = $5

        if (NF > 5) {
            mibindex[$1] = $6
            for (i=7; i<=NF; i++) {
                mibindex[$1] = mibindex[$1] ", " $i
            }
        }

        if (!(a[1] in mibfirstchild)) {
            mibfirstchild[a[1]] = $1
        }
        mibchildren[a[1]]++
        mibsequence[a[1],mibchildren[a[1]]] = $1
    }

    if (lastobject in mibparent) {
        mibnextobject[lastobject] = $1
    }
    lastobject = $1

}

# EOF
