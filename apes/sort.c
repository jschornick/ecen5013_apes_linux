/*
 * File        : sort.c
 * Description : APES Homework 2, Problem 3.
 *
 * This is the implementation of the new sort system call.
 *
 * Author      : Jeff Schornick (jesc5667@colorado.edu)
 * Version     : Version and history information is available at the GitHub repository:
 *               https://github.com/jschornick/ecen5013_apes_linux
 *
 * NOTE: This file attempts to follow the Linux kernel coding style.
 */

#include <linux/syscalls.h>   // SYSCALL_DEFINEn macros

//SYSCALL_DEFINE1(sort, unsigned int, test)
SYSCALL_DEFINE3(sort, const int32_t __user *, p_unsorted, int32_t __user *, p_sorted, const size_t, buf_size)
{

        long retval;
        int32_t *p_buffer = NULL;

        printk( KERN_INFO "sys_sort() : Entering syscall" );

        printk( KERN_DEBUG "sys_sort() : Allocating kernel buffer of %zu bytes",  buf_size );
        p_buffer = kmalloc( buf_size, GFP_KERNEL ); // not necessarily atomic, may sleep

        if( !p_buffer ) {
            printk( KERN_ERR "sys_sort() : Unable to allocate kernel buffer!" );
            return -ENOMEM;
        }

        if( copy_from_user(p_buffer, p_unsorted, buf_size) != 0 ) {
            printk( KERN_ERR "sys_sort() : Invalid source pointer!" );
            retval = -EINVAL;
        } else {

            // sort here

            if( copy_to_user(p_sorted, p_buffer, buf_size) != 0 ) {
                printk( KERN_ERR "sys_sort() : Invalid destination pointer!" );
                retval = -EINVAL;
            } else {
                retval = 0;
            };
        }

        printk( KERN_INFO "sys_sort() : Freeing kernel buffer" );
        kfree( p_buffer );
        printk( KERN_INFO "sys_sort() : Exiting syscall" );

        return retval;
}
