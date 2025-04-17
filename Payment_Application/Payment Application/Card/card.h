/*
 * card.h
 *
 *  Created on: Apr 3, 2024
 *      Author: Mohamed Amr
 */

#ifndef CARD_CARD_H_
#define CARD_CARD_H_


#include "../my_Library/my_lib.h"

/*getCardHolderName function*/
/*The size 26 is selected to allow the user to enter
 * characters more than 24 so: 25 is proper + the null character sums up to 26*/
#define TEMP__NAME_SIZE 26

#define MAX_STRING 24
#define MIN_STRING 20

#define EMPTY_SPACE 32

#define TEST_CASES 6
#define PAN_TEST_CASES 5

/*getCardExpiryDate function*/
#define TEMP_DATE_SIZE 7 // 6+1=7 same concept as  TEMP__NAME_SIZE
#define DATE_SIZE 5

/*for the Date Formate stored in the array as [M1 M2 / Y1 Y2] */
#define M1 0
#define M2 1
#define SLASH 2
#define Y1 3
#define Y2 4





/*getCardPAN function*/
#define TEMP_PAN_SIZE 21 //20+1
#define PAN_MAX 19
#define PAN_MIN 16











typedef struct ST_cardData_t
{

	uint8_t cardHolderName[25];
	uint8_t primaryAccountNumber[20];
	uint8_t cardExpirationDate[6];
}ST_cardData_t;


typedef enum EN_cardError_t
{
     CARD_OK, WRONG_NAME, WRONG_EXP_DATE, WRONG_PAN
}EN_cardError_t;


EN_cardError_t getCardHolderName(ST_cardData_t *cardData);
EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData);
EN_cardError_t getCardPAN(ST_cardData_t *cardData);



/*Functions created for testing*/
void getCardHolderNameTest(void);
void getCardExpiryDateTest (void);
void getCardPANTest (void);


















#endif /* CARD_CARD_H_ */
