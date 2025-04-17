/*
 * lib.c
 *
 *  Created on: Apr 8, 2024
 *      Author: User-H
 */
#include "my_lib.h"

void str_cpy(uint8_t array1[],uint8_t array2[])
{
	int i=0;

	while(array2[i]!='\0')
	{
		array1[i]=array2[i];

		i++;
	}

	array1[i]='\0';



}

int str_len(uint8_t array[])
{
	int i=0;


	while(array[i]!='\0')
	{
		i++;
	}

	return i;

}



void str_newline_remover(uint8_t array[])
{
	int i=0;

	while(array[i]!='\0')
	{
		if(array[i]=='\n'&& array[i+1]=='\0')
		{
			array[i]='\0';
		}
		i++;
	}

}



