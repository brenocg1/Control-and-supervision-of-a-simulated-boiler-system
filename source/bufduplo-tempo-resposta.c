#include <pthread.h>

#define TAMBUF 120

// Buffer duplo dos dados de tempo de resposta da temperatura

static double buffer_0[TAMBUF]; 
static double buffer_1[TAMBUF];

static int emuso = 0; 
static int prox_insercao = 0; 
static int gravar = -1;

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 
static pthread_cond_t buffer_cheio = PTHREAD_COND_INITIALIZER;

void bufduplo_insereLeitura_temperatura(double leitura){
	pthread_mutex_lock(&exclusao_mutua); 
	if(emuso == 0) 
		buffer_0[prox_insercao] = leitura; 
	else 
		buffer_1[prox_insercao] = leitura; 
	++prox_insercao; 
	if(prox_insercao == TAMBUF){ 
		gravar = emuso; 
		emuso = (emuso + 1) % 2; 
		prox_insercao = 0; 
		pthread_cond_signal(&buffer_cheio); 
	} 
	pthread_mutex_unlock(&exclusao_mutua);
}

double * bufduplo_esperaBufferCheio_temperatura(){
	double * buffer; 
	pthread_mutex_lock(&exclusao_mutua); 
	while(gravar == -1) 
		pthread_cond_wait(&buffer_cheio, &exclusao_mutua);
	if(gravar == 0) 
		buffer = buffer_0; 
	else 
		buffer = buffer_1; 
	gravar = -1; 
	pthread_mutex_unlock(&exclusao_mutua); 
	return buffer; 
}


// Buffer duplo dos dados de tempo de resposta da altura
static double buffer_0_H[TAMBUF]; 
static double buffer_1_H[TAMBUF];

static int emuso_H = 0; 
static int prox_insercao_H = 0; 
static int gravar_H = -1;

static pthread_mutex_t exclusao_mutua_altura = PTHREAD_MUTEX_INITIALIZER; 
static pthread_cond_t buffer_cheio_altura = PTHREAD_COND_INITIALIZER;


void bufduplo_insereLeitura_altura(double leitura){
	pthread_mutex_lock(&exclusao_mutua_altura); 
	if(emuso_H == 0) 
		buffer_0_H[prox_insercao_H] = leitura; 
	else 
		buffer_1_H[prox_insercao_H] = leitura; 
	++prox_insercao_H; 
	if(prox_insercao_H == TAMBUF){ 
		gravar_H = emuso_H; 
		emuso_H = (emuso_H + 1) % 2; 
		prox_insercao_H = 0; 
		pthread_cond_signal(&buffer_cheio_altura); 
	} 
	pthread_mutex_unlock(&exclusao_mutua_altura);
}

double * bufduplo_esperaBufferCheio_altura(){
	double * buffer; 
	pthread_mutex_lock(&exclusao_mutua_altura); 
	while(gravar_H == -1) 
		pthread_cond_wait(&buffer_cheio_altura, &exclusao_mutua_altura);
	if(gravar_H == 0) 
		buffer = buffer_0_H; 
	else 
		buffer = buffer_1_H; 
	gravar_H = -1; 
	pthread_mutex_unlock(&exclusao_mutua_altura); 
	return buffer; 
}