#==============================================================================
# How to run & debug vybrid using JLinkGDBServer
#
# 1)  Run "JLinkGDBServer.exe -select USB -device VF6xx_M4 -if JTAG -speed auto"
# 2)  Run gdb client: "arm-none-eabi-gdb.exe –x <script_file>.gdbinit"
# 3)  Run one of LOAD_XXXXX functions defined in <script_file>.gdbinit,
#    using application path as argument.
#        f.i "LOAD_RAM app/path/ram_debug/hello.elf"
# 4) Type "continue" to run application
#
# verified with JLINK JLinkARM_V466, codesourcery arm-2012.03-56-arm-none-eabi
#==============================================================================






#==============================================================================
# LOAD_RAM <my_app.elf>
define LOAD_RAM
#==============================================================================
    if $argc == 0
        print "missing elf filename"
    else

        # remote connection
        target remote localhost:2331

        # reset type - core reset
        monitor reset 1

        # halt cpu
        monitor halt

        # load image, 'pc' is set automatically
        load $arg0

        # load symbols
        file $arg0

        # take stack address from 0.th item of  __vector_table
        print (*((unsigned int *)__vector_table))
        # and set sp register
        set $sp = $
    end
end
#==============================================================================






