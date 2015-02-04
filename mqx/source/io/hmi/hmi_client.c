/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*
*   This file contains the implementation of HMI clients
*
*
*END************************************************************************/
#include "hmi_client.h"

#define HMI_START_PROVIDER_COUNT       2
#define HMI_GROW_UP_PROVIDER_COUNT     2

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_client_init
* Returned Value   : allocated HMI_CLIENT_STRUCT_PTR
* Comments         :
*   This function adds provider.
*
*END*--------------------------------------------------------------------*/
HMI_CLIENT_STRUCT_PTR hmi_client_init(void)
{
    HMI_CLIENT_STRUCT_PTR       btnled_struct;
    HMI_PROVIDER_STRUCT_PTR     provider_table;

    btnled_struct = _mem_alloc_system_zero(sizeof(HMI_CLIENT_STRUCT));
    if(btnled_struct == NULL)
    {
        return NULL;
    }
    /* allocate provider tablet */
    provider_table = _mem_alloc_system_zero(HMI_START_PROVIDER_COUNT * sizeof(HMI_PROVIDER_STRUCT_PTR));
    if(provider_table == NULL)
    {
        _mem_free(btnled_struct);
        return NULL;
    }
    btnled_struct->PROVIDER_TABLE = (void *)provider_table;
    btnled_struct->MAX_PROVIDER_COUNT = HMI_START_PROVIDER_COUNT;

    return btnled_struct;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_add_provider
* Returned Value   : TRUE if adding success, else FALSE
* Comments         :
*   This function adds provider
*
*END*--------------------------------------------------------------------*/
bool hmi_add_provider(HMI_CLIENT_STRUCT_PTR client_handle, HMI_PROVIDER_STRUCT_PTR provider_handle)
{
    bool result = FALSE;
    uint32_t i;

    if((provider_handle == NULL) || (client_handle==NULL))
    {
        return result;
    }
    /* add provider into the table*/
    for(i=0; i<client_handle->MAX_PROVIDER_COUNT; i++)
    {
        if(client_handle->PROVIDER_TABLE[i] == NULL)
        {
            result = TRUE;
            client_handle->PROVIDER_TABLE[i] = provider_handle;
            client_handle->PROVIDER_TABLE[i]->ATTACH_CLIENT(provider_handle, client_handle);
            if (client_handle->PROVIDER_TABLE[i]->INSTALL_INTERRUPT_PTR != NULL)
            {
                /* register callback */
                client_handle->PROVIDER_TABLE[i]->INSTALL_INTERRUPT_PTR(provider_handle);
            }
            break;
        }
    }
    /* if array is full do re-alloc */
    if(result == FALSE)
    {
        HMI_PROVIDER_STRUCT_PTR     provider_table_new;

        provider_table_new = _mem_alloc_system_zero(client_handle->MAX_PROVIDER_COUNT * HMI_GROW_UP_PROVIDER_COUNT * sizeof(HMI_PROVIDER_STRUCT_PTR));
        if(provider_table_new == NULL)
        {
            return FALSE;
        }
        /* copy smaller array to bigger array*/
        _mem_copy(client_handle->PROVIDER_TABLE, provider_table_new, client_handle->MAX_PROVIDER_COUNT * sizeof(client_handle->PROVIDER_TABLE));
        _mem_free(client_handle->PROVIDER_TABLE);
        client_handle->PROVIDER_TABLE = (void *) provider_table_new;
        client_handle->MAX_PROVIDER_COUNT = client_handle->MAX_PROVIDER_COUNT * HMI_GROW_UP_PROVIDER_COUNT;
        for(i=0; i<client_handle->MAX_PROVIDER_COUNT; i++)
        {
            if(client_handle->PROVIDER_TABLE[i] == NULL)
            {
                result = TRUE;
                client_handle->PROVIDER_TABLE[i] = provider_handle;
                client_handle->PROVIDER_TABLE[i]->ATTACH_CLIENT(provider_handle,client_handle);
                if (client_handle->PROVIDER_TABLE[i]->INSTALL_INTERRUPT_PTR != NULL)
                {
                    /* register callback */
                    client_handle->PROVIDER_TABLE[i]->INSTALL_INTERRUPT_PTR(provider_handle);
                }
                break;
            }
        }
    }

    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_remove_provider
* Returned Value   : TRUE if removing success, else FALSE
* Comments         :
*   This function removes provider
*
*END*--------------------------------------------------------------------*/
bool hmi_remove_provider(HMI_CLIENT_STRUCT_PTR client_handle, HMI_PROVIDER_STRUCT_PTR provider_handle)
{
    bool result = FALSE;
    uint32_t i;

    if((provider_handle == NULL) || (client_handle==NULL))
    {
        return result;
    }
    /* remove provider from the table */
    for(i=0; i<client_handle->MAX_PROVIDER_COUNT; i++)
    {
        if(client_handle->PROVIDER_TABLE[i] == provider_handle)
        {
            result = TRUE;
            if (client_handle->PROVIDER_TABLE[i]->UNINSTALL_INTERRUPT_PTR != NULL)
            {
                /* unregister callback */
                client_handle->PROVIDER_TABLE[i]->UNINSTALL_INTERRUPT_PTR(provider_handle);
            }
            client_handle->PROVIDER_TABLE[i] = NULL;
            break;
        }
    }
    return result;
}
