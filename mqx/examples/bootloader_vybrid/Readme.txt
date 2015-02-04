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
     fromelf.exe --bincombined --output=bootloader_vybrid_twrvf65gs10_a5.bin bootloader_vybrid_twrvf65gs10_a5.axf
     
  Build bootloader_vybrid application in the CodeSourcery/Makefiles
  a) Invoke 'make' process from make/ directory: 
    For A5:  
     mingw32-make BOARD=twrvf65gs10_a5 CONFIG=debug TOOL=gcc_cs LINKER_FILE=../sram_gcc_cs_a5.ld build
    For M4:  
     mingw32-make BOARD=twrvf65gs10_m4 CONFIG=debug TOOL=gcc_cs LINKER_FILE=../sram_gcc_cs_m4.ld build
  b) Create binary:
     arm-none-eabi-objcopy.exe -O binary bootloader_vybrid_twrvf65gs10_a5.elf bootloader_vybrid_twrvf65gs10_a5.bin 
    
3) Write image to SD card bootable format
    It depends on which OS you're using. It only lists examples for Windows and Linux respectively for your reference.

  Keep in mind the bootloader MUST BE written to the card at the right offset of 1024 bytes from the begining.

  > Use ddcopy.exe to copy image file to SDCARD (<MQX_install_dir>/tools/ddcopy/ddcopy.exe) on Windows
      infile        - image to be copied
      outdevice     - sdcard disk name
      seek          - offset of image on medium. For SD Card used on Vybrid always 0x400
      obs           - output block size. Use 512 for SD Card
      
    usage: ddcopy.exe infile=bootloader_vybrid_twrvf65gs10_a5.bin outdevice=<your disk name:> seek=0x400 obs=512      

  > Use dd to copy image file to SDCARD on linux
      if            - image to be copied
      of            - output device name, i.e. then device name of sdcard found on linux, like /dev/sdb, make sure the device name is correct
      seek          - skip blocks (with size of obs) at the start of output device.
      obs           - output block size.
    (for more detail please refer to the man page of dd)

    usage: dd if=bootloader_vybrid_twrvf65gs10_a5.bin of=/dev/sdX seek=2 obs=512; sync

    Note:
        1. It needs root privileges to run the command.
        2. The following "sync" just makes sure all the data would be written to the card on its return.

4) Run applications on Vybrid loaded by this bootloader
    4.1) Build application images and setup boodloader and run it on Vybrid board 
        a) Build the the applications you want to run on A5 and M4 core to binary format (.bin) and store them to SD card FAT system from your PC
        b) Copy configuration file (setup_a5.ini for A5, setup_m4.ini for M4) to sdcard, and modify it to set the paths to your application binaries on SD Card.
        c) Remove SD Card from PC and plug it to Micro SD Card slot on your Vybrid board
        d) Power up the Vybrid board and bootloader shoud print out following message on default console (RS232 TWR-SER) and start selected M4 and A5 applications
              Mounting filesystem
              Starting bootloader 

    4.2) To run different application repeat step 4) 
    Note: make sure [boot] should be the first label in the configuration file, otherwise it will fail.

5) Flash XIP applications to qspi nor flash on Vybrid board by this bootloader
    5.1) Build applications
    5.2) On your PC, Copy configuration file (setup_a5.ini for A5, setup_m4.ini for M4) to sdcard, and modify it to set the following info:
        5.2.1) New label
            5.2.1.1) [flash_image], which indicates this is a configuration to flash images.
                    Note: make sure this is the first label in the configuration file.
        5.2.2) New properties
            5.2.2.1) flash: the image to be flashed needs to be assigned.
            5.2.2.2) path: the paths to your application binaries on SD Card.
            5.2.2.3) flash_id: the flash id to which the image will be flashed to.
            5.2.2.4) flash_base: the address to which the image will be flashed to.
        5.2.3) The image without the property of base or core will not be recorded for application images, but it will be flashed still.
            So that it can distinguish the image for bootloader from the one for applications.
    5.3) Remove SD Card from PC and plug it to Micro SD Card slot on your Vybrid board
    5.4) Power up the Vybrid board and bootloader shoud print out following message on default console (RS232 TWR-SER), take A5 dual core bootloader Release version for example:
            Bootloader from A5 as primary core, dual core
            ======================
            Mounting filesystem
            Starting bootloader
            Ready to flash images
        Note: Debug version will show other debug information.
    5.5) It will print out the following, taking 3 images being flashed for example.
            Flashed 3 images in total, 2 recorded.
    Note:
        1. It can flash 3 images (one image for bootloader, the other two images are applications for different cores) at most a time.
        2. It will clean the target nor flash before flashing.

------------------------------------------------------------------------------------------------
Notes:
-  If application exceeds defined code area, try to disable unnecessary peripheral (i2c, sai, ...) in user_config and rebuild bsp library.
-  Ensure that applications do not use memory region reserved by bootloader by default 0x3f07ebf0 - 0x3f07eff0.
-  The application images(ROM) must not exceed bootloader region (0x3f040000 - 0x3f07eff0). This region can be used for application RAM. 
-  All mqx applications for A5 DDram and M4 Sram targets should work without any changes.


