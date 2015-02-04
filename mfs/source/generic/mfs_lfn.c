/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains functions related to using long filenames of up to
*   255 characters.
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_to_sfn
* Returned Value   :  none
* Comments  :  Will take a long filename (actual file name or directory name
*   and will parse it into the corresponding 8.3 name. It is assumed that
*   there is space allocated for the short name.
*END*---------------------------------------------------------------------*/

_mfs_error  MFS_lfn_to_sfn
    (
    char    *l_fname,    /*[IN] the long filename */
    char    *s_fname     /*[OUT] the short filename */
    )
{
    int32_t      i;
    int32_t      len;
    char    *temp_lname1, *temp_lname2;
    char    *new, *orig, *valid_dot = NULL;
    _mfs_error  error_code;

    error_code = MFS_alloc_2paths(&temp_lname1,&temp_lname2);
    if ( error_code != MFS_NO_ERROR )
    {
        return( error_code );
    }

    orig = l_fname;
    new = temp_lname1;

    /* 1. Remove all spaces and translate all unwanted 8.3 chars into '_' */
    for ( i = 0 ; i < FILENAME_SIZE; i++ )
    {
        if ( *orig == '\0' )
        {
            break;
        }

        switch (*orig)
        {
            case '[':
            case ']':
            case ';':
            case ',':
            case '+':
            case '=':
                *new++ = '_';
                orig++;
                break;

            case ' ':
                orig++;
                break;

            default:
                *new++ = *orig++;
                break;
        }
    }
    *new-- = '\0';

    /* Remove all of the trailing '.'  */
    while ( *new == '.' )
    {
        *new-- = '\0';
    }

    /*
    ** Locate the valid '.' if there is one
    */
    while ( new != temp_lname1 )
    {
        if ( *new == '.' )
        {
            valid_dot = new;
            break;
        }
        new--;
    }

    /* Now we can start copying to the new name, without the extra periods */
    new = temp_lname2;
    orig = temp_lname1;
    while ( *orig != '\0' )
    {
        if ( *orig == '.' )
        {
            if ( orig == valid_dot )
            {
                *new++ = *orig++;
            }
            else
            {
                orig++;
            }
        }
        else
        {
            *new++ = *orig++;
        }
    }
    *new = '\0';

    /* Now we create final 8.3 name */
    for ( i = 0 ; i <= 5 && temp_lname2[i] != '.' && temp_lname2[i] != '\0'; i++ )
    {
        s_fname[i] =  temp_lname2[i];
    }

    len = i;
    s_fname[len++] = '~';
    s_fname[len++] = '1';
    if ( valid_dot != NULL )
    {
        s_fname[len++] = '.';
        orig = temp_lname2;
        while ( *orig++ != '.' )
            /* void */ ;
        for ( i = 0 ; i <= 2 && orig[i] != '\0'; i++ )
        {
            s_fname[len++] = orig[i];
        }
    }

    s_fname[len] = '\0';

    /* Capitalize the name */
    for ( i = 0; i <= len - 1; i++ )
        if ( s_fname[i] >= 'a' && s_fname[i] <= 'z' )
        {
            s_fname[i] -= 32;
        }

    MFS_free_path(temp_lname1);
    MFS_free_path(temp_lname2);

    return( MFS_NO_ERROR );
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_is_valid_lfn
* Returned Value   :  bool
* Comments  :  Will take a long filename (actual file name or directory name)
*   and will validate it by checking for length and for invalid characters.
*END*---------------------------------------------------------------------*/

bool  MFS_is_valid_lfn
    (
    char *l_fname /*[IN] the long filename */
    )
{
    register uint32_t    len;
    register uint32_t    i;
    register char        c;
#if defined(__GNUC__)
    // FIXME: workaround for gcc release version of qspi flash xip images, to
    // make sure the address of the string is 4-byte aligned. Otherwise, it
    // might be put a non-4-byte aligned address, then the access will abort.
    static const char    illegal[] __attribute__((aligned(0x4))) = "*?<>|\":/\\";
#else
    const char    illegal[] = "*?<>|\":/\\";
#endif
    bool              ok_char_flag;

    ok_char_flag = FALSE;
    len = 0;

    /* process characters until null term found */
    while ('\0' != (c = *l_fname++))
    {
        /* check maximum lfn length */
        if (++len > FILENAME_SIZE)
        {
            return (FALSE);
        }

        /* dot and space are allowed but at least one different character is necessary to form a valid lfn */
        if ( c == '.' || c == ' ' )
        {
            continue;
        }

        /* check c against list of illegal characters */
        for ( i=0 ; illegal[i]; i++ )
        {
            if ( c == illegal[i] )
            {
                return (FALSE);
            }
        }

        /* valid character found */
        ok_char_flag = TRUE;
    }

    return (ok_char_flag);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_is_autogenerated_name
* Returned Value   :  bool
* Comments  :  Will take a short filename (actual file name or directory name)
*   and will determine if it is an autogenerated name. If it is, LFN entries
*   should exist for it, if not no LFN entries should exist.
*
*  The autogenerated name has the following format:
*     [
*
*END*---------------------------------------------------------------------*/

bool  MFS_is_autogenerated_name
    (
    char    *s_fname /*[IN] the long filename */
    )
{
    uint32_t  i, digits=0;
    bool  found_tilde = FALSE;


    for ( i=0;i<SFILENAME_SIZE;i++ )
    {
        if ( !found_tilde )
        {
            // Looking for '~'
            switch ( s_fname[i] )
            {
                case '[':
                case ']':
                case ';':
                case ',':
                case '+':
                case '=':
                case ' ':
                case '\0':
                    // All invalid before ~ in an autogenerated name.
                    return FALSE;

                case '~':
                    found_tilde=TRUE;
                    break;
            }
        }
        else
        {
            // Found '~', counting numbers
            if ( (s_fname[i]=='.')  || (s_fname[i]==0) )
            {
                // Valid if we saw at least on digit
                return(digits!=0);
            }

            if ( (s_fname[i]>='0') && (s_fname[i]<='9') )
            {
                digits++;
                if ( digits>6 ) return FALSE;
            }
        }
    }
    return FALSE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_dirname_valid
* Returned Value   :  bool - TRUE if valid LFN directory name.
* Comments  :
*        Allows . and ..
*END*---------------------------------------------------------------------*/

bool MFS_lfn_dirname_valid
    (
     char *filename   /*[IN] filename to validate */
    )
{

    if (filename[0] == '.')
    {
        if (filename[1] == '\0')
        {
            return TRUE; // '.'
        }
        if (filename[1] == '.' && filename[2] == '\0')
        {
            return TRUE; // '..'
        }
    }

    return MFS_is_valid_lfn(filename);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_match
* Returned Value   :  bool - TRUE if the dir_entry_ptr points to the entry
*                     actually corresponding to the LFN.
* Comments  : Will read the previous entries and reconstruct the LFN
*END*---------------------------------------------------------------------*/

bool MFS_lfn_match
    (
    char  *search_ptr,   /* [IN] the search string */
    char  *lfn,          /* [IN] the long file name to compare */
    uint32_t   level         /* [IN] the level of recursion or the number of * encountered */
    )
{
    char cw;
    char ct;

    if ( *search_ptr == '\0' )
    {
        return TRUE;
    }
    if ( *lfn == '\0' )
    {
        return FALSE;
    }

    while ( *search_ptr )
    {
        if ( *lfn == '\0' )
        {
            if ( (*search_ptr == '*') || (*search_ptr == '?') )
            {
                if ( !*(search_ptr+1) )
                {
                    return TRUE;
                }
            }
            return FALSE;
        }
        else if ( *search_ptr == '?' )
        {
            search_ptr++;
            lfn++;
        }
        else if ( *search_ptr == '*' )
        {
            if ( level > 3 )
            {
                return FALSE;
            }
            search_ptr++;
            while ( *lfn )
            {
                if ( MFS_lfn_match(search_ptr, lfn, level + 1) )
                {
                    return TRUE;
                }
                lfn++;
            }
        }
        else
        {
            cw = CAPITALIZE(*search_ptr);
            ct = CAPITALIZE(*lfn);
            if ( cw != ct )
            {
                return FALSE;
            }
            else
            {
                search_ptr++;
                lfn++;
            }
        }
    }
    if ( *lfn )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


#if MFS_EXTRACT_UTF8
#include "unicode.h"
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_extract
* Returned Value   :
*
* Comments  : Takes a LFN directory entry, extracts the 13 UTF16 code units
*             from the entry and stores it using CESU-8 encoding.
*END*---------------------------------------------------------------------*/
int MFS_lfn_extract
    (
    MFS_LNAME_ENTRY_PTR     lname_entry,    /* [IN] pointer to the entry to read UNICODE name from */
    char                    *filename       /* [OUT] buffer to store the UTF-8 chars in  */
    )
{
    int i;
    int len;

    char *output;
    int output_len;

    unsigned char *utf16_chunk;
    uint16_t utf16_unit;

    output = filename;
    output_len = 0;

    utf16_chunk = lname_entry->NAME0_4;
    for (i=0; i<10; i+=2) {
        utf16_unit = utf16_chunk[0] | (utf16_chunk[1] << 8);
        utf16_chunk += 2;
        if (utf16_unit == 0 || utf16_unit == 0xFFFF) {
            return output_len;
        }
        len = utf8_encode(utf16_unit, output, 3);
        output += len;
        output_len += len;
    }

    utf16_chunk = lname_entry->NAME5_10;
    for (i=0; i<12; i+=2) {
        utf16_unit = utf16_chunk[0] | (utf16_chunk[1] << 8);
        utf16_chunk += 2;
        if (utf16_unit == 0 || utf16_unit == 0xFFFF) {
            return output_len;
        }
        len = utf8_encode(utf16_unit, output, 3);
        output += len;
        output_len += len;
    }

    utf16_chunk = lname_entry->NAME11_12;
    for (i=0; i<4; i+=2) {
        utf16_unit = utf16_chunk[0] | (utf16_chunk[1] << 8);
        utf16_chunk += 2;
        if (utf16_unit == 0 || utf16_unit == 0xFFFF) {
            return output_len;
        }
        len = utf8_encode(utf16_unit, output, 3);
        output += len;
        output_len += len;
    }

    return output_len;
}
#else

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_extract
* Returned Value   :
*
* Comments  : Takes a LFN directory entry, extracts the 13 UTF16 code units
*             from the entry and stores it stripping the upper byte.
*             This conversion is compatible with ASCII characters
*             but looses information if the filename contains characters above 0xFF.
*END*---------------------------------------------------------------------*/
int MFS_lfn_extract
    (
    MFS_LNAME_ENTRY_PTR     lname_entry,    /* [IN] pointer to the entry to read UNICODE name from */
    char                    *filename       /* [OUT] buffer to store the 13 chars in  */
    )
{
    int i;

    char *output;
    int output_len;

    unsigned char *utf16_chunk;
    uint16_t utf16_unit;

    output = filename;
    output_len = 0;

    utf16_chunk = lname_entry->NAME0_4;
    for (i=0; i<10; i+=2) {
        utf16_unit = utf16_chunk[0] | (utf16_chunk[1] << 8);
        utf16_chunk += 2;
        if (utf16_unit == 0 || utf16_unit == 0xFFFF) {
            return output_len;
        }
        *output++ = utf16_unit & 0xFF;
        output_len++;
    }

    utf16_chunk = lname_entry->NAME5_10;
    for (i=0; i<12; i+=2) {
        utf16_unit = utf16_chunk[0] | (utf16_chunk[1] << 8);
        utf16_chunk += 2;
        if (utf16_unit == 0 || utf16_unit == 0xFFFF) {
            return output_len;
        }
        *output++ = utf16_unit & 0xFF;
        output_len++;
    }

    utf16_chunk = lname_entry->NAME11_12;
    for (i=0; i<4; i+=2) {
        utf16_unit = utf16_chunk[0] | (utf16_chunk[1] << 8);
        utf16_chunk += 2;
        if (utf16_unit == 0 || utf16_unit == 0xFFFF) {
            return output_len;
        }
        *output++ = utf16_unit & 0xFF;
        output_len++;
    }

    return output_len;
}
#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_increment_lfn
* Returned Value   :  void
*
* Comments  : Takes a compressed short filename (that represents a lfn) and
*             increments the number after the ~
*END*---------------------------------------------------------------------*/

void MFS_increment_lfn
    (
    char       *filename /* [IN]/[OUT] a short compressed filename (a valid one) */
    )
{
    int32_t     i;
    uint32_t     num;
    uint32_t     power;

    /* Find out where the ~ starts and get the number after the ~ */
    for ( i = 7, power = 1, num = 0; i >= 0; i-- )
    {
        if ( filename[i] == '~' )
        {
            break;
        }
        /* Translate the ASCII value to an int value */
        num += power * (uint32_t) (filename[i] - '0');
        power *= 10;
    }

    num++;

    if ( num / power >= 1 )
    {
        i--;
        filename[i] = '~';
        power *= 10;
    }

    /* Write the new number */
    for ( i++ ; i <= 7 ;i++ )
    {
        power /= 10;
        /* Translate the int value into an ASCII value */
        filename[i] = (unsigned char) (num / power) + '0';
        num %= power;
    }
}

#if !MFSCFG_READ_ONLY
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_remove_lfn_entries
* Returned Value   :  uint32_t error code
*
* Comments  : Removes the extra entries (corresponding to the dir_entry_ptr
*             containing the LFN). Can be used on a normal dir entry, but does
*             nothing in this case.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_remove_lfn_entries
    (
    MFS_DRIVE_STRUCT_PTR       drive_ptr,       /* [IN] drive on which to operate */
    uint32_t                    parent_cluster,  /* [IN] the cluster in which the entry lies */
    uint32_t                    dir_index,       /* [IN] the index of where in the cluster the entry lies */
    uint32_t                    prev_cluster     /* [IN] the cluster previous to the cluster in which the entry lies */
    )
{
    MFS_LNAME_ENTRY_PTR           lname_entry = NULL;
    bool                       last_entry_flag = FALSE;
    uint32_t                       current_index;
    _mfs_error                    error_code = MFS_NO_ERROR;
    uint32_t                       temp_prev_cluster;
    bool                       went_back = FALSE;
    bool                       start_deletion = FALSE;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    /* As long as we haven't found the EOF flag in the LFN name */
    while ( last_entry_flag == FALSE )
    {
        /* Check to see if the LFN spans to different sector */
        if ( dir_index == 0 )
        {
            /* Check to see if we are in root directory */
            if ( parent_cluster == 0 )
            {
                break;
            }
            else if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
            {
                if ( drive_ptr->BPB32.ROOT_CLUSTER == parent_cluster )
                {
                    break;
                }
            }
            if ( !went_back )
            {
                error_code = MFS_get_prev_cluster(drive_ptr, &parent_cluster, prev_cluster);
                went_back = TRUE;
            }
            else
            {
                if ( start_deletion )
                {
                    error_code = MFS_FAILED_TO_DELETE_LFN;
                }
                break;
            }
            dir_index = drive_ptr->ENTRIES_PER_SECTOR * drive_ptr->BPB.SECTORS_PER_CLUSTER ;
        }

        current_index = dir_index;

        if ( !error_code )
        {
            dir_index--;
            lname_entry = (MFS_LNAME_ENTRY_PTR) MFS_Find_directory_entry(drive_ptr,
                NULL, &parent_cluster, &dir_index, &temp_prev_cluster, MFS_ATTR_LFN, &error_code);
        }

        /* If a LFN, mark entry as deleted */
        if ( (lname_entry != NULL) && !error_code && (dir_index < current_index) )
        {
            if ( lname_entry->ID & MFS_LFN_END )
            {
                last_entry_flag = TRUE;
            }

            lname_entry->ID = MFS_DEL_FILE;
            drive_ptr->DIR_SECTOR_DIRTY = TRUE;

            start_deletion = TRUE;
        }
        else
        {
            break;
        }
    }

    return( error_code);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_name_to_entry
* Returned Value   :  uint32_t error code
*
* Comments  : Takes a string of up to 13 characters and writes them to a LFN
*             directory entry.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_lfn_name_to_entry
    (
    char                *filename,   /* [IN] a string up to 13 chars long, to be put in the entry */
    MFS_LNAME_ENTRY_PTR     entry_ptr   /* [OUT] the LNAME entry in which the string is places */
    )
{
    uint32_t     i;
    bool     end_of_name = FALSE;

    if ( entry_ptr == NULL )
    {
        return( MFS_CANNOT_CREATE_DIRECTORY );
    }


    /* The entry contains UNICODE chars... Which is why i = i +2 */
    /* Copy chars 0 to 4 */
    for ( i = 0 ; i <= 9 ; i = i + 2 )
    {
        if ( *filename )
        {
            mqx_htods((entry_ptr->NAME0_4 + i),(uint16_t) *filename);
            filename++;
        }
        else if ( !end_of_name )
        {
            mqx_htods((entry_ptr->NAME0_4 + i), 0);
            end_of_name = TRUE;
        }
        else
        {
            mqx_htods((entry_ptr->NAME0_4 + i), 0xFFFF);
        }
    }

    /* Copy chars 5 to 10 */
    for ( i = 0 ; i <= 11 ; i = i + 2 )
    {
        if ( *filename )
        {
            mqx_htods((entry_ptr->NAME5_10 + i), (uint16_t) *filename);
            filename++;
        }
        else if ( !end_of_name )
        {
            mqx_htods((entry_ptr->NAME5_10 + i), 0);
            end_of_name = TRUE;
        }
        else
        {
            mqx_htods((entry_ptr->NAME5_10 + i), 0xFFFF);
        }
    }

    /* Copy chars 11 to 12 */
    for ( i = 0 ; i <= 3 ; i = i + 2 )
    {
        if ( *filename )
        {
            mqx_htods((entry_ptr->NAME11_12 + i), (uint16_t) *filename);
            filename++;
        }
        else if ( !end_of_name )
        {
            mqx_htods((entry_ptr->NAME11_12 + i), 0);
            end_of_name = TRUE;
        }
        else
        {
            mqx_htods((entry_ptr->NAME11_12 + i), 0xFFFF);
        }
    }


    return( MFS_NO_ERROR );
}

#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_lfn_checksum
* Returned Value   :  unsigned char checksum
*
* Comments  : Takes a valid expanded filename of 11 characters (doesn't need
*             to be terminated by a NULL char) and calculates the LFN checksum
*             value for it.
*END*---------------------------------------------------------------------*/
unsigned char MFS_lfn_checksum
    (
    char       *filename_ptr /*[IN] pointer to filename on which to calculate checksum */
    )
{
    unsigned char       sum;
    uint32_t     i;

    for ( sum = 0, i = 0 ; i < 11 ; i++ )
    {
        sum = (((sum & 1) << 7) | (( sum & 0xfe) >> 1)) + *filename_ptr++;
    }

    return( sum );
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_prev_cluster
* Returned Value   :  uint32_t error_code
*
* Comments  : Finds the previous cluster in the cluster chain for any given
*             cluster.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_prev_cluster
    (
    MFS_DRIVE_STRUCT_PTR       drive_ptr,       /* [IN] the drive on which to operate */
    uint32_t                *cluster_ptr,     /* [IN]/[OUT] cluster to perform search with / previous cluster */
    uint32_t                    previous_cluster /* [IN] the previous cluster */
    )
{
    uint32_t        i;
    _mfs_error     error_code = MFS_NO_ERROR;
    uint32_t        next_cluster;

    /* Check to see if we are at the very first cluster */
    if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
    {
        if ( *cluster_ptr == drive_ptr->BPB32.ROOT_CLUSTER )
        {
            return MFS_INVALID_CLUSTER_NUMBER;
        }
    }
    else if ( *cluster_ptr == 0 )
    {
        return MFS_INVALID_CLUSTER_NUMBER;
    }

    // If we have been given a previous cluster
    if ( previous_cluster != CLUSTER_INVALID )
    {
        // Make sure it is before the current cluster
        MFS_get_cluster_from_fat(drive_ptr, previous_cluster, &next_cluster);
        if ( *cluster_ptr == next_cluster )
        {
            // It is, so use it
            *cluster_ptr = previous_cluster;
            return MFS_NO_ERROR;
        }
    }

    if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
    {
        return MFS_INVALID_CLUSTER_NUMBER;
    }

    /* Search the rest of the fat for the wanted cluster */
    for ( i = CLUSTER_MIN_GOOD ; i <= drive_ptr->LAST_CLUSTER &&
        !error_code; i++ )
    {
        MFS_get_cluster_from_fat(drive_ptr, previous_cluster, &next_cluster);
        if ( *cluster_ptr == next_cluster )
        {
            *cluster_ptr = i;
            break;
        }
    }

    if ( i > drive_ptr->LAST_CLUSTER )
    {
        error_code = MFS_INVALID_CLUSTER_NUMBER;
    }

    return( error_code );
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_lfn
* Returned Value   :  uint32_t error code
*
* Comments  : Will take an a pathname that points to a normal 8.3 entry.
*             If this entry has a LFN, the LFN will be placed in the lfn buffer.
*             Assumes the drive is locked.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_lfn
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    char                *filepath,   /*[IN] the filepath to the 8.3 filename */
    char                *lfn         /*[OUT] the long file name, if it exists */
    )
{
    MFS_DIR_ENTRY_PTR  entry_ptr;
    uint32_t              index;
    uint32_t              cluster;
    _mfs_error           error_code;
    uint32_t              prev_cluster;


    // If filepath is not an autogenerated name, then it will not have a
    // corresponding long file name.

    entry_ptr =  MFS_Find_entry_on_disk(drive_ptr, filepath, &error_code, &cluster, &index, &prev_cluster);

    if ( error_code==MFS_NO_ERROR )
    {
        error_code = MFS_get_lfn_of_entry(drive_ptr,entry_ptr,cluster,index,prev_cluster,lfn);
    }
    return error_code;

}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_lfn
* Returned Value   :  uint32_t error code
*
* Comments  : Will take an a pathname that points to a normal 8.3 entry.
*             If this entry has a LFN, the LFN will be placed in the lfn buffer.
*             Assumes the drive is locked.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_lfn_of_entry
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    MFS_DIR_ENTRY_PTR       entry_ptr,
    uint32_t                 cluster,
    uint32_t                 index,
    uint32_t                 prev_cluster,
    char                *lfn         /*[OUT] the long file name, if it exists */
    )
{
    _mfs_error           error_code=MFS_NO_ERROR;
    uint32_t              sum;
    uint32_t              entry_count;
    bool             went_back = FALSE;
    bool              extracting = FALSE;
    MFS_LNAME_ENTRY_PTR  lfn_entry_ptr;


    if ( entry_ptr == NULL )
    {
        return(MFS_INVALID_PARAMETER);
    }
    else
    {
        /* Calculate the checksum for the file */
        sum = MFS_lfn_checksum(((MFS_DIR_ENTRY_PTR) entry_ptr)->NAME);
    }

    entry_count = 1;

    while ( TRUE )
    {
        /* Check to see if the LFN spans to different sector */
        if ( index == 0 )
        {
            if ( went_back )
            {
                if ( extracting )
                {
                    error_code = MFS_BAD_LFN_ENTRY;
                }
                break;
            }
            else
            {
                went_back=TRUE;
                error_code = MFS_get_prev_cluster(drive_ptr, &cluster, prev_cluster);
                index = drive_ptr->ENTRIES_PER_SECTOR *
                    drive_ptr->BPB.SECTORS_PER_CLUSTER ;
            }
        }

        if ( !error_code )
        {
            index--;
            entry_ptr = MFS_Find_directory_entry(drive_ptr,
                NULL, &cluster, &index, &prev_cluster, MFS_ATTR_ANY, &error_code);
        }

        if ( error_code )
        {
            *lfn = '\0';
            return( error_code );
        }

        if ( !entry_ptr )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        lfn_entry_ptr = (MFS_LNAME_ENTRY_PTR) entry_ptr;

        if ( lfn_entry_ptr->ATTR != MFS_ATTR_LFN )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        if ( lfn_entry_ptr->ALIAS_CHECKSUM != sum )
        {
            *lfn = '\0';
            break;
        }
        else if ( !(lfn_entry_ptr->ID & entry_count++) )
        {
            *lfn = '\0';
            break;
        }
        else
        {
            int chunk_len;
            chunk_len = MFS_lfn_extract(lfn_entry_ptr, lfn);
            lfn += chunk_len;
            *lfn = '\0';
            extracting = TRUE;
        }

        /* Check if this is the last entry */
        if ( lfn_entry_ptr->ID & MFS_LFN_END )
        {
            break;
        }
    }

    return( MFS_NO_ERROR );
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_lfn_dir_cluster
* Returned Value   :  uint32_t error code
*
* Comments  : Will take an a current directory cluster entry.
*             If this entry has a LFN, the LFN will be placed in the lfn buffer.
*             Assumes the drive is locked.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_lfn_dir_cluster
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,          /*[IN] the drive on which to operate */
    void                   *search_next_ptr,    /*[IN] address of search data block indicating the current criteria and the results of
                                                ** the last search results of this search are placed in this data block */
    char                *filepath,           /*[OUT] the long file name, if it exists */
    char                *lfn                 /*[IN] the filepath to the 8.3 filename */
    )
{
    MFS_INTERNAL_SEARCH_PTR     internal_search_ptr;
    MFS_LNAME_ENTRY_PTR         entry_ptr;
    uint32_t                     index;
    uint32_t                     cluster;
    _mfs_error                  error_code;
    uint32_t                     sum;
    uint32_t                     entry_count;
    MFS_SEARCH_DATA_PTR         transfer_ptr;
    uint32_t                     temp_prev_cluster;
    bool                     went_back = FALSE;
    bool                     extracting = FALSE;

    transfer_ptr = (MFS_SEARCH_DATA_PTR) search_next_ptr;

    if ( transfer_ptr == NULL )
    {
        return(MFS_INVALID_PARAMETER);
    }

    internal_search_ptr = &transfer_ptr->INTERNAL_SEARCH_DATA;
    entry_ptr = (MFS_LNAME_ENTRY_PTR)MFS_Find_directory_entry(drive_ptr,NULL, &internal_search_ptr->CURRENT_CLUSTER,
        &internal_search_ptr->DIR_ENTRY_INDEX, &internal_search_ptr->PREV_CLUSTER, MFS_ATTR_ANY,&error_code);
    cluster    = internal_search_ptr->CURRENT_CLUSTER;
    index      = internal_search_ptr->DIR_ENTRY_INDEX;
    /* Calculate the checksum for the file */
    sum = MFS_lfn_checksum(((MFS_DIR_ENTRY_PTR) entry_ptr)->NAME);

    entry_count = 1;

    while ( TRUE )
    {
        /* Check to see if the LFN spans to different sector */
        if ( index == 0 )
        {
            if ( went_back )
            {
                if ( extracting )
                {
                    error_code = MFS_BAD_LFN_ENTRY;
                }
                break;
            }
            else
            {
                went_back=TRUE;
                error_code = MFS_get_prev_cluster(drive_ptr, &cluster, internal_search_ptr->PREV_CLUSTER);
                index = drive_ptr->ENTRIES_PER_SECTOR * drive_ptr->BPB.SECTORS_PER_CLUSTER ;
            }
        }

        if ( !error_code )
        {
            index--;
            entry_ptr = (MFS_LNAME_ENTRY_PTR) MFS_Find_directory_entry(drive_ptr,
                NULL, &cluster, &index, &temp_prev_cluster, MFS_ATTR_ANY, &error_code);
        }

        if ( error_code )
        {
            *lfn = '\0';
            return( error_code );
        }

        if ( !entry_ptr )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        if ( entry_ptr->ATTR != MFS_ATTR_LFN )
        {
            *lfn = '\0';
            return MFS_FILE_NOT_FOUND;
        }

        if ( entry_ptr->ALIAS_CHECKSUM != sum )
        {
            *lfn = '\0';
            break;
        }
        else if ( !(entry_ptr->ID & entry_count++) )
        {
            *lfn = '\0';
            break;
        }
        else
        {
            int chunk_len;
            chunk_len = MFS_lfn_extract(entry_ptr, lfn);
            lfn += chunk_len;
            *lfn = '\0';
            extracting = TRUE;
        }

        /* Check if this is the last entry */
        if ( entry_ptr->ID & MFS_LFN_END )
        {
            break;
        }
    }

    return( MFS_NO_ERROR );
}
