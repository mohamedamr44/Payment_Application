/*
 * lib.h
 *
 *  Created on: Apr 8, 2024
 *      Author: User-H
 */

#ifndef LIBRARY_MY_LIB_H_
#define LIBRARY_MY_LIB_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


/*function to copy array 2 to array 1*/
void str_cpy(uint8_t array1[],uint8_t array2[]);


/*function to return the length of the array*/
int str_len(uint8_t array[]);


/*function to remove the new line character added by fgets function*/
void str_newline_remover(uint8_t array[]);


/*function to fill accounts database with valid data*/

#endif /* LIBRARY_MY_LIB_H_ */
