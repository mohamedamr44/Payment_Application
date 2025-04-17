/*
 * main.c
 *
 *  Created on: Apr 3, 2024
 *      Author: User-H
 */
#include "Application/app.h"


/*Global variable of ST_transaction_t to store all data being processed
 * (card data,terminal data, transaction state, and sequence number)
 * These data that are saved later in the transaction database*/
ST_transaction_t TRANSACTION;
ST_transaction_t *const ptr_TRANSACTION=&TRANSACTION;

ST_accountsDB_t accountsDB[ACC_MAX];//Global array for the server-side valid accounts database
ST_accountsDB_t *account_reference=accountsDB;//Global pointer to accounts dataBase used to give reference to the account being checked

ST_transaction_t transactionsDB[TRANSDB_MAX]={0};//Global array for the server-side transactions database*/
ST_transaction_t * ptr_transactionsDB=transactionsDB; //pointer to transactions database




int main()
{
	/*This two line to clear output buffer*/
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	/*Starting the application*/
	appStart();






	return 0;
}
