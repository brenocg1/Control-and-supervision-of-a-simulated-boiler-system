/* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "sensor.h"

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 
static pthread_cond_t alarme = PTHREAD_COND_INITIALIZER; 
static double sensor_lido = 0;
static double limite_atual = HUGE_VAL;

// Sensores
// Ta, T, Ti, No e H
static float sensor_lido_tempAr = 0; //Ta //Pertubação
static float sensor_lido_temp = 0; //T 
static float sensor_lido_temp_entrada = 0; //Ti //Pertubação 
static float sensor_lido_fluxo_saida = 0; //No // Pertubação
static float sensor_lido_altura = 0; //H 

static pthread_mutex_t mutex_tempAr = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_temp = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_temp_entrada = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_altura = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_fluxo_saida = PTHREAD_MUTEX_INITIALIZER; 

Sensors sensor_get_all(){
	Sensors s;
	s.tempAr = sensor_get_tempAr_Ta();
	s.temp = sensor_get_temp_T();
	s.temp_entrada = sensor_get_temp_entrada_Ti();
	s.altura = sensor_get_altura_H();
	s.fluxo_saida = sensor_get_fluxo_saida_No();
	return s;
}

void sensor_alarme_temperatura(double limite) {
	pthread_mutex_lock(&exclusao_mutua); 
	limite_atual = limite; 
	while(sensor_lido_temp < limite_atual) 
		pthread_cond_wait(&alarme, &exclusao_mutua); 
	limite_atual = HUGE_VAL; 
	pthread_mutex_unlock(&exclusao_mutua); 
}

// set e get da agua que sai No
void sensor_put_fluxo_saida_No(float lido) {
	 pthread_mutex_lock(&mutex_fluxo_saida); 
	 sensor_lido_fluxo_saida = lido; 
	 pthread_mutex_unlock(&mutex_fluxo_saida); 
}

float sensor_get_fluxo_saida_No(void) {
	 float aux; 
	 pthread_mutex_lock(&mutex_fluxo_saida); 
	 aux = sensor_lido_fluxo_saida; 
	 pthread_mutex_unlock(&mutex_fluxo_saida); 
	 return aux; 
}


// set e get da altura da agua
void sensor_put_altura_H(float lido) {
	 pthread_mutex_lock(&mutex_altura); 
	 sensor_lido_altura = lido; 
	 pthread_mutex_unlock(&mutex_altura); 
}

float sensor_get_altura_H(void) {
	 float aux; 
	 pthread_mutex_lock(&mutex_altura); 
	 aux = sensor_lido_altura; 
	 pthread_mutex_unlock(&mutex_altura); 
	 return aux; 
}


// set e get da temperatura da agua que entra.  
void sensor_put_temp_entrada_Ti(float lido) {
	 pthread_mutex_lock(&mutex_temp_entrada); 
	 sensor_lido_temp_entrada = lido; 
	 pthread_mutex_unlock(&mutex_temp_entrada); 
}

float sensor_get_temp_entrada_Ti(void) {
	 float aux; 
	 pthread_mutex_lock(&mutex_temp_entrada); 
	 aux = sensor_lido_temp_entrada; 
	 pthread_mutex_unlock(&mutex_temp_entrada); 
	 return aux; 
}


// set e get da temperatura dentro.  
void sensor_put_temp_T(float lido) {
	 pthread_mutex_lock(&mutex_temp); 
	 sensor_lido_temp = lido; 
	 pthread_mutex_unlock(&mutex_temp); 
}

float sensor_get_temp_T(void) {
	 float aux; 
	 pthread_mutex_lock(&mutex_temp); 
	 aux = sensor_lido_temp; 
	 pthread_mutex_unlock(&mutex_temp); 
	 return aux; 
}


// set e get da temperatura do ar fora.
void sensor_put_tempAr_Ta(float lido) {
	 pthread_mutex_lock(&mutex_tempAr); 
	 sensor_lido_tempAr = lido; 
	 pthread_mutex_unlock(&mutex_tempAr); 
}

float sensor_get_tempAr_Ta(void) {
	 float aux; 
	 pthread_mutex_lock(&mutex_tempAr); 
	 aux = sensor_lido_tempAr; 
	 pthread_mutex_unlock(&mutex_tempAr); 
	 return aux; 
}



// Exemplos:
void sensor_put(double lido) {
	 pthread_mutex_lock(&exclusao_mutua); 
	 sensor_lido = lido; 
	 if( sensor_lido >= limite_atual) 
	 	pthread_cond_signal(&alarme); 
	 pthread_mutex_unlock(&exclusao_mutua); 
}
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
double sensor_get(void) {
	 double aux; 
	 pthread_mutex_lock(&exclusao_mutua); 
	 aux = sensor_lido; 
	 pthread_mutex_unlock(&exclusao_mutua); 
	 return aux; 
}

/* Thread fica bloqueada até o valor do sensor chegar em limite */ 
void sensor_alarme( double limite) {
	pthread_mutex_lock(&exclusao_mutua); 
	limite_atual = limite; 
	while(sensor_lido < limite_atual) 
		pthread_cond_wait(&alarme, &exclusao_mutua); 
	limite_atual = HUGE_VAL; 
	pthread_mutex_unlock(&exclusao_mutua); 
}

