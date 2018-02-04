/*
 * File        : sort.c
 * Description : APES Homework 2, Problem 3.
 *
 * This is the implementation of the new sort system call.
 *
 * Author      : Jeff Schornick (jesc5667@colorado.edu)
 * Version     : Version and history information is available at the GitHub repository:
 *               https://github.com/jschornick/ecen5013_apes_linux (hw2_p3 branch)
 *
 * NOTE: This file attempts to follow the Linux kernel coding style.
 */

#include <linux/syscalls.h>   // SYSCALL_DEFINEn macros
#include <linux/sort.h>

/**
 * int32_compare - Compare two 32-bit signed integers
 * @a: left side value to compare
 * @b: right side value to compare
 *
 * Since the kernel sort function is generic, arguments must be cast to their
 * expected type and dereferenced to perform a comparison.
 *
 * Returns : -1 if a > b
 *           +1 if a < b
 *            0 if a == b
 */
static int int32_compare(const void *a, const void *b);
static int int32_compare(const void *a, const void *b)
{
        if ( (*(const int32_t *) a) > (*(const int32_t *) b) ) {
                return -1;
        } else if ( (*(const int32_t *) a) < (*(const int32_t *) b )) {
                return 1;
        } else {
                return 0;
        }
}

/**
 * sys_sort - Sort a buffer of 32-bit signed integers
 *
 * @p_unsorted: A pointer to the unsorted data in user space
 * @p_sorted  : A pointer to where the sorted data should be stored in user space
 * @count     : The number of 32-bit elements to sort
 *
 * This system call takes two user space buffers, one for the unsorted source data
 * and one for the sorted results. The source data is copied into kernel space,
 * sorted using the kernel's generic sorting function, and then the results are
 * copied back into user space at the @p_sorted pointer.
 */

SYSCALL_DEFINE3(sort, const int32_t __user *, p_unsorted, int32_t __user *,
                                              p_sorted, const size_t, count)
{
        long retval;
        int32_t *p_buffer = NULL;
        size_t buf_size = sizeof(*p_buffer) * count;

        printk( KERN_INFO "sys_sort() : Entering syscall\n" );

        printk( KERN_DEBUG "sys_sort() : Allocating kernel buffer of %zu bytes\n",  buf_size );
        p_buffer = kmalloc( buf_size, GFP_KERNEL );

        /* Attempt to allocate enough memory to perform sort within kernel space */
        if( !p_buffer ) {
                printk( KERN_ERR "sys_sort() : Unable to allocate kernel buffer!\n" );
                retval = -ENOMEM;
                goto free_and_exit;
        }

        /* Copy all of the source data into the kernel buffer */
        if( copy_from_user(p_buffer, p_unsorted, buf_size) != 0 ) {
                printk( KERN_ERR "sys_sort() : Invalid source pointer!\n" );
                retval = -EINVAL;
                goto free_and_exit;
        }

        printk( KERN_DEBUG "sys_sort() : Beginning sort" );
        /* Use the kernel's built in sort function, with our custom int32 comparator */
        sort( p_buffer, count, sizeof(*p_buffer), int32_compare, NULL );
        printk( KERN_DEBUG "sys_sort() : Sorting complete" );

        /* Copy the now sorted data back into user space */
        if( copy_to_user(p_sorted, p_buffer, buf_size) != 0 ) {
                printk( KERN_ERR "sys_sort() : Invalid destination pointer!\n" );
                retval = -EINVAL;
        } else {
                retval = 0;
        };

        /* Cleanup the allocated kernel memory and return the status of the system call
         * Note to TAs: The use of goto in this style is recommended for the kernel. See
         *     http://koblents.com/Ches/Links/Month-Mar-2013/20-Using-Goto-in-Linux-Kernel-Code/
         *     ...for more information.
         */
        free_and_exit:

        printk( KERN_INFO "sys_sort() : Freeing kernel buffer\n" );
        kfree( p_buffer );
        printk( KERN_INFO "sys_sort() : Exiting syscall\n" );

        return retval;
}
