This folder contains the mass build project for all tool chains supported in MQX RTOS. 

Before you start read following documentation:
  Build, Run on target and debugging
  - /doc/MQX_Getting_Started.pdf file provides general information on MQX build process
  - /doc/<tool>/MQX_<TOOL>_Getting_started.pdf file provides step-by-step guide for your favorite tool chain.
  Board related information  and jumper settings
  - /doc/MQX_Getting_Started.pdf chapter "Board Specific information related to MQX" 

This folder contains makefiles in following structure: 

build
+---common
|   +---make        ... shared Makefiles with global settings, variables and paths
+---<board1>        ... board-specific folder
|   +---make        ... folder contains mass-build Makefile for all libraries
|       +---tools   ... tool-specific global settings, variables and paths
|       +---bsp     ... BSP Library Makefile
|       +---psp     ... PSP Library Makefile
|       +---mfs     ... MFS Library Makefile
|       +---rtcs    ... RTCS Library Makefile
|       +---shell   ... Shell Library Makefile
|       +---usbd    ... USB Device Library Makefile
|       +---usbh    ... USB Host Library Makefile
|   +---iar         ... folder contains workspace with mass-build configuration for MQX libraries IAR EWARM
|   +---uv4         ... folder contains workspace with mass-build configuration for MQX libraries KEIL uv4 IDE
|   +---cw10Ggcc    ... folder contains workspace with mass-build configuration for MQX libraries CodeWarrior 10 IDE (GCC compiler)
+---<board2>  
    +---make
        ...

*** MAKE usage ***
  - See step-by-step guide in /doc/tools/gnu/MQX_GNU_Getting_Started.pdf
  - Navigate to the <mqx_install_dir>/build/<board>/make directory 
  - Run the following command to build MQX libraries using the GCC from CW10.3 or later toolchain
    C:\MinGW\bin\mingw32-make.exe build TOOL=gcc_cw CONFIG=debug 
  Note: 
  - Please use the mingw32-make version 3.8.2 or higher. Download the latest version from http://sourceforge.net/projects/mingw/. 
  - Prior the build you should specify the path (TOOLCHAIN_ROOTDIR variable) to your build tools in the <mqx_install_dir>/build/common/make/global.mak. To get the default path to the toolchain 
    navigate to the toolchain installation directory and execute "command" on the Windows Command Prompt.
