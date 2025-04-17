/*
 * server.c
 *
 *  Created on: Apr 3, 2024
 *      Author: Mohamed Amr
 */

#include "server.h"


/*Function Description: This function takes the global pointer of the TRANSACTION global variable
 * and validates all data stored in that struct*/
EN_transState_t recieveTransactionData(ST_transaction_t *transData)
{
	EN_serverError_t account_validation;
	EN_serverError_t flag;

	/*Checking if the account exits*/

	account_validation=isValidAccount(&transData->cardHolderData,account_reference);

	if(account_validation == ACCOUNT_NOT_FOUND )
	{

		transData->transState=FRAUD_CARD;
		saveTransaction(ptr_transactionsDB);
		return FRAUD_CARD;
	}


	/*Checking if the amount is available*/

	account_validation=isAmountAvailable(&transData->terminalData,account_reference);
	if(account_validation== LOW_BALANCE)
	{
		transData->transState=DECLINED_INSUFFECIENT_FUND;
		saveTransaction(ptr_transactionsDB);
		return DECLINED_INSUFFECIENT_FUND;
	}


	/*checking if the account is Running or Blocked*/
	account_validation=isBlockedAccount(account_reference);

	if(account_validation== BLOCKED_ACCOUNT)
	{
		transData->transState=DECLINED_STOLEN_CARD;
		saveTransaction(ptr_transactionsDB);
		return DECLINED_STOLEN_CARD;
	}

	/*update the balance */
	account_reference->balance = account_reference->balance - (transData->terminalData.transAmount);


	transData->transState=APPROVED;
	flag=saveTransaction(ptr_transactionsDB);

	if(flag== SAVING_FAILED)
	{
		transData->transState=INTERNAL_SERVER_ERROR;
		return INTERNAL_SERVER_ERROR ;
	}
	else
	{
		return APPROVED;
	}




}










//-----------------------------------------------------------------------------------------------------------
/*Function description: The function copmares the PAN number stored in the cardData with the
 * PAN number of each accounts
 * NOTE: The function works well only if there is a valid PAN number stored in the CardData
 * So, in The application layer, if the PAN number is not valid the program exits before calling this
 * function
 * if the function returns server_ok then the pointer passed to this function is pointing to the address
 * of the account that has the matching PAN*/
EN_serverError_t isValidAccount(ST_cardData_t *cardData, ST_accountsDB_t *accountRefrence)
{
	uint8_t i=0;
	uint8_t j;
	uint8_t flag;

	accountRefrence=accountsDB;//pointing to the accountsDB with each call to start comparing from first element

	/*looping on the accounts database to check the PAN of each account*/
	for(i=0; i<ACC_MAX; i++)
	{
		flag=0;
		/*comparing the PAN of the card with the PAN of each account*/
		j=0;
		while(cardData->primaryAccountNumber[j]!= '\0')
		{

			/*if any PAN number differs than the one on the server break the loop and check next account*/
			if(cardData->primaryAccountNumber[j] == (accountRefrence+i)->primaryAccountNumber[j])
			{

				j++;
				continue;
			}
			else
			{
				flag=1;
				break;


			}


		}
		/*flag = 0 means that the characters of the PAN number on the card
				 * and the server are the same */
		if(flag==0)
		{
			account_reference=accountRefrence+i;//saving the address of the valid account PAN in the global pointer
			return SERVER_OK;
		}

		j++;


	}

	/*by reaching this line it means that no match between CARD and any of PAN numbers*/
	accountRefrence=NULL;
	return ACCOUNT_NOT_FOUND;

}


//------------------------------------------------------------------------------------------------

/*Function Description: The function takes the pointer that directs to the matching PAN number
 * and returns the account state running or blocked*/
EN_serverError_t isBlockedAccount(ST_accountsDB_t *accountRefrence)
{
	EN_accountState_t res;

	res=accountRefrence->state; //checking the state of the account from the accountsDB

	if(res==RUNNING)
	{
		return SERVER_OK;
	}
	else
	{
		return BLOCKED_ACCOUNT;
	}




}


//----------------------------------------------------------------------------------------------
/*Function Description: The function takes reference of the validated PAN number
 * and checks it's balance comparing it with the transaction*/
EN_serverError_t isAmountAvailable(ST_terminalData_t *termData, ST_accountsDB_t *accountRefrence)
{

	if(termData->transAmount > accountRefrence->balance)
	{
		return LOW_BALANCE;
	}
	else
	{
		return SERVER_OK;
	}




}

//----------------------------------------------------------------------------------------------
/*Function description: The function takes a pointer to the transactionsDB and copy all data from the global transaction variable to the transactionsDB
 * depending on global transaction sequence used to increment the pointer passed to it to access the
 * right struct in the database and the sequence number is incremented each time a transaction is saved*/
EN_serverError_t saveTransaction(ST_transaction_t *transData)
{
	EN_serverError_t flag=SAVING_FAILED;

	*(transData+ptr_TRANSACTION->transactionSequenceNumber)=*ptr_TRANSACTION;

	ptr_TRANSACTION->transactionSequenceNumber++;//increment the sequence number



	flag=SERVER_OK;

	/*Lists all saved Transactions in the transactionsDB*/
	listSavedTransactions();

	return flag;

}

//-----------------------------------------------------------------------------------------------
/*Function Description: This function lists all transaction data in the transactionDB*/
void listSavedTransactions(void)
{
	uint8_t i;
	EN_transState_t Transaction_State;
	ptr_transactionsDB=transactionsDB;

	/*looping on the transactionsDB*/
	for(i=0; i<ptr_TRANSACTION->transactionSequenceNumber;i++)
	{

		Transaction_State=(ptr_transactionsDB+i)->transState;

		printf("\n##############################\n");


		printf("Transaction Sequence Number: %d\n",(ptr_transactionsDB+i)->transactionSequenceNumber);
		printf("Transaction Date: %s\n",(ptr_transactionsDB+i)->terminalData.transactionDate);
		printf("Transaction Amount: %.2f\n",(ptr_transactionsDB+i)->terminalData.transAmount);

		if(Transaction_State== APPROVED)
		{
			printf("Transaction State: APPROVED\n");
		}
		else if(Transaction_State== DECLINED_INSUFFECIENT_FUND)
		{
			printf("Transaction State: DECLINED_INSUFFECIENT_FUND\n");
		}
		else if(Transaction_State== DECLINED_STOLEN_CARD)
		{
			printf("Transaction State: DECLINED_STOLEN_CARD\n");
		}
		else if(Transaction_State== FRAUD_CARD)
		{
			printf("Transaction State: FRAUD_CARD\n");
		}
		else if(Transaction_State== INTERNAL_SERVER_ERROR)
		{
			printf("Transaction State: INTERNAL_SERVER_ERROR\n");
		}

		printf("Terminal max amount: %.2f\n",(ptr_transactionsDB+i)->terminalData.maxTransAmount);

		printf("CardHolder Name: %s\n",(ptr_transactionsDB+i)->cardHolderData.cardHolderName);

		printf("PAN: %s\n",(ptr_transactionsDB+i)->cardHolderData.primaryAccountNumber);

		printf("Card Expiration Date: %s\n",(ptr_transactionsDB+i)->cardHolderData.cardExpirationDate);

		printf("\n##############################\n");


	}






}


//---------------------------FUNCTIONS CREATED TO USE FILE INSTED OF ARRAY----------------
//----------------------------------------------------------------------------------------------
void filling_accountsDB_file(FILE * ptr_accountsDB_FILE,float balance,EN_accountState_t state,uint8_t PAN[])
{
	/*struct variable of type accounts*/
	ST_accountsDB_t accountsDB_FILE;


	/*filling the struct with data*/
	accountsDB_FILE.balance=balance;
	accountsDB_FILE.state=state;
	str_cpy(accountsDB_FILE.primaryAccountNumber,PAN);

	/*writing the data from the struct to the file*/
	fwrite(&accountsDB_FILE,sizeof(accountsDB_FILE),1,ptr_accountsDB_FILE);




}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
/*This function recieves data from the terminal and validate it by accessing the accounts database file,
 the function temporarly stores account information in a local struct until being validated, meanwhile,
the function also buffers these accounts in another temp file to be able to update the balance in case needed,
at the end the function renames the updated temp file to be the original database and removes the other file.*/
EN_transState_t recieveTransactionData_FILE(ST_transaction_t *transData)
{
	FILE* ptr_accountsDB_FILE; 	//used to open original file
	FILE* ptr_accountsDB_FILE_temp;	//used to copy data from original file to a temp file

	/*local struct used to temporarily store account data from the original file to process it and to buffer it to the temp file */
	ST_accountsDB_t accounts_buffer;
	ST_accountsDB_t *ptr_accounts_buffer;

	EN_serverError_t account_validation; //used to store the server error returned by the called functions

	uint8_t flag=0;
	EN_serverError_t flag_2; //used to detect saving state

	ptr_accounts_buffer=&accounts_buffer;


	/*opening the accounts database file*/
	if((ptr_accountsDB_FILE=fopen("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\accountsDB.txt","rb"))==NULL)
		{
			printf("couldn't open accounts database 2 \n");
			transData->transState=INTERNAL_SERVER_ERROR;
			saveTransaction_FILE(ptr_TRANSACTION);
			return INTERNAL_SERVER_ERROR ;

		}


	/*opening the accounts database  temp file*/
	if((ptr_accountsDB_FILE_temp=fopen("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\accountsDB_temp.txt","wb"))==NULL)
		{
			printf("couldn't open accounts database 3\n");
			transData->transState=INTERNAL_SERVER_ERROR;
			saveTransaction_FILE(ptr_TRANSACTION);
			return INTERNAL_SERVER_ERROR ;

		}

	while(fread(&accounts_buffer,sizeof(accounts_buffer),1,ptr_accountsDB_FILE))
	{
		/*buffer accounts to the temp file*/
		fwrite(&accounts_buffer,sizeof(accounts_buffer),1,ptr_accountsDB_FILE_temp);

		/*Checking if the account exits*/
		account_validation=isValidAccount_FILE(&transData->cardHolderData,ptr_accounts_buffer);

		if(account_validation == ACCOUNT_NOT_FOUND )
		{
			continue;
		}
		else
		{
			flag=1; //account found
		}


	/*Checking if the amount is available*/

	account_validation=isAmountAvailable(&transData->terminalData,ptr_accounts_buffer);
	if(account_validation== LOW_BALANCE)
	{
		transData->transState=DECLINED_INSUFFECIENT_FUND;
		saveTransaction_FILE(ptr_TRANSACTION);
		return DECLINED_INSUFFECIENT_FUND;
	}


	/*checking if the account is Running or Blocked*/
	account_validation=isBlockedAccount(ptr_accounts_buffer);

	if(account_validation== BLOCKED_ACCOUNT)
	{
		transData->transState=DECLINED_STOLEN_CARD;
		saveTransaction_FILE(ptr_TRANSACTION);
		return DECLINED_STOLEN_CARD;
	}

	/*update the balance */
	/*updating balance of the account_buffer then in buffer the updated balance to the temp file*/
	ptr_accounts_buffer->balance = ptr_accounts_buffer->balance - (transData->terminalData.transAmount);
	fwrite(&accounts_buffer,sizeof(accounts_buffer),1,ptr_accountsDB_FILE_temp);//buffer updated account to the tempfile



}

	fclose(ptr_accountsDB_FILE); //close original file
	fclose(ptr_accountsDB_FILE_temp); // close temp file

	/*replace the original file with the updated temp file*/
	remove("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\accountsDB.txt");
	rename("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\accountsDB_temp.txt","accountsDB.txt");

	if(flag==0)//account not found
	{
		transData->transState=FRAUD_CARD;
		saveTransaction_FILE(ptr_TRANSACTION);
		return FRAUD_CARD;
	}

	else//account found and validated
	{
		transData->transState=APPROVED;
		flag_2=saveTransaction_FILE(ptr_TRANSACTION);
		if(flag_2==SAVING_FAILED)
		{
			return INTERNAL_SERVER_ERROR;
		}
		return APPROVED;

	}


}

//------------------------------------------------------------------------------------------------
/*Function desciption: This function is edited to work when using FILE database, it compares card data with the data (PAN) that is t
 * read from the file and stored in a temp account struct*/

EN_serverError_t isValidAccount_FILE(ST_cardData_t *cardData, ST_accountsDB_t *accountRefrence)
{
	uint8_t i=0;


		/*comparing the PAN of the card with the PAN of each account*/
		while(cardData->primaryAccountNumber[i]!= '\0')
		{

			/*if any PAN number differs than the one on the server break the loop and check next account*/
			if(cardData->primaryAccountNumber[i] == accountRefrence->primaryAccountNumber[i])
			{

				i++;
				continue;
			}
			else
			{
				return ACCOUNT_NOT_FOUND;


			}


		}


		return SERVER_OK;


}

//------------------------------------------------------------------------------------------------
/*This function takes pointer to the global TRANSACTION struct, it saves the transactions in a text file and uses a temp file
 * to update the transactions after each new transaction*/
EN_serverError_t saveTransaction_FILE(ST_transaction_t *transData)
{

	FILE* ptr_transactionsDB_FILE;
	FILE* ptr_transactionsDB_FILE_temp;

	/*temp struct to temporarily save transactions to buffer to the file*/
	ST_transaction_t transactions_buffer;

	/*open transactionsDB*/
	if((ptr_transactionsDB_FILE=fopen("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\transactionsDB.txt","rb"))==NULL)
	{
		printf("couldn't open transactionsDB\n");
		return SAVING_FAILED;

	}

	/*open temp file*/
	if((ptr_transactionsDB_FILE_temp=fopen("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\transactionsDB_temp.txt","wb"))==NULL)
	{
		printf("couldn't open transactionsDB_temp\n");
		return SAVING_FAILED;

	}

	while(fread(&transactions_buffer,sizeof(transactions_buffer),1,ptr_transactionsDB_FILE))
	{
		/*buffer transactions to the temp file*/
		fwrite(&transactions_buffer,sizeof(transactions_buffer),1,ptr_transactionsDB_FILE_temp);

	}

	/*add the new transaction when you finish buffering*/
	fwrite(transData,sizeof(*transData),1,ptr_transactionsDB_FILE_temp);

	transData->transactionSequenceNumber++; //increment the sequence number in the global struct

	fclose(ptr_transactionsDB_FILE);
	fclose(ptr_transactionsDB_FILE_temp);

	/*remove the old database and update it with the new temp database file*/
	remove("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\transactionsDB.txt");
	rename("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\transactionsDB_temp.txt","transactionsDB.txt");

	/*list all saved transactions*/
	listSavedTransactions_FILE();
	return SERVER_OK;




}
/*This function lists all the transactions saved in the transactionsDB*/
void listSavedTransactions_FILE(void)
{
	FILE * ptr_transactionsDB_FILE;
	ST_transaction_t transactions_buffer;
	EN_transState_t Transaction_State;



	/*open transactionsDB*/
	if((ptr_transactionsDB_FILE=fopen("D:\\Embedded systems\\Mohamed Amr codes\\MA projects\\Payment Application\\transactionsDB.txt","rb"))==NULL)
	{
		printf("couldn't open transactionsDB to list\n");

	}

	while(fread(&transactions_buffer,sizeof(transactions_buffer),1,ptr_transactionsDB_FILE))
	{


		Transaction_State=transactions_buffer.transState;

		printf("\n##############################\n");


		printf("Transaction Sequence Number: %d\n",transactions_buffer.transactionSequenceNumber);
		printf("Transaction Date: %s\n",transactions_buffer.terminalData.transactionDate);
		printf("Transaction Amount: %.2f\n",transactions_buffer.terminalData.transAmount);

		if(Transaction_State== APPROVED)
		{
			printf("Transaction State: APPROVED\n");
		}
		else if(Transaction_State== DECLINED_INSUFFECIENT_FUND)
		{
			printf("Transaction State: DECLINED_INSUFFECIENT_FUND\n");
		}
		else if(Transaction_State== DECLINED_STOLEN_CARD)
		{
			printf("Transaction State: DECLINED_STOLEN_CARD\n");
		}
		else if(Transaction_State== FRAUD_CARD)
		{
			printf("Transaction State: FRAUD_CARD\n");
		}
		else if(Transaction_State== INTERNAL_SERVER_ERROR)
		{
			printf("Transaction State: INTERNAL_SERVER_ERROR\n");
		}

		printf("Terminal max amount: %.2f\n",transactions_buffer.terminalData.maxTransAmount);

		printf("CardHolder Name: %s\n",transactions_buffer.cardHolderData.cardHolderName);

		printf("PAN: %s\n",transactions_buffer.cardHolderData.primaryAccountNumber);

		printf("Card Expiration Date: %s\n",transactions_buffer.cardHolderData.cardExpirationDate);

		printf("\n##############################\n");


	}

	fclose(ptr_transactionsDB_FILE);


}
//-------------------------------Function used to create and Initialize DATABASE-----------------------------------------------

/*Function Description: This function is created to fill in the database with valid accounts
 * using pointer to struct, giving the pointer the address of the array to fill the data in the first struct
 * then incrementing the pointer to fill the next struct
 * */
void filling_accountsDB(ST_accountsDB_t **ptr_accountsDB,float balance, EN_accountState_t state, uint8_t PAN[] )
{
	(*ptr_accountsDB)->balance=balance;
	(*ptr_accountsDB)->state=state;
	str_cpy((*ptr_accountsDB)->primaryAccountNumber,PAN);
	(*ptr_accountsDB)++;//increment the pointer to fill the next account

}

//-------------------------------------------------------------------------------------------

/*Function Description: This function Initializes the accounts DB*/
void accountsDB_Start_Array(void)
{
	/*pointer created to fill the database*/
	ST_accountsDB_t *ptr_accounts_filler;
	ptr_accounts_filler=accountsDB;


	/*Filling the accountsDB with valid data using ARRAY*/
	filling_accountsDB(&ptr_accounts_filler,2000.0,RUNNING,(uint8_t*)"8989374615436851");
	filling_accountsDB(&ptr_accounts_filler,100000.0,BLOCKED,(uint8_t*)"5807007076043875");
	filling_accountsDB(&ptr_accounts_filler,500.0,RUNNING,(uint8_t*)"88496147450549162");
	filling_accountsDB(&ptr_accounts_filler,5000.0,RUNNING,(uint8_t*)"880414283913348071");
	filling_accountsDB(&ptr_accounts_filler,30000.0,RUNNING,(uint8_t*)"4681383168647409262");


}


//------------------------------------------------------------------------------------------------
/*Function Description: This function creats the accounts database file and fill it with accounts*/
void accountsDB_start_FILE(void)
{

	/*opening accountsDB file */
	FILE *ptr_accounts_filler_FILE;
	if((ptr_accounts_filler_FILE=fopen("accountsDB.txt","wb"))==NULL)
		{
			printf("Failed to open accounts Database 1 ");
			exit(1);
		}

	/*Filling the accountsDB with valid data using FILE*/
	filling_accountsDB_file(ptr_accounts_filler_FILE,2000.0,RUNNING,(uint8_t*)"8989374615436851");
	filling_accountsDB_file(ptr_accounts_filler_FILE,100000.0,BLOCKED,(uint8_t*)"5807007076043875");
	filling_accountsDB_file(ptr_accounts_filler_FILE,500.0,RUNNING,(uint8_t*)"88496147450549162");
	filling_accountsDB_file(ptr_accounts_filler_FILE,5000.0,RUNNING,(uint8_t*)"880414283913348071");
	filling_accountsDB_file(ptr_accounts_filler_FILE,30000.0,RUNNING,(uint8_t*)"4681383168647409262");

	fclose(ptr_accounts_filler_FILE);




}

//----------------------------------------------------------------------------------------------
/* Function Description: this function is created to start a new empty file when running the program*/
void transactionsDB_start_FILE(void)
{


		/*opening transactionsDB file:*/
		FILE *ptr_transactions_FILE;
		if((ptr_transactions_FILE=fopen("transactionsDB.txt","wb"))==NULL)
			{
				printf("Failed to open transactions Database 1 ");
				exit(1);
			}



		fclose(ptr_transactions_FILE);


}

//----------------------------------------------Testing functions---------------------------------
/*function to test isValidAccount funciton*/
void isValidAccountTest(void)
{
	EN_serverError_t res;

	ST_cardData_t test_card;  // variable to test
	ST_cardData_t *ptr_card;
	ptr_card=&test_card; //pointer to cardata

	ST_accountsDB_t *ptr_server;
	ptr_server=accountsDB;


	getCardPAN(ptr_card); //fill the card data with PAN

	printf("PAN entered is: %s\n",ptr_card->primaryAccountNumber);


	res=isValidAccount(ptr_card,ptr_server);

	if(res==SERVER_OK)
	{
		printf("SERVER OK");
	}
	else if (res==ACCOUNT_NOT_FOUND)
	{
		printf("ACCOUNT NOT FOUND");
	}


}

//------------------------------------------------------------------------------------------------
/*Testing isBlockedAccount function*/
void isBlockedAccountTest(void)
{
	EN_serverError_t res;



	ST_cardData_t test_card2;  // variable to test
	ST_cardData_t *ptr_card2;
	ptr_card2=&test_card2; //pointer to cardata

	ST_accountsDB_t *ptr_server2;
	ptr_server2=accountsDB;


	getCardPAN(ptr_card2); //fill the card data with PAN




	isValidAccount(ptr_card2,ptr_server2);


	res=isBlockedAccount(account_reference);

	if(res== SERVER_OK)
	{
		printf("RUNNING");
	}
	else if (res== BLOCKED_ACCOUNT)
	{
		printf("BLOCKED");
	}
	else
	{
		printf("NOT A VALID ACCOUNT");
	}



}


//-----------------------------------------------------------------------------------------------
/*Testing isAmountAvailable function*/

void isAmountAvailableTest(void)
{



	EN_serverError_t res;


	ST_terminalData_t Tran_amount;  //struct variable of type ST_terminalData_t
	ST_terminalData_t *ptr_tran;  //pointer to that struct

	ptr_tran=&Tran_amount;

	getTransactionAmount(ptr_tran);

	res=isAmountAvailable(ptr_tran,account_reference);

	if(res== LOW_BALANCE)
	{
		printf("LOW Balance");
	}
	else
	{
		printf("SERVER OK");

	}




}
//----------------------------------------------------------------------------------------------
/*Testing recieveTransactionData,saveTransaction,listSavedTransactions Functions together
 * as they are dependent on each other
 * in this function only specific parts are tested not the whole program, data used to test
 * these functions are made to pass validation cases (as expiration date and etc,) so they are not
 * checked here*/

void recieveTransactionDataTest(void)
{
	EN_transState_t receive_transaction_data;

	/*passing cardata to the global variable TRANSACTION*/
	getCardHolderName(&ptr_TRANSACTION->cardHolderData);
	getCardExpiryDate(&ptr_TRANSACTION->cardHolderData);
	getCardPAN(&ptr_TRANSACTION->cardHolderData);

	/*passing terminal data*/
	getTransactionAmount(&ptr_TRANSACTION->terminalData);
	setMaxAmount(&ptr_TRANSACTION->terminalData,10000);
	getTransactionDate(&ptr_TRANSACTION->terminalData);


	receive_transaction_data=recieveTransactionData(ptr_TRANSACTION);

	if(receive_transaction_data==APPROVED)
	{
		printf("APPROVED\n");
	}
	else if(receive_transaction_data==DECLINED_INSUFFECIENT_FUND)
	{
		printf("DECLINED_INSUFFECIENT_FUND\n");
	}
	else if(receive_transaction_data==DECLINED_STOLEN_CARD)
	{
		printf("DECLINED_STOLEN_CARD\n");
	}
	else if(receive_transaction_data==FRAUD_CARD)
	{
		printf("FRAUD_CARD\n");
	}
	else if(receive_transaction_data==INTERNAL_SERVER_ERROR)
	{
		printf("INTERNAL_SERVER_ERROR\n");
	}

}







