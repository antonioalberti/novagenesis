/*
	NovaGenesis

	Name:		EPGS wrapper to run an embedded/simulated version on Linux
	Object:		epgs_wrapper
	File:		epgs_wrapper.c
	Author:		Vâner José Magalhães
	Date:		05/2021
	Version:	0.1

  	Copyright (C) 2021  Vâner José Magalhães

    This work is available under the GNU General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    This header file contains functions that are used by the EPGS.
 	All of them must be ported to the platform where the EPGS will be used.
 	They are all based on standard C functions.
*/

#ifndef EPGS_WRAPPER_H_
#define EPGS_WRAPPER_H_

#include "epgs_defines.h"

#define NULL ((void *)0)

//stdlib.h

/**
 * This function returns a pseudo-random number in the range of 0 to RAND_MAX.
 * RAND_MAX is a constant whose default value may vary between implementations but it is granted to be at least 32767.
 *
 * @return This function returns an integer value between 0 and RAND_MAX.
 */
int ng_rand (void);

/**
 * This function converts the string argument str to an integer (type int).
 *
 * @param str -- This is the string representation of an integral number.
 *
 * @return This function returns the converted integral number as an int value. If no valid conversion could be performed, it returns zero.
 */
int ng_atoi (const char *str);

/**
 * This function converts the initial part
 * of the string in str to an unsigned long int value according to the given base,
 * which must be between 2 and 36 inclusive, or be the special value 0.
 *
 * @param str -- This is the string containing the representation of an unsigned integral number.
 * @param endptr -- This is the reference to an object of type char*, whose value is set by the function to the next character in str after the numerical value.
 * @param base -- This is the base, which must be between 2 and 36 inclusive, or be the special value 0.
 *
 * @return This function returns the converted integral number as a long int value. If no valid conversion could be performed, a zero value is returned.
 */
unsigned long ng_strtoul (const char *str, char **endptr, int base);

/**
 * This function allocates the requested memory and returns a pointer to it.
 * The difference in malloc and calloc is that malloc does not set the memory to zero where as calloc sets allocated memory to zero.
 *
 * @param nitems -- This is the number of elements to be allocated.
 * @param size -- This is the size of elements.
 *
 * @return This function returns a pointer to the allocated memory, or NULL if the request fails.
 */
void *ng_calloc (long unsigned int nitems, long unsigned int size);

/**
 * This function allocates the requested memory and returns a pointer to it.
 *
 * @param size -- This is the size of the memory block, in bytes.
 *
 * @return This function returns a pointer to the allocated memory, or NULL if the request fails.
 */
void *ng_malloc (long unsigned int size);

/**
 * This function attempts to resize the memory block pointed to by ptr that was previously allocated with a call to malloc or calloc.
 *
 * @param ptr -- This is the pointer to a memory block previously allocated with malloc, calloc or realloc to be reallocated.
 * 					If this is NULL, a new block is allocated and a pointer to it is returned by the function.
 * @param size -- This is the new size for the memory block, in bytes.
 * 					If it is 0 and ptr points to an existing block of memory, the memory block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return This function returns a pointer to the newly allocated memory, or NULL if the request fails.
 */
void *ng_realloc (void *ptr, long unsigned int size);

/**
 * This function deallocates the memory previously allocated by a call to calloc, malloc, or realloc.
 *
 * @param ptr -- This is the pointer to a memory block previously allocated with malloc, calloc or realloc to be deallocated.
 * 					If a null pointer is passed as argument, no action occurs.
 */
void ng_free (void *ptr);


//string.h
/**
 * This function copies n characters from memory area ptrSrc to memory area ptrDst.
 *
 * @param ptrDst -- This is pointer to the destination array where the content is to be copied, type-casted to a pointer of type void*.
 * @param ptrSrc -- This is pointer to the source of data to be copied, type-casted to a pointer of type void*.
 * @param size -- This is the number of bytes to be copied.
 *
 * @return This function returns a pointer to destination, which is str1.
 */
void *ng_memcpy (void *ptrDst, const void *ptrSrc, long unsigned int size);

/**
 * This function compares the string pointed to, by ptrA to the string pointed to by ptrB.
 *
 * @param ptrA -- This is the first string to be compared.
 * @param ptrB -- This is the second string to be compared.
 *
 * @return This function return values that are as follows:
 * 			if Return value < 0 then it indicates ptrA is less than ptrB.
 * 			if Return value > 0 then it indicates ptrB is less than ptrA.
 * 			if Return value = 0 then it indicates ptrA is equal to ptrB.
 */
int ng_strcmp (const char *ptrA, const char *ptrB);

/**
 * This function copies the string pointed to, by ptrSrc to ptrDst.
 *
 * @param ptrDst -- This is the pointer to the destination array where the content is to be copied.
 * @param ptrSrc -- This is the string to be copied.
 *
 * @return This returns a pointer to the destination string dest.
 */
char *ng_strcpy (char *ptrDst, const char *ptrSrc);

/**
 * This function copies up to n characters from the string pointed to, by ptrSrc to ptrDst.
 * In a case where the length of ptrSrc is less than that of n, the remainder of ptrDst will be padded with null bytes.
 *
 * @param ptrDst -- This is the pointer to the destination array where the content is to be copied.
 * @param ptrSrc -- This is the string to be copied.
 * @param n -- The number of characters to be copied from source.
 *
 * @return This function returns the final copy of the copied string.
 */
char *ng_strncpy (char *ptrDst, const char *ptrSrc, long unsigned int n);

/**
 * This function computes the length of the string str up to, but not including the terminating null character.
 *
 * @param str -- This is the string whose length is to be found.
 *
 * @return This function returns the length of string.
 */
long unsigned int ng_strlen (const char *str);

/**
 * This function appends the string pointed to by ptrSrc to the end of the string pointed to by ptrDst.
 *
 * @param ptrDst -- This is pointer to the destination array, which should contain a C string, and should be large enough to contain the concatenated resulting string.
 * @param ptrSrc -- This is the string to be appended. This should not overlap the destination.
 *
 * @return This function returns a pointer to the resulting string ptrDst.
 */
char *ng_strcat (char *ptrDst, const char *ptrSrc);

/**
 * This function calculates the length of the initial segment of str1 which consists entirely of characters in str2.
 *
 * @param str1 -- This is the main C string to be scanned.
 * @param str2 -- This is the string containing the list of characters to match in str1.
 *
 * @return This function returns the number of characters in the initial segment of str1 which consist only of characters from str2.
 */
long unsigned int ng_strspn (const char *str1, const char *str2);

/**
 * This function calculates the length of the initial segment of str1, which consists entirely of characters not in str2.
 *
 * @param str1 -- This is the main C string to be scanned.
 * @param str2 -- This is the string containing a list of characters to match in str1.
 *
 * @return This function returns the number of characters in the initial segment of string str1 which are not in the string str2.
 */
long unsigned int ng_strcspn (const char *str1, const char *str2);

//stdio.h

/**
 * This function sends formatted output to stdout.
 *
 * @param format - This is the string that contains the text to be written to stdout.
 * 				It can optionally contain embedded format tags that are replaced by the values specified in subsequent additional arguments and formatted as requested.
 * 				Format tags prototype is %[flags][width][.precision][length]specifier
 * @param additional arguments - Depending on the format string, the function may expect a sequence of additional arguments,
 * 				each containing one value to be inserted instead of each %-tag specified in the format parameter (if any).
 *	 			There should be the same number of these arguments as the number of %-tags that expect a value.
 *
 * @return If successful, the total number of characters written is returned. On failure, a negative number is returned.
 */
int ng_printf (const char *format, ...);

/**
 * This function sends formatted output to a string pointed to, by str.
 *
 * @param str - This is the pointer to an array of char elements where the resulting C string is stored.
 * @param format - This is the String that contains the text to be written to buffer.
 * 				It can optionally contain embedded format tags that are replaced by the values specified in subsequent additional arguments and formatted as requested.
 * 				Format tags prototype: %[flags][width][.precision][length]specifier
 * @param additional arguments - Depending on the format string, the function may expect a sequence of additional arguments,
 * 				each containing one value to be inserted instead of each %-tag specified in the format parameter (if any).
 *	 			There should be the same number of these arguments as the number of %-tags that expect a value.
 *
 * @return If successful, the total number of characters written is returned excluding the null-character appended at the end of the string,
 * 			otherwise a negative number is returned in case of failure.
 */
int ng_sprintf (char *str, const char *format, ...);

/**
 * This function returns the number of milliseconds since the system start up.
 *
 * @return The number of milliseconds.
 */
double ng_GetTime ();

/**
 * This function sends Data through Ethernet stack.
 *
 * @param addr - Pointer to the beginning of the data to be sent.
 * @param size - This is the number of bytes to be sent.
 */
void ng_EthernetSendData (void *addr, long long size, char *_Interface);

/**
 * This function sends Data through Bluetooth stack.
 *
 * @param addr - Pointer to the beginning of the data to be sent.
 * @param size - This is the number of bytes to be sent.
 */
void ng_BLESendData (char *addr, long long size);
#endif /* EPGS_WRAPPER_H_ */
