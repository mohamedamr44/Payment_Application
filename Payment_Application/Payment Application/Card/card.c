/*
 * card.c
 *
 *  Created on: Apr 3, 2024
 *      Author: Mohamed Amr
 */

#include "card.h"


/*Function description: The function mainly asks for a the card holder name,
 * stores it first in a temporary array which is bigger in size than the
 * main array (cardholdername array in the struct) to check if the given string passes the length
 *  and the alpahepets requirements, if the string is validated , the function then stores
 *  the string in the main array using pointer to struct recieved as an argument*/

EN_cardError_t getCardHolderName(ST_cardData_t *cardData)
{

	/*Temp array created to validate the length of the string first before storing it
	 * in the main array,  */
	uint8_t temp[TEMP__NAME_SIZE];
	uint8_t i=0;

	printf("Enter cardHolder's Name:");
	fflush(stdin); //clear input buffer before taking new input
	fgets(temp,sizeof(temp),stdin);


/* This line removes the '\n' character from the array
 * where the fgets() function counts  the enter button pressed also as a  character("\n)
 * given with string and stores it in the array too if the array size allows it */
	str_newline_remover(temp);

	if((str_len(temp)> MAX_STRING) ||(str_len(temp)< MIN_STRING) )//Checking the length of the name
	{
		return  WRONG_NAME;
	}


	/*Checking if the entered is string is English Alphapets
	 * by looping on the array and comparing each character with
	 * the ASCI code boundaries*/
	while(temp[i]!='\0')
		{
			/*Checking if the character lies in the range of UPPERCASE Alphabets*/
			if(temp[i] >= 'A' && temp[i] <= 'Z' )
			{
				i++;
				continue;

			}
			/*Checking if the character lies in the range of LOWERCASE Alphabets*/
			else if(temp[i] >= 'a' && temp[i] <= 'z' )
			{
				i++;
				continue;
			}
			/*Checking if the character is an empty space*/
			else if(temp[i]==EMPTY_SPACE)
			{
				i++;
				continue;
			}
			else
			{
				return  WRONG_NAME;
			}

			i++;

		}

	/*By reaching this line the string passes all tests
	 * so it is valid to be stored in the array*/

	//copying the name from the temporary array to the main array
	str_cpy(cardData->cardHolderName,temp);

	return CARD_OK;

}

//----------------------------------------------------------------------------------------------



/*Function Description: The function checks that lengths of expiration date is equal to 5
 * Then, it checks the date format that it refers to valid month and year in the format MM/YY*/
EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData)
{
	uint8_t temp[TEMP_DATE_SIZE];

	printf("Enter the Expiration  Date in the format MM/YY:");
	fflush(stdin); //clear input buffer before taking new input
	fgets(temp,sizeof(temp),stdin);


	/*replace the '\n' character by the null terminator
	if added by the fgets() function*/
	str_newline_remover(temp);

	if(str_len(temp)!= DATE_SIZE)
	{
		return WRONG_EXP_DATE;
	}


	/*Checking the format of the Date*/

	/*Checking if the first digit of the month is zero
	 *  then the second digit of the month must be from 1 to 9 to give a valid month*/
	if(temp[M1]=='0')
	{

		if(temp[M2] <= '0' || temp[M2] > '9')
		{
			return WRONG_EXP_DATE;

		}

	}
	/*if the first digit of the month is 1 then the second digit of the month must be 0 or or 1 or 2 only
	 *  to give a valid month */
	else if (temp[M1]=='1')
	{

		if(temp[M2] < '0' || temp[M2] > '2')
		{
			return WRONG_EXP_DATE;

		}

	}
	else
	{
		return WRONG_EXP_DATE;
	}

	/*the first digit of the year  must be 2 to represent a year in 2020s*/
	if(temp[SLASH] !='/' || temp[Y1] !='2')
	{
		return WRONG_EXP_DATE;

	}

	/*assuming maximux expiration date allowed is 2029*/
	else if(temp[Y2] < '0' || temp[Y2]> '9')
	{
		return WRONG_EXP_DATE;

	}



	/*By reaching this line the Date is validate Then copy the date from
	 * the temp array to the main array*/

	str_cpy(cardData->cardExpirationDate,temp);
	return CARD_OK;


}



//----------------------------------------------------------------------------------------------

/*Function Description: The function checks the lenth of the PAN number if it is [16,19]
 * and then checks if the input characters are valid numbers*/
EN_cardError_t getCardPAN(ST_cardData_t *cardData)
{
	uint8_t temp[TEMP_PAN_SIZE];
	uint8_t i;

	printf("Enter PAN number:");
	fflush(stdin); //clear input buffer before taking new input
	fgets(temp,sizeof(temp),stdin);

	/*replace the '\n' character by the null terminator
		if added by the fgets() function*/
	str_newline_remover(temp);


	/*checking if the length is less than 16 or greater than 19 */
	if(str_len(temp) > PAN_MAX || str_len(temp) < PAN_MIN )
	{
		return WRONG_PAN;
	}

	while(temp[i]!='\0')
	{
		/*checking it they are not numbers*/
		if(temp[i] <'0' || temp[i] > '9')
		{
			return WRONG_PAN;
		}

		i++;

	}

	/*By reaching this line then the PAN number is validated*/

	//copying the PAN from the temp array to the main array
	str_cpy(cardData->primaryAccountNumber,temp);
	return CARD_OK;

}



//----------------------------------------Testing fucntions-----------------------------------------------------

/*function created to test the getCardHolderName function */
/*Test case 1 input data: mohamed                     (<20)	           Expected Result:Wrong name
 *Test case 2 input data: mohamed amr mohamed amr  	  (validated)      Expected Result: Card ok,The name is %s
 *Test case 3 input data: 123aaf                      (numbers)        Expected Result: Wrong name
 *Test case 4 input data: mohamed amr mohamed sayed   (>24)            Expected Result: Wrong name
 *Test case 5 input data:mohamed amr mohameda         (=20)            Expected Result: Card ok,The name is %s
 *Test case 6 input data:mohamed amr mohamed amra    (=24)             Expected Result: Card ok,The name is %s */
void getCardHolderNameTest(void)
{
	uint8_t i;
	ST_cardData_t test; //creating struct variable to test the function


	EN_cardError_t Name_test; /*variable to recieve the return value of the function
								to be tested*/


	ST_cardData_t *ptr_test; //pointer to struct
	ptr_test=&test;

	printf("Tester Name: Mohamed Amr\n");
	printf("Function Name: getCardHolderName\n");

	//loop to the test the function 6 time with 6 different test cases
	for(i=0;i<TEST_CASES;i++)
	{

		printf("Test case %d\n",i+1);


		if(i==0)
		{	printf("Input data:mohamed\n");
			Name_test=getCardHolderName(ptr_test);
			printf("Expected Result: Wrong name\n");
		}
		else if(i==1)
		{
			printf("Input data:mohamed amr mohamed amr\n");
			Name_test=getCardHolderName(ptr_test);
			printf("Expected Result: Card ok The name is mohamed amr mohamed amr\n");
		}
		else if (i==2)
		{
			printf("Input data:123aaf\n");
			Name_test=getCardHolderName(ptr_test);
			printf("Expected Result: Wrong name\n");
		}
		else if (i==3)
		{
			printf("Input data:mohamed amr mohamed amr sayed\n");
			Name_test=getCardHolderName(ptr_test);
			printf("Expected Result: Wrong name\n");


		}

		else if (i==4)
		{

			printf("input data:mohamed amr mohameda\n");
			Name_test=getCardHolderName(ptr_test);
			printf("Expected Result: Card ok,The name is mohamed amr mohameda\n");

		}

		else if (i==5)
		{
			printf("input data:mohamed amr mohamed amra\n");
			Name_test=getCardHolderName(ptr_test);
			printf("Expected Result: Card ok,The name is mohamed amr mohamed amra\n");
		}


		/*Checking the return value of the function and printing its actual outpu*/
		if(Name_test == WRONG_NAME)
		{	printf("Actual Result:   Wrong name\n");

		}
		else if( Name_test ==CARD_OK)
		{
			printf("Actual Result:   Card ok The name is %s \n",ptr_test->cardHolderName);

		}

	}

}

//------------------------------------------------------------------------------------------------
/*function created to test the getCardExpiryDateTest function */
/*Test case 1 input: 22/22,					Expected Result: WRONG_EXP_DATE
 *Test case 2 input : 0125  				Expected Result: WRONG_EXP_DATE
 *Test case 3 input data: 11/25             Expected Result:  Card ok Exp Date is: 11/25
 *Test case 4 input data 0a/25  			Expected Result: WRONG_EXP_DATE
  Test case 5 input data: 06/30             Expected Result : Wrong EXP_DATE*
  Test case 6 input data : 06/2             Expected Result : Wrong EXP_DATE */
void getCardExpiryDateTest (void)
{
	uint8_t i;

	ST_cardData_t test; //creating struct variable to test the function

	EN_cardError_t Date_test; //variable to recieve the return value of the function to be tested

	ST_cardData_t *ptr_test; //pointer to struct
	ptr_test=&test;



	printf("Tester Name: Mohamed Amr\n");
	printf("Function Name: getCardExpiryDateTest\n");

	//loop to the test the function 6 time with 6 different test cases
	for(i=0;i<TEST_CASES;i++)
	{

		printf("Test case %d\n",i+1);


		if(i==0)
		{
			printf("Input data:22/22\n");
			Date_test=getCardExpiryDate(ptr_test);
			printf("Expected Result: WRONG_EXP_DATE\n");
		}
		else if(i==1)
		{
			printf("Input data:0125\n");
			Date_test=getCardExpiryDate(ptr_test);
			printf("Expected Result: WRONG_EXP_DATE\n");
		}
		else if (i==2)
		{
			printf("Input data:11/25\n");
			Date_test=getCardExpiryDate(ptr_test);
			printf("Expected Result:  Card ok Exp Date is: 11/25\n");
		}
		else if (i==3)
		{	printf("Input data:0a/25\n");
			fflush(stdin); //clear input buffer before taking new input
			Date_test=getCardExpiryDate(ptr_test);
			printf("Expected Result: WRONG_EXP_DATE\n");
		}
		else if (i==4)
		{
			printf("Input data:06/30\n");
			Date_test=getCardExpiryDate(ptr_test);
			printf("Expected Result: WRONG_EXP_DATE\n");
		}
		else if (i==5)
		{
			printf("Input data:06/2\n");
			Date_test=getCardExpiryDate(ptr_test);
			printf("Expected Result: WRONG_EXP_DATE\n");
		}


		/*Checking the return value of the function and printing its actual outpu*/
		if(Date_test == WRONG_EXP_DATE)
		{	printf("Actual Result:   WRONG_EXP_DATE\n");

		}
		else if( Date_test ==CARD_OK)
		{
			printf("Actual Result:    Card ok Exp Date is  %s \n",ptr_test->cardExpirationDate);

		}

	}


}

//-------------------------------------------------------------------------------------------------
/*Testing the getCardPAN function*/

/*Test case 1 input:1234567891111111111,      (=19 number)			      Expected Result:Card ok PAN is 1234567891111111111
 *Test case 2 input:1abcdefghijklmklij        (16 number but characters)  Expected Result: WRONG_PAN
 *Test case 3 input data: 123                 (<16)                		  Expected Result: WRONG_PAN
 *Test case 4 input data 12345678911111111111 (>19)					      Expected Result: WRONG_PAN
 *Test case 5 input data 1234567891111111     (=16 number )               Expected Result:Card ok PAN is 1234567891111111 */

void getCardPANTest (void)
{

	uint8_t i;

		ST_cardData_t test; //creating struct variable to test the function

		EN_cardError_t PAN_test; //variable to recieve the return value of the function to be tested

		ST_cardData_t *ptr_test; //pointer to struct
		ptr_test=&test;



		printf("Tester Name: Mohamed Amr\n");
		printf("Function Name: getCardPAN\n");

		//loop to the test the function 5 time with 5 different test cases
		for(i=0;i<PAN_TEST_CASES;i++)
		{

			printf("Test case %d\n",i+1);


			if(i==0)
			{	printf("Input data:1234567891111111111\n");
				PAN_test=getCardPAN(ptr_test);
				printf("Expected Result:Card ok PAN is 1234567891111111111\n");
			}
			else if(i==1)
			{
				printf("Input data:1abcdefghijklmklij\n");
				PAN_test=getCardPAN(ptr_test);
				printf("Expected Result:WRONG_PAN\n");
			}
			else if (i==2)
			{
				printf("Input data:123\n");
				PAN_test=getCardPAN(ptr_test);
				printf("Expected Result:WRONG_PAN\n");
			}
			else if (i==3)
			{	printf("Input data:12345678911111111111\n");
				PAN_test=getCardPAN(ptr_test);
				printf("Expected Result:WRONG_PAN\n");
			}
			else if (i==4)
			{
				printf("Input data:1234567891111111\n");
				PAN_test=getCardPAN(ptr_test);
				printf("Expected Result:Card ok PAN is 1234567891111111\n");
			}


			/*Checking the return value of the function and printing its actual outpu*/
			if(PAN_test == WRONG_PAN)
			{	printf("Actual Result:  WRONG_PAN\n");

			}
			else if( PAN_test ==CARD_OK)
			{
				printf("Actual Result:  Card ok PAN is %s \n",ptr_test->primaryAccountNumber);

			}

		}

}
