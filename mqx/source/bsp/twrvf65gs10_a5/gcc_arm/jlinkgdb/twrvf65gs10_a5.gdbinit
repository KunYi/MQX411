#==============================================================================
# How to run & debug vybrid using JLinkGDBServer
#
# 1)  Run "JLinkGDBServer.exe -select USB -device VF6xx_A5 -if JTAG -speed auto"
# 2)  Run gdb client: "arm-none-eabi-gdb.exe –x <script_file>.gdbinit"
# 3)  Run one of LOAD_XXXXX functions defined in <script_file>.gdbinit,
#    using application path as argument.
#        f.i "LOAD_INTRAM app/path/intram_debug/hello.elf"
# 4) Type "continue" to run application
#
# verified with JLINK JLinkARM_V466, codesourcery arm-2012.03-56-arm-none-eabi
#==============================================================================




#==============================================================================
define INIT_REGISTERS
# set banked registers to 0, otherwise step debugging is impossible for boot.S,
# JLINK tries to dereference uninitialized registers - causing hardfault
#==============================================================================
    # user registers
    set $cpsr = 0x1d0
    set $r8 = 0
    set $r9 = 0
    set $r10 = 0
    set $r11 = 0
    set $r12 = 0
    set $sp = 0
    set $lr = 0

    # fiq registers
    set $cpsr = 0x1d1
    set $r8 = 0
    set $r9 = 0
    set $r10 = 0
    set $r11 = 0
    set $r12 = 0
    set $sp = 0
    set $lr = 0

    # irq registers
    set $cpsr = 0x1d2
    set $sp = 0
    set $lr = 0

    # abort registers
    set $cpsr = 0x1d7
    set $sp = 0
    set $lr = 0

    # undefined registers
    set $cpsr = 0x1db
    set $sp = 0
    set $lr = 0

    # system registers
    set $cpsr = 0x1df
    set $sp = 0
    set $lr = 0

    # svc registers
    set $cpsr = 0x1d3
    set $sp = 0
    set $lr = 0
end
#==============================================================================






#==============================================================================
define SETUP_DDR
#==============================================================================
    # select pll as system clock
    monitor long 0x4006b02c 0x78 # power up fxosc
    # wait ((data.long(0x4006b004))&0x20)==0x20 #wait for fxosc to lock
    monitor waithalt  500
    monitor long 0x4006b008 0x20 # set fxosc as fast clock

    # enable all clocks gates from ccm
    monitor long 0x4006b040 0xffffffff
    monitor long 0x4006b044 0xffffffff
    monitor long 0x4006b048 0xffffffff
    monitor long 0x4006b04c 0xffffffff
    monitor long 0x4006b050 0xffffffff
    monitor long 0x4006b054 0xffffffff
    monitor long 0x4006b058 0xffffffff
    monitor long 0x4006b05c 0xffffffff
    monitor long 0x4006b060 0xffffffff
    monitor long 0x4006b064 0xffffffff
    monitor long 0x4006b068 0xffffffff
    monitor long 0x4006b06c 0xffffffff

    monitor long 0x40050270 0x2001 # pll1
    # wait ((data.long(0x40050270))&0x80000000)==0x80000000 #wait for lock
    monitor waithalt  500
    monitor long 0x40050030 0x2001 # pll2
    # wait ((data.long(40050030))&0x80000000)==0x80000000 #wait for lock
    monitor waithalt  500
    monitor long 0x40050010 0x3040  # pll3
    # wait ((data.long(40050010))&0x80000000)==0x80000000 #wait for lock
    monitor waithalt  500

    monitor long 0x40050070 0x00002031  # anadig_pll_audio_ctrl: pll4
    monitor waithalt  500
    monitor long 0x400500e0 0x00002001  # anadig_pll_enet_ctrl: pll5
    monitor waithalt  500
    monitor long 0x400500a0 0x00002028  # anadig_pll_video_ctrl: pll6
    monitor waithalt  500
    # wait ((data.long(0x400502c0))&0xff)==0x7c #wait for lock


    # enable all pfds sources and set sysclk as pll1 pfd3 for 396mhz

    monitor long 0x4006b000 0x00010005 # ccm_ccr: firc_en=1 and oscnt=5
    monitor long 0x4006b008 0x0003ff24 # ccm_ccsr: pll1 uses pll1_pfd3, enable all pll1 and pll2, select fast clock, and sys_clock_sel use pll1
    monitor long 0x4006b00c 0x00000850 # ccm_cacrr: arm_div=0 (div by 1), bus_div=2 (div by 3), ipg_div value is 1 (div by 2), pll4 value is 1 (div 4)


    #--------------------------------------------------------------------------
    # ddr_iomuxvoid
    #--------------------------------------------------------------------------
    monitor long 0x4004821c 0x00000140
    monitor long 0x40048220 0x00000140
    monitor long 0x40048224 0x00000140
    monitor long 0x40048228 0x00000140
    monitor long 0x4004822c 0x00000140
    monitor long 0x40048230 0x00000140
    monitor long 0x40048234 0x00000140
    monitor long 0x40048238 0x00000140
    monitor long 0x4004823c 0x00000140
    monitor long 0x40048240 0x00000140
    monitor long 0x40048244 0x00000140
    monitor long 0x40048248 0x00000140
    monitor long 0x4004824c 0x00000140
    monitor long 0x40048250 0x00000140
    monitor long 0x40048254 0x00000140
    monitor long 0x40048258 0x00000140
    monitor long 0x4004825c 0x00000140
    monitor long 0x40048260 0x00000140
    monitor long 0x40048264 0x00000140
    monitor long 0x40048268 0x00000140
    monitor long 0x4004826c 0x00000140
    monitor long 0x40048270 0x00000140
    monitor long 0x40048274 0x00010140
    monitor long 0x40048278 0x00000140
    monitor long 0x4004827c 0x00000140
    monitor long 0x40048280 0x00000140
    monitor long 0x40048284 0x00000140
    monitor long 0x40048288 0x00000140
    monitor long 0x4004828c 0x00000140
    monitor long 0x40048290 0x00000140
    monitor long 0x40048294 0x00000140
    monitor long 0x40048298 0x00000140
    monitor long 0x4004829c 0x00000140
    monitor long 0x400482a0 0x00000140
    monitor long 0x400482a4 0x00000140
    monitor long 0x400482a8 0x00000140
    monitor long 0x400482ac 0x00000140
    monitor long 0x400482b0 0x00000140
    monitor long 0x400482b4 0x00000140
    monitor long 0x400482b8 0x00000140
    monitor long 0x400482bc 0x00000140
    monitor long 0x400482c0 0x00000140
    monitor long 0x400482c4 0x00010140
    monitor long 0x400482c8 0x00010140
    monitor long 0x400482cc 0x00000140
    monitor long 0x400482d0 0x00000140
    monitor long 0x400482d4 0x00000140
    monitor long 0x400482d8 0x00000140
    monitor long 0x400482dc 0x00000140
    monitor long 0x400482e0 0x00000140
    #--------------------------------------------------------------------------


    #--------------------------------------------------------------------------
    # ddr_init2
    #--------------------------------------------------------------------------
    monitor long 0x400ae000 0x00000600
    monitor long 0x400ae008 0x00000020
    monitor long 0x400ae028 0x0000007c
    monitor long 0x400ae02c 0x00013880
    monitor long 0x400ae030 0x0000050c
    monitor long 0x400ae034 0x15040404
    monitor long 0x400ae038 0x1406040f
    monitor long 0x400ae040 0x04040000
    monitor long 0x400ae044 0x006db00c
    monitor long 0x400ae048 0x00000403
    monitor long 0x400ae050 0x01000403
    monitor long 0x400ae054 0x06060101
    monitor long 0x400ae058 0x000b0000
    monitor long 0x400ae05c 0x03000200
    monitor long 0x400ae060 0x00000006
    monitor long 0x400ae064 0x00010000
    monitor long 0x400ae068 0x0c28002c
    monitor long 0x400ae070 0x00000005
    monitor long 0x400ae074 0x00000003
    monitor long 0x400ae078 0x0000000a
    monitor long 0x400ae07c 0x00440200
    monitor long 0x400ae084 0x00010000
    monitor long 0x400ae088 0x00050500
    monitor long 0x400ae098 0x00000100
    monitor long 0x400ae09c 0x04001002
    monitor long 0x400ae0a4 0x00000001
    monitor long 0x400ae0b4 0x00000000
    monitor long 0x400ae0b8 0x00000000
    monitor long 0x400ae0bc 0x00000000
    monitor long 0x400ae0c0 0x00460420
    monitor long 0x400ae0c4 0x00000000
    monitor long 0x400ae0cc 0x00000000
    monitor long 0x400ae0d0 0x00000000
    monitor long 0x400ae0e4 0x00000000
    monitor long 0x400ae0e8 0x00000000
    monitor long 0x400ae108 0x01000200
    monitor long 0x400ae10c 0x02000040
    monitor long 0x400ae114 0x00000200
    monitor long 0x400ae118 0x00000040
    monitor long 0x400ae11c 0x00000000
    monitor long 0x400ae120 0x01000000
    monitor long 0x400ae124 0x0a010300
    monitor long 0x400ae128 0x0101ffff
    monitor long 0x400ae12c 0x01010101
    monitor long 0x400ae130 0x03030101
    monitor long 0x400ae134 0x01000101
    monitor long 0x400ae138 0x0000000c
    monitor long 0x400ae13c 0x01000000
    monitor long 0x400ae15c 0x01010000
    monitor long 0x400ae160 0x00040000
    monitor long 0x400ae164 0x00000002
    monitor long 0x400ae16c 0x00020000
    monitor long 0x400ae170 0x00000000
    monitor long 0x400ae180 0x00002819
    monitor long 0x400ae1a4 0x00202000
    monitor long 0x400ae1a8 0x20200000
    monitor long 0x400ae1b8 0x00002020
    monitor long 0x400ae1c8 0x00202000
    monitor long 0x400ae1cc 0x20200000
    monitor long 0x400ae1d4 0x00000101
    monitor long 0x400ae1d8 0x01010000
    monitor long 0x400ae1dc 0x00000000
    monitor long 0x400ae1e0 0x02020000
    monitor long 0x400ae1e4 0x00000202
    monitor long 0x400ae1e8 0x01010064
    monitor long 0x400ae1ec 0x00010101
    monitor long 0x400ae1f0 0x00000064
    monitor long 0x400ae1f4 0x00000000
    monitor long 0x400ae1f8 0x00000b00
    monitor long 0x400ae1fc 0x00000000
    monitor long 0x400ae20c 0x00000000
    monitor long 0x400ae210 0x00000506
    monitor long 0x400ae224 0x00020000
    monitor long 0x400ae22c 0x04070303
    monitor long 0x400ae220 0x00000000
    monitor long 0x400ae268 0x68200000
    monitor long 0x400ae26c 0x00000202
    monitor long 0x400ae278 0x00000006
    monitor long 0x400ae27c 0x00000006
    #--------------------------------------------------------------------------


    #--------------------------------------------------------------------------
    # ddr_phy_init2
    #--------------------------------------------------------------------------
    monitor long 0x400ae400 0x00002613
    monitor long 0x400ae440 0x00002613
    monitor long 0x400ae480 0x00002613
    monitor long 0x400ae4c0 0x00002613

    monitor long 0x400ae404 0x00002615
    monitor long 0x400ae444 0x00002615
    monitor long 0x400ae484 0x00002615
    monitor long 0x400ae4c4 0x00002615

    monitor long 0x400ae408 0x01210080
    monitor long 0x400ae448 0x01210080
    monitor long 0x400ae488 0x01210080
    monitor long 0x400ae4c8 0x01210080

    monitor long 0x400ae40c 0x0001012a
    monitor long 0x400ae44c 0x0001012a
    monitor long 0x400ae48c 0x0001012a
    monitor long 0x400ae4cc 0x0001012a

    monitor long 0x400ae410 0x00012020
    monitor long 0x400ae450 0x00012020
    monitor long 0x400ae490 0x00012020
    monitor long 0x400ae4d0 0x00012020

    monitor long 0x400ae4c8 0x00001100
    #--------------------------------------------------------------------------


    #--------------------------------------------------------------------------
    # ddr_init2 end
    #--------------------------------------------------------------------------

    monitor long 0x400ae148 0x1fffffff
    monitor long 0x400ae000 0x00000601

    monitor waithalt  100
    #--------------------------------------------------------------------------


end
#==============================================================================






#==============================================================================
# LOAD_INTRAM <my_app.elf>
define LOAD_INTRAM
#==============================================================================
    if $argc == 0
        print "missing elf filename"
    else

        # remote connection
        target remote localhost:2331

        # reset type - hw reset by DBGRQ
        monitor reset 5

        # halt cpu
        monitor halt

        # load image, 'pc' is set automatically
        # no need to setup 'cpsr' register, processor runs in default SVC mode
        load $arg0

        # load symbols
        file $arg0

        INIT_REGISTERS
    end
end
#==============================================================================






#==============================================================================
# LOAD_DDR <my_app.elf>
define LOAD_DDR
#==============================================================================
    if $argc == 0
        print "missing elf filename"
    else

        target remote localhost:2331

        # reset type - hw reset by DBGRQ
        monitor reset 5

        # halt cpu
        monitor halt

        SETUP_DDR

        # load image, 'pc' is set automatically
        load $arg0

        # load symbols
        file $arg0

        INIT_REGISTERS
    end
end
#==============================================================================






