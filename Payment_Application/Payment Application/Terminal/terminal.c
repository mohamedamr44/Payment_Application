/*
 * terminal.c
 *
 *  Created on: Apr 3, 2024
 *      Author: Mohamed Amr
 */
#include "terminal.h"

/*getTransactionDate()*/
#define TEMP_TRANSACTION_DATE 12 //11+1=12 extra space is set to test if its > 11
#define TRANSACTION_DATE_SIZE 10

/*for The transaction date Stored in the array as [TD1 TD2 / TM1 TM2 / TY1 TY2 TY3 TY4 ]*/
#define TD1 0
#define TD2 1
#define SLASH_1 2
#define TM1 3
#define TM2 4
#define SLASH_2 5
#define TY1 6
#define TY2 7
#define TY3 8
#define TY4 9

//-----------------------------------------------------------------------------------------------
/*Function Description: the function checks the length of the date if it is =10
 * and checks the format DD//MM/YYYY allowing only 2020s years*/
EN_terminalError_t getTransactionDate(ST_terminalData_t *termData)
{

	uint8_t temp[TEMP_TRANSACTION_DATE];

	printf("Enter the transaction date in the fomrat DD/MM/YYYY:");
	fflush(stdin);//clear input buffer before taking input
	fgets(temp,sizeof(temp),stdin);

	/*replace the '\n' character by the null terminator
	if added by the fgets() function*/
	str_newline_remover(temp);

	if(str_len(temp)!=TRANSACTION_DATE_SIZE)
	{

		return WRONG_DATE;

	}


		/*Checking the format of the Date*/

		/*Checking the char "/" is in the right place*/

	 	if(temp[SLASH_1]!= '/' || temp[SLASH_2]!='/')
			{


				return WRONG_DATE;

			}


		/*Checking the month*/

		/*Checking if the first digit of the month  is zero
		 *  then the second digit of the month must be from 1 to 9 to give a valid month*/
		if(temp[TM1]=='0')
		{

			if(temp[TM2] <= '0' || temp[TM2] > '9')
			{

				return WRONG_DATE;

			}

		}

		/*if the first digit of the month  is 1 then the second digit of the month
		 *  must be 0 or or 1 or 2 only to give a valid month */
		else if (temp[TM1]=='1')
		{

			if(temp[TM2] < '0' || temp[TM2] > '2')
			{


				return WRONG_DATE;

			}

		}
		else
		{

			return WRONG_DATE;
		}


		/*checking the day*/

		/*checking if day given is 00*/
		if(temp[TD1]=='0' && temp[TD2]=='0')
		{

			return WRONG_DATE;

		}

		/*checking if this month is feburary then the days must be <= 29*/
		if(temp[TM1]== '0' && temp [TM2]=='2')
		{
			if(temp[TD1]<'0' || temp[TD1]>'2')
			{

				return WRONG_DATE;
			}

		}

		/*checking if it is any month other than feburary then days can be <=31*/
		else
		{
			if(temp[TD1]< '0' || temp[TD1]>'3')
			{

				return WRONG_DATE;
			}
			else if(temp[TD1] == '3' && temp[TD2]>'1')
			{


				return WRONG_DATE;
			}

		}

		/*checking the year*/

		if(temp[TY1] != '2' || temp[TY2] != '0' || temp[TY3]!= '2' ) //checking if the year is in 2020s
		{

			return WRONG_DATE;

		}
		else if (temp[TY4]>'9')
		{

			return WRONG_DATE;
		}



	/*By reaching this line it means date is validated*/

		/*copying date from temp array to the main array*/
		str_cpy(termData->transactionDate,temp);

		return TERMINAL_OK;


}



//-----------------------------------------------------------------------------------------------

/*Function Description: Comparing the year and month
 * to check if the Expiration date is before Transaction date*/
EN_terminalError_t isCardExpired(ST_cardData_t *cardData, ST_terminalData_t *termData)
{

	/*comparing the years together*/
	//if the exp year < transaction year then date is expired
	if(cardData->cardExpirationDate[Y2] < termData->transactionDate[TY4])
	{
		return EXPIRED_CARD;
	}
	//if the both years are equal then check the months
	else if(cardData->cardExpirationDate[Y2]== termData->transactionDate[TY4])
	{	/*if  first digit of exp month < trans month then expired*/
		if(cardData->cardExpirationDate[M1] < termData->transactionDate[TM1])
		{
			return EXPIRED_CARD;

		}
		/*if both digits are equal then check second digit of the month*/
		else if(cardData->cardExpirationDate[M1] == termData->transactionDate[TM1])
		{
			if(cardData->cardExpirationDate[M2] <= termData->transactionDate[TM2])
			{
				return EXPIRED_CARD;

			}
		}
	}


	return TERMINAL_OK;


}




//-----------------------------------------------------------------------------------------------


/*Function Description: The function takes input transaction amount from the user and stores in
 * transamount of the terminal then checks if it <= 0*/
EN_terminalError_t getTransactionAmount(ST_terminalData_t *termData)
{

	printf("Enter Transaction amount:");
	scanf("%f",&termData->transAmount);

	if(termData->transAmount <= 0)
	{
		return INVALID_AMOUNT;
	}
	else
	{
		return TERMINAL_OK;
	}



}


//----------------------------------------------------------------------------------------------

/*Function Description: Compares the transaction amount with
 * the maximum amount allowed at the terminal*/
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t *termData)
{
		if(termData->transAmount > termData->maxTransAmount)
		{
			return EXCEED_MAX_AMOUNT;
		}
		else
		{
			return TERMINAL_OK;
		}

}

//-----------------------------------------------------------------------------------------------


/*This function takes maxAmount and store it into the terminal*/
EN_terminalError_t setMaxAmount(ST_terminalData_t *termData, float maxAmount)
{

	if(maxAmount <= 0)
	{
		return INVALID_MAX_AMOUNT ;
	}
	else
	{
		termData->maxTransAmount = maxAmount;
		return TERMINAL_OK;

	}


}

//----------------------------------------------------------------------------------------------
/*Function Description: This function uses Luhn algorithm to check the validity of the PAN number
 * The algorithm depends on starting from the second right most number and doubling every second digit
 *  then adding all the numbers together, if the sum  mod 10 == 0 then the number is valid  */
EN_terminalError_t isValidCardPAN(ST_cardData_t *cardData)
{
	int i;//counter for looping
	uint8_t size=str_len(cardData->primaryAccountNumber);//calculating the length of the PAN
	uint8_t doubled;

	/*initalizing the checksum with the  right most number as the looping starts from the right second most*/
	uint8_t checksum=cardData->primaryAccountNumber[size-1]-CH_TO_INT;

		/*Note:48 constant (CH_TO_INT) os subtracted from each character of the array
		 *  to convert them to integer numbers*/

	for(i=size-2;i>=0;i-=2)
	{
		/*number the are going to be doubled*/
		doubled=((cardData->primaryAccountNumber[i]-CH_TO_INT)*2);

		if(doubled>=10)
		{
			doubled=doubled-9;
		}

		if(i-1<0)
		{
			checksum+=doubled;
			break;
		}					/*number that are not doubled*/
		checksum+=doubled+(cardData->primaryAccountNumber[i-1]-CH_TO_INT);

	}


	if(checksum % 10==0)
		{
			return TERMINAL_OK;
		}
		else
		{
			return INVALID_CARD;
		}




}





//---------------------------------------------Testing functions----------------------------------

/*Testing the getTransactionDate function*/
void getTransactionDateTest(void)
{
	ST_terminalData_t test;
	ST_terminalData_t *ptr;

	ptr=&test;

	EN_terminalError_t res;

	res=getTransactionDate(ptr);

	if(res==WRONG_DATE)
	{

		printf("Wrong date");
	}
	else if(res==TERMINAL_OK)
	{
		printf("Terminal okay transaction date is %s",ptr->transactionDate);
	}



}
//------------------------------------------------------------------------------------------------
/*Testing isCardExpired function */
void isCardExpriedTest(void)
{
	EN_terminalError_t res;


	ST_cardData_t Exp_date; //struct variable of type cardData
	ST_cardData_t *ptr_exp; //pointer to that struct

	ST_terminalData_t Tran_date;  //struct variable of type ST_terminalData_t
	ST_terminalData_t *ptr_tran;  //pointer to that struct


	ptr_exp = &Exp_date;
	ptr_tran = &Tran_date;

	getCardExpiryDate(ptr_exp);
	getTransactionDate(ptr_tran);

	res=isCardExpired(ptr_exp,ptr_tran);

	if(res==TERMINAL_OK)
	{
		printf("Terminal okay");
	}
	else if(res==EXPIRED_CARD)
	{
		printf("Expired card");
	}

}
//--------------------------------------------------------------------------------------------------

/*Testing the getTransactionAmount function  */
void getTransactionAmountTest(void)
{
	EN_terminalError_t res;


	ST_terminalData_t Tran_amount;  //struct variable of type ST_terminalData_t
	ST_terminalData_t *ptr_tran;  //pointer to that struct

	ptr_tran=&Tran_amount;

	res=getTransactionAmount(ptr_tran);

	if(res==TERMINAL_OK)
	{
		printf("Terminal okay, Transaction amount is %.2f",ptr_tran->transAmount);
	}
	else if (res== INVALID_AMOUNT )
	{

		printf("Invalid Amount");
	}


}
//-------------------------------------------------------------------------------------------------
/*Testing setMaxAmount function */

void setMaxAmountTest(void)
{
	EN_terminalError_t res;


	ST_terminalData_t set_max;  //struct variable of type ST_terminalData_t
	ST_terminalData_t *ptr;  //pointer to that struct

	ptr=&set_max;

	res=setMaxAmount(ptr,5000.500);

	if(res==TERMINAL_OK)
	{
		printf("Terminal okay, Max amount is %.2f",ptr->maxTransAmount);
	}
	else if (INVALID_MAX_AMOUNT)
	{
		printf("INVALID_MAX_AMOUNT");
	}


}

//-----------------------------------------------------------------------------------------------
/*Testing isBelowMaxAmount */

void isBelowMaxAmountTest(void)
{
	EN_terminalError_t res;


	ST_terminalData_t below_max;  //struct variable of type ST_terminalData_t
	ST_terminalData_t *ptr;  //pointer to that struct

	ptr=&below_max;


	setMaxAmount(ptr,5000.500); //setting max amount at the terminal to 5000.500

	getTransactionAmount(ptr); //taking input transaction amount from user to compare with max amount

	res=isBelowMaxAmount(ptr); //testing the below max amount to compare transaction amount with max amount

	if(res==TERMINAL_OK)
	{
		printf("Terminal okay, trans amount is below max amount");
	}
	else if(res== EXCEED_MAX_AMOUNT)
	{
		printf("EXCEED_MAX_AMOUNT");
	}


}
//------------------------------------------------------------------------------------------
/*Testing isValidCardPANTest*/
void isValidCardPANTest(void)
{
	ST_cardData_t test;
	EN_terminalError_t error_detect;

	str_cpy(test.primaryAccountNumber,(uint8_t*)"4681383168647409262");

	error_detect=isValidCardPAN(&test);

	if(error_detect== TERMINAL_OK)
	{
		printf("Terminal okay\n");
	}
	else
	{
		printf("INVALID CARD\n");
	}





}
