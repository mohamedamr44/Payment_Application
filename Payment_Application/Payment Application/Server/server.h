/*
 * server.h
 *
 *  Created on: Apr 3, 2024
 *      Author: Mohamed Amr
 */

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include "../Terminal/terminal.h"


#define PAN_SIZE 20
#define ACC_MAX 255
#define TRANSDB_MAX 255



typedef enum EN_transState_t
{
     APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR
}EN_transState_t;

typedef struct ST_transaction_t
{
     ST_cardData_t cardHolderData;
     ST_terminalData_t terminalData;
     EN_transState_t transState;
     uint32_t transactionSequenceNumber;
}ST_transaction_t;

typedef enum EN_serverError_t
{
     SERVER_OK, SAVING_FAILED, TRANSACTION_NOT_FOUND, ACCOUNT_NOT_FOUND, LOW_BALANCE, BLOCKED_ACCOUNT
}EN_serverError_t ;

typedef enum EN_accountState_t
{
     RUNNING,
     BLOCKED
}EN_accountState_t;

typedef struct ST_accountsDB_t
{
     float balance;
     EN_accountState_t state;
     uint8_t primaryAccountNumber[20];
}ST_accountsDB_t;

EN_transState_t recieveTransactionData(ST_transaction_t *transData);
EN_serverError_t isValidAccount(ST_cardData_t *cardData, ST_accountsDB_t *accountRefrence);
EN_serverError_t isBlockedAccount(ST_accountsDB_t *accountRefrence);
EN_serverError_t isAmountAvailable(ST_terminalData_t *termData, ST_accountsDB_t *accountRefrence);
EN_serverError_t saveTransaction(ST_transaction_t *transData);
void listSavedTransactions(void);

/*function created to fill the accountsDB with valid data using global array*/
void accountsDB_Start_Array(void);
void filling_accountsDB(ST_accountsDB_t **ptr_accountsDB,float balance, EN_accountState_t state, uint8_t PAN[]);
void accountsDB_start_FILE(void);
void transactionsDB_start_FILE(void);


/*function created to fill accountsDB using text file*/
void filling_accountsDB_file(FILE * ptr_accountsDB_FILE,float balance,EN_accountState_t state,uint8_t PAN[]);


/*Testing functions*/

void isValidAccountTest(void);
void isBlockedAccountTest(void);
void isAmountAvailableTest(void);
void recieveTransactionDataTest(void);
/*These two functions are tested in recieveTransactionDataTest
as they are dependant on each other
void saveTransactionTest(void);
void listSavedTransactionsTest(void);*/




/*functions edited to be used with FILE database*/
EN_transState_t recieveTransactionData_FILE(ST_transaction_t *transData);
EN_serverError_t isValidAccount_FILE(ST_cardData_t *cardData, ST_accountsDB_t *accountRefrence);
EN_serverError_t saveTransaction_FILE(ST_transaction_t *transData);
void listSavedTransactions_FILE(void);




/*global variables for database*/
extern ST_accountsDB_t accountsDB[ACC_MAX];
extern ST_accountsDB_t *account_reference;

extern ST_transaction_t transactionsDB[TRANSDB_MAX];
extern ST_transaction_t * ptr_transactionsDB;

extern ST_transaction_t TRANSACTION;
extern ST_transaction_t *const ptr_TRANSACTION;




#endif /* SERVER_SERVER_H_ */
