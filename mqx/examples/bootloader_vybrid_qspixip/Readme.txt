How-to use Vybrid dual core bootloader

1) Prepare sdcard (create single FAT partition with 2M offset from MBR, you can use Microsoft Windows 7 'diskpart' tool.)

  a) Opend 'command prompt', and run 'diskpart'
  b) 'list disk' - show all available disks
  c) 'select disk [num]' - choose disk number which represent your sdcard, e.g. 1
  d) 'clean all' - remove all partitions on disk 1, it will take a while
  e) 'create partition primary offset=2048 size=512' will create primary partition of 512 MB size with offset 2048 KB. Offset should be at the least 1MB.
  f) Remove sdcard from slot and plug it again and now you can format partition to FAT filesystem

2) Build bootloader_vybrid application in the IAR WorkBench for ARM IDE
  a) Build bsp and psp with the following settings to default bsp package:
    a.1) set BSPCFG_ENABLE_QUADSPI0 to 1 in related user_config.h for the board selected.
    a.2) disable parallel mode for QSPI0, in the file mqx/source/bsp/<board>/init_qspi.c, in the following struct:
        static const QuadSPI_INIT_STRUCT _bsp_quadspi0_init_data = {
            ...
            _bsp_quadspi0_flash_device,         /* flash device information */
            &_bsp_quadspi0_flash_cmd,           /* flash command */
            TRUE,                               /* Parallel mode */
        };
        change it to:
        static const QuadSPI_INIT_STRUCT _bsp_quadspi0_init_data = {
            ...
            _bsp_quadspi0_flash_device,         /* flash device information */
            &_bsp_quadspi0_flash_cmd,           /* flash command */
            FALSE,                               /* Parallel mode */ /* this is the only place needs to be modified */
        };
  b) Open bootloader project and switch to Int RAM target.
  c) Set 'Output converter' to binary format in project properties
  d) Build the Int RAM target (output is by default ./<target>/Exe/bootloader_vybrid_twrvf65gs10_a5.bin)
  
   Build bootloader_vybrid application in the ARM DS5 IDE
  a) Use DS5 "fromelf" utility to create binary.
     fromelf.exe --bincombined --output=bootloader_vybrid_qspixip_twrvf65gs10_a5.bin bootloader_vybrid_qspixip_twrvf65gs10_a5.axf  

   Build bootloader_vybrid application in the CodeSourcery/Makefiles
  a) Invoke 'make' process from make/ directory: 
    For A5:  
     mingw32-make BOARD=twrvf65gs10_a5 CONFIG=debug TOOL=gcc_cs LINKER_FILE=../sram_gcc_cs_a5.ld build
    For M4:  
     mingw32-make BOARD=twrvf65gs10_m4 CONFIG=debug TOOL=gcc_cs LINKER_FILE=../sram_gcc_cs_m4.ld build
  b) Create binary:
     arm-none-eabi-objcopy.exe -O binary bootloader_vybrid_qspixip.elf bootloader_vybrid_qspixip.bin 
  
3) program the bootloader_vybrid_twrvf65gs10_a5.bin or bootloader_vybrid_twrvf65gs10_m4.bin into QSPI flash. 
    please refer to section 5 in mqx/examples/bootloader_vybrid/Readme.txt, especially item 5.2.3.

4) Build application images and setup boodloader and run it on Vybrid board 
    a) Build the the applications you want to run on A5 and M4 core to binary format (.bin) and store them to SD card FAT system from your PC
    b) Copy configuration file (setup.ini for A5, setup_m4.ini for M4) to sdcard, and modify it to set the paths to your application binaries on SD Card.
    c) Remove SD Card from PC and plug it to Micro SD Card slot on your Vybrid board
    d) Set the boot pins for qspi boot. In TWR board REVG, set the j22 with 1-2 on, 3-4 off, 5-6 off, 7-8 off, 9-10off, 11-12 off.
    In AutoEVB board, set the sw10,sw11,sw12,sw13 all off. For make the A5 primary, you should set the j55 2-3 j56 1-2. For m4 primary, you should set j55 j56 2-3.
      
    e) Power up the Vybrid board and bootloader shoud print out following message on default console (RS232 TWR-SER) and start selected M4 and A5 applications
          Mounting filesystem
          Starting bootloader 

5) To run different application repeat step 4) 

------------------------------------------------------------------------------------------------
Notes:
-  If application exceeds defined code area, try to disable unnecessary peripheral (i2c, sai, ...) in user_config and rebuild bsp library.
-  Ensure that applications do not use memory region reserved by bootloader by default 0x3f07ebf0 - 0x3f07eff0.
-  The application images(ROM) must not exceed bootloader region (0x3f040000 - 0x3f07eff0). This region can be used for application RAM. 
-  All mqx applications for A5 DDram and M4 Sram targets should work without any changes.


