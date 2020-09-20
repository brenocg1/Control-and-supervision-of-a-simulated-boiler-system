#ifndef BUFDUPLO_TIME_RESPONSE_H
#define BUFDUPLO_TIME_RESPONSE_H

void bufduplo_insereLeitura_temperatura(double leitura);

double * bufduplo_esperaBufferCheio_temperatura();

void bufduplo_insereLeitura_altura(double leitura);

double * bufduplo_esperaBufferCheio_altura();

#endif