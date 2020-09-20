#ifndef SENSOR_H
#define SENSOR_H

typedef struct Sensors{
	float tempAr; 
	float temp;
	float temp_entrada;  
	float fluxo_saida; 
	float altura;
}Sensors;

Sensors sensor_get_all();

void sensor_alarme_temperatura(double limite);

void sensor_put_fluxo_saida_No (float lido);
float sensor_get_fluxo_saida_No (void);

void sensor_put_altura_H (float lido);
float sensor_get_altura_H (void);

void sensor_put_temp_entrada_Ti (float lido);
float sensor_get_temp_entrada_Ti (void);

void sensor_put_tempAr_Ta (float lido);
float sensor_get_tempAr_Ta (void);

void sensor_put_temp_T (float lido);
float sensor_get_temp_T (void);

void sensor_put(double lido);
double sensor_get(void);
void sensor_alarme(double limite);

#endif



