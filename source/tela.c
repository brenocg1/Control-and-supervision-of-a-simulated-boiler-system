/**
******************************************************************************
*-> file: tela.c
*-> author: Breno Campos - brenocg@alu.ufc.br
*-> version: 1.0
*-> date: 18/August/2020
*-> Description: none 	
******************************************************************************
*/

#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t tela = PTHREAD_MUTEX_INITIALIZER; 

void lock_tela(){
	pthread_mutex_lock(&tela);
}

void unlock_tela(){
	pthread_mutex_unlock(&tela);
}

