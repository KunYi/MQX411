/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
IO expander driver example.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <io_expander.h>


int32_t example()
{
    MQX_FILE_PTR file[3];
    int32_t ret;
    int32_t i;

    printf("IO expander EXAMPLE BEGIN\n");
    
    file[0] = fopen("ioexp0:", NULL);
    if (file[0] == NULL) {
        printf("open ioexp0 file 0 failed\n");
        return -1;
    }

    file[1] = fopen("ioexp0:", NULL);
    if (file[1] == NULL) {
        printf("open ioexp0 file 1 failed\n");
        return -1;
    }

    file[2] = fopen("ioexp0:", NULL);
    if (file[2] == NULL) {
        printf("open ioexp0 file 2 failed\n");
        return -1;
    }

    ret = ioctl(file[0], IO_IOCTL_IOEXP_SET_PIN_NO,
         (void *)0);
    if (ret != MQX_OK)
        printf("test failed, file 0 IO_IOCTL_IOEXP_SET_PIN_NO %d\n", ret);
    ret = ioctl(file[0], IO_IOCTL_IOEXP_SET_PIN_DIR_OUT, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 0 IO_IOCTL_IOEXP_SET_PIN_DIR_OUT %d\n", ret);
    ret = ioctl(file[0], IO_IOCTL_IOEXP_SET_PIN_VAL_LOW, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 0 IO_IOCTL_IOEXP_SET_PIN_VAL_LOW %d\n", ret);

    _time_delay(100);
    ret = ioctl(file[0], IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 0 IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH %d\n", ret);
    _time_delay(200);

    ret = ioctl(file[1], IO_IOCTL_IOEXP_SET_PIN_NO,
         (void *)1);
    if (ret != MQX_OK)
        printf("test failed, file 1 IO_IOCTL_IOEXP_SET_PIN_NO %d\n", ret);
    ret = ioctl(file[1], IO_IOCTL_IOEXP_SET_PIN_DIR_OUT, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 1 IO_IOCTL_IOEXP_SET_PIN_DIR_OUT %d\n", ret);
    ret = ioctl(file[1], IO_IOCTL_IOEXP_SET_PIN_VAL_LOW, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 1 IO_IOCTL_IOEXP_SET_PIN_VAL_LOW %d\n", ret);

    _time_delay(100);
    ret = ioctl(file[1], IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 1 IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH %d\n", ret);
    _time_delay(200);

    ret = ioctl(file[2], IO_IOCTL_IOEXP_SET_PIN_NO,
         (void *)2);
    if (ret != MQX_OK)
        printf("test failed, file 2 IO_IOCTL_IOEXP_SET_PIN_NO %d\n", ret);
    ret = ioctl(file[2], IO_IOCTL_IOEXP_SET_PIN_DIR_OUT, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 2 IO_IOCTL_IOEXP_SET_PIN_DIR_OUT %d\n", ret);
    ret = ioctl(file[2], IO_IOCTL_IOEXP_SET_PIN_VAL_LOW, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 2 IO_IOCTL_IOEXP_SET_PIN_VAL_LOW %d\n", ret);

    _time_delay(100);
    ret = ioctl(file[2], IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 2 IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH %d\n", ret);
    _time_delay(200);

    fclose(file[0]);
    fclose(file[1]);
    fclose(file[2]);


    file[0] = fopen("ioexp0:", NULL);
    if (file[0] == NULL) {
        printf("open ioexp0 file 0 failed\n");
        return -1;
    }

    file[1] = fopen("ioexp0:", NULL);
    if (file[1] == NULL) {
        printf("open ioexp0 file 1 failed\n");
        return -1;
    }

    for (i=0; i<8; i++) {
        ret = ioctl(file[0], IO_IOCTL_IOEXP_SET_PIN_NO,
             (void *)i);
        if (ret != MQX_OK)
            printf("1) test failed, file 0 IO_IOCTL_IOEXP_SET_PIN_NO %d\n", ret);

        ret = ioctl(file[1], IO_IOCTL_IOEXP_SET_PIN_NO,
             (void *)i);
        if (MQX_OK == ret)
            printf("1) test failed, file 1 conflicts with file 0 %d\n", ret);
    }

    for (i=0; i<8; i++) {
        ret = ioctl(file[0], IO_IOCTL_IOEXP_SET_PIN_NO,
             (void *)((i+1)%8));
        if (ret != MQX_OK)
            printf("2) test failed, file 0 IO_IOCTL_IOEXP_SET_PIN_NO %d\n",
                ret);

        ret = ioctl(file[1], IO_IOCTL_IOEXP_SET_PIN_NO,
             (void *)i);
        if (ret != MQX_OK)
            printf("2) test failed, no conflict with file 0 and file 1 %d\n",
                ret);
    }

    ret = ioctl(file[1], IO_IOCTL_IOEXP_CLEAR_PIN_NO, NULL);
    if (ret != MQX_OK)
        printf("test failed, file 1 IO_IOCTL_IOEXP_CLEAR_PIN_NO %d\n",
                ret);

    for (i=0; i<8; i++) {
        ret = ioctl(file[0], IO_IOCTL_IOEXP_SET_PIN_NO,
             (void *)i);
        if (ret != MQX_OK)
            printf("3) test failed, file 0 IO_IOCTL_IOEXP_SET_PIN_NO %d\n",
                ret);

        ret = ioctl(file[0], IO_IOCTL_IOEXP_CLEAR_PIN_NO, NULL);
        if (ret != MQX_OK)
            printf("3) test failed, file 0 IO_IOCTL_IOEXP_CLEAR_PIN_NO %d\n",
                ret);

        ret = ioctl(file[1], IO_IOCTL_IOEXP_SET_PIN_NO,
             (void *)i);
        if (ret != MQX_OK)
            printf("3) test failed, no conflict with file 0 and file 1 %d\n",
                ret);
    }

    fclose(file[0]);
    fclose(file[1]);

    printf("IO expander EXAMPLE END\n");
    
    return 0;
}

extern void main_task(uint32_t);


const TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{
   /* Task Index,   Function,   Stack,  Priority, Name,   Attributes,          Param, Time Slice */
    { 10,           main_task,  2000,   8,        "Main", MQX_AUTO_START_TASK, 0,     0},
    { 0 }
}; 

void main_task
   (
      uint32_t initial_data
   )
{
    example();
}


