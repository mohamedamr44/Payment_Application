/*
 * app.c
 *
 *  Created on: Apr 3, 2024
 *      Author: Mohamed Amr
 */

#include "app.h"

void appStart(void)
{
	EN_cardError_t card_state;//for detecting card state
	EN_terminalError_t terminal_state; // for detecting terminal state
	EN_transState_t transaction_state; //for detecting transaction state

	int array_file;//for switching between array Database or File database



//--------------------------------------Initializing Data Base-----------------------------------

	/*Initializing transaction sequence to 0*/
	TRANSACTION.transactionSequenceNumber=0;

	/*Setting max amount at terminal to 20000*/
	setMaxAmount(&ptr_TRANSACTION->terminalData,20000);
	/*Initializing accountsDB array*/
	 accountsDB_Start_Array();

	/*Initializing accounts Database File and TransactionsDB file*/
	 accountsDB_start_FILE();
	 transactionsDB_start_FILE();


	/*switching between array and file, not for user but for testing*/
	printf("Select 1-for arraysDB or 2-for files DB:");
	scanf("%d",&array_file);
	if(array_file != 1 && array_file != 2)
	{
		printf("Wrong choice\n");
		exit(4);
	}





	while(1)
	{
//--------------------------------------------CARD--------------------------------------------------------

		/*checking CardHolder name*/
		card_state=getCardHolderName(&ptr_TRANSACTION->cardHolderData);
		if(card_state==WRONG_NAME)
		{
			printf("WRONG_NAME\n");
			continue;
		}
		/*checking card Expiry date*/
		card_state=getCardExpiryDate(&ptr_TRANSACTION->cardHolderData);
		if(card_state==WRONG_EXP_DATE)
		{
			printf("WRONG_EXP_DATE\n");
			continue;
		}
		/*checking card PAN*/
		card_state=getCardPAN(&ptr_TRANSACTION->cardHolderData);
		if(card_state==WRONG_PAN)
		{
			printf("WRONG_PAN\n");
			continue;
		}


//-----------------------------------------TERMINAL-----------------------------------------------
		/*checking if the CARD PAN passed Luhn Check */
		terminal_state=isValidCardPAN(&ptr_TRANSACTION->cardHolderData);
		if(terminal_state==INVALID_CARD)
		{
			printf("INVALID_CARD - Failed in Luhn Check\n");
			continue;
		}

		/*checking transaction Date*/
		terminal_state=getTransactionDate(&ptr_TRANSACTION->terminalData);
		if(terminal_state==WRONG_DATE)
		{
			printf("WRONG_DATE\n");
			continue;
		}

		/*checking if the card is expired or not*/
		terminal_state=isCardExpired(&ptr_TRANSACTION->cardHolderData,&ptr_TRANSACTION->terminalData);
		if(terminal_state==EXPIRED_CARD)
		{
			printf("Decline Expired Card\n");
			continue;
		}

		terminal_state=getTransactionAmount(&ptr_TRANSACTION->terminalData);
		if(terminal_state==INVALID_AMOUNT)
		{
			printf("INVALID_AMOUNT\n");
			continue;
		}

		terminal_state=isBelowMaxAmount(&ptr_TRANSACTION->terminalData);
		if(terminal_state==EXCEED_MAX_AMOUNT)
		{
			printf("Declined Amount Exceeding limit\n");
			continue;
		}


//--------------------------------------------SERVER---------------------------------------------

		switch(array_file)
		{
			case 1: //arrayDB
				transaction_state=recieveTransactionData(ptr_TRANSACTION);
				break;
			case 2://fileDB
				transaction_state=recieveTransactionData_FILE(ptr_TRANSACTION);
				break;
		}


		if(transaction_state==FRAUD_CARD)
		{
			printf("Declined Invalid Account\n");
			continue;
		}
		else if (transaction_state==DECLINED_INSUFFECIENT_FUND)
		{
			printf("Declined Insufficient fund\n");
			continue;
		}
		else if (transaction_state==DECLINED_STOLEN_CARD)
		{
			printf("Declined Stolen card\n");
			continue;
		}
		else if (transaction_state== INTERNAL_SERVER_ERROR)
		{
			printf("INTERNAL_SERVER_ERROR\n");
			continue;
		}

		printf("APPROVED\n");





	}



}
