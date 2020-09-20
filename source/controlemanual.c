/**
******************************************************************************
*-> file: controlemanual.c
*-> author: Breno Campos - brenocg@alu.ufc.br
*-> version: 1.0
*-> date: 10/August/2020
*-> Description: Controle e supervisão de caldeira
******************************************************************************
*	COMPILE -> make
*	EXEC -> make run
*	A porta padrão usada no Makefile é a 4545
* 	E o IP padrão é localhost
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "sensor.h"
#include "socket.h"
#include "tela.h"
#include "bufduplo.h"
#include "bufduplo-tempo-resposta.h"


#define FALHA 1
#define NSEC_PER_SEC (1000000000)


/*****************************************************************************
**                				GLOBAl VARIABLES
*****************************************************************************/

// default ta localhost, mas na main eu pego dos argumentos 
char * ip = "localhost";

// solicitado na main
float temp_padrao = 0;
float altura_padrao = 0;
int socket_local = 0;
int porta_destino = 0;
struct sockaddr_in endereco_destino;

/*****************************************************************************
**                					UTILS
*****************************************************************************/

// remove sub-string de string
// para remover os st- das mensagems recebidas
char *strremove(char *str, const char *sub) {
	// Função para tirar as letras no output dos sensores 
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        size_t size = 0;
        while ((p = strstr(p, sub)) != NULL) {
            size = (size == 0) ? (p - str) + strlen(p + len) + 1 : size - len;
            memmove(p, p + len, size - (p - str));
        }
    }
    return str;
}

// arrendonda o float para 1 casa decimal
float round_2_decimals(float value){
	return roundf(value * 10) / 10;
}

/*****************************************************************************
**                			ALARME DE TEMPERATURA
*****************************************************************************/

void alarme_temperatura(){
	struct timespec t;
	long int periodo = 10000000; // 10ms
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Tarefa periodica iniciará em 1 segundo
	t.tv_sec++;

	while(1) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		sensor_alarme_temperatura(temp_padrao);
		lock_tela();
		puts("ALARME BIP BOP!!");
		unlock_tela();

		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
}

/*****************************************************************************
**                		FUNÇÕES DE SETAR ATUADORES
*****************************************************************************/

pthread_mutex_t mutex_Ni = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Nf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Na = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_Q = PTHREAD_MUTEX_INITIALIZER;

float Ni = 0.0;
// “Ni” fluxo de água de entrada do recipiente
void atuador_set_Ni(float valor){
	pthread_mutex_lock(&mutex_Ni);
	char msg_enviada[1000];
	sprintf(msg_enviada, "ani%f", valor);
	envia_mensagem(socket_local, endereco_destino, msg_enviada);
	Ni = valor;
	pthread_mutex_unlock(&mutex_Ni);
}

float Q = 0.0;
// “Q” fluxo de calor do elemento aquecedor
void atuador_set_Q(float valor){
	pthread_mutex_lock(&mutex_Q);
	char msg_enviada[1000];
	sprintf(msg_enviada, "aq-%f", valor);
	Q = valor;
	envia_mensagem(socket_local, endereco_destino, msg_enviada);
	pthread_mutex_unlock(&mutex_Q);
}

float Na = 0.0;
// “Na” fluxo de água aquecida a 80C de entrada controlada
void atuador_set_Na(float valor){
	pthread_mutex_lock(&mutex_Na);
	char msg_enviada[1000];
	sprintf(msg_enviada, "ana%f", valor);
	Na = valor;
	envia_mensagem(socket_local, endereco_destino, msg_enviada);
	pthread_mutex_unlock(&mutex_Na);
}

float Nf = 0.0;
// “Nf” fluxo de água de saída para esgoto controlada
void atuador_set_Nf(float valor){
	pthread_mutex_lock(&mutex_Nf);
	char msg_enviada[1000];
	sprintf(msg_enviada, "anf%f", valor);
	Nf = valor;
	envia_mensagem(socket_local, endereco_destino, msg_enviada);
	pthread_mutex_unlock(&mutex_Nf);
}


/*****************************************************************************
**                			ATUALIZA SENSORES
*	pega os valores dos sensores, imprime-os na tela e atualiza no sensor.c
*****************************************************************************/

void tela_sensores(){
	Sensors s = sensor_get_all();
	lock_tela();
	puts("###################################");
	puts("TELA DE MONITORAMENTO");
	printf("\nT - Temperatura da água no interior do recipiente = %.3f", s.temp);
	printf("\nTa - Temperatura do ar ambiente em volta do recipiente = %.3f", s.tempAr);
	printf("\nTi - Temperatura da água que entra no recipiente = %.3f", s.temp_entrada);
	printf("\nNo - Fluxo de água de saída do recipiente = %.3f", s.fluxo_saida);
	printf("\nH - Altura da coluna de água dentro do recipiente = %.3f", s.altura);
	puts("\n###################################");
	unlock_tela();
}

int tela_sensores_tarefa(int argc, char* argv[])
{
	struct timespec t;
	long int periodo = 100000000; 	// 100ms
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Tarefa periodica iniciará em 1 segundo
	t.tv_sec++;

	while(1) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		// imprimindo valores na tela
		tela_sensores();

		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
}

void atualiza_sensores(){
	// Tarefa que irá atualizar as variáveis sensores no socket, e mostrar na tela
	// configs da tarefa periódica
	struct timespec t;
	long int periodo = 30000000;  // 30ms	
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Tarefa periodica iniciará em 1 segundo
	t.tv_sec++;
	// FIM das configs

	while(1){
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		// argv[2] = porta
		// char * porta = (char *) param;
		// porta_destino = atoi(porta);
		// socket_local = cria_socket_local();

		// ip = argv[1] = localhost
		
		// endereco_destino = cria_endereco_destino(ip, porta_destino);

		char msg_enviada[1000];
		char msg_recebida[1000];

		int nrec;

		//H - Altura da coluna de água dentro do recipiente
		strcpy(msg_enviada, "sh-0");
		envia_mensagem(socket_local, endereco_destino, msg_enviada);
		nrec = recebe_mensagem(socket_local, msg_recebida, 1000);
		msg_recebida[nrec] = '\0';
		strremove(msg_recebida, "sh-");
		float altura = atof(msg_recebida);
		if(altura > 0) sensor_put_altura_H(altura);

		//T - Temperatura
		strcpy(msg_enviada, "st-0");
		envia_mensagem(socket_local, endereco_destino, msg_enviada);
		nrec = recebe_mensagem(socket_local, msg_recebida, 1000);
		msg_recebida[nrec] = '\0';
		strremove(msg_recebida, "st-");
		float temp = atof(msg_recebida);
		if(temp > 0) sensor_put_temp_T(temp);

		//Ta - Temperatura do ar ambiente em volta do recipiente
		strcpy(msg_enviada, "sta0");
		envia_mensagem(socket_local, endereco_destino, msg_enviada);
		nrec = recebe_mensagem(socket_local, msg_recebida, 1000);
		msg_recebida[nrec] = '\0';
		strremove(msg_recebida, "sta");
		float temp_ar = atof(msg_recebida);
		if(temp_ar > 0) sensor_put_tempAr_Ta(temp_ar);

		//Ti - Temperatura da água que entra no recipiente
		strcpy(msg_enviada, "sti0");
		envia_mensagem(socket_local, endereco_destino, msg_enviada);
		nrec = recebe_mensagem(socket_local, msg_recebida, 1000);
		msg_recebida[nrec] = '\0';
		strremove(msg_recebida, "sti");
		float temp_agua_entra = atof(msg_recebida);
		if(temp_agua_entra > 0) sensor_put_temp_entrada_Ti(temp_agua_entra);

		//No - Fluxo de água de saída do recipiente
		strcpy(msg_enviada, "sno0");
		envia_mensagem(socket_local, endereco_destino, msg_enviada);
		nrec = recebe_mensagem(socket_local, msg_recebida, 1000);
		msg_recebida[nrec] = '\0';
		strremove(msg_recebida, "sno");
		float fluxo_agua = atof(msg_recebida);
		if(fluxo_agua > 0) sensor_put_fluxo_saida_No(fluxo_agua);
				
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}

}

/*****************************************************************************
**                		  CONTROLE DE TEMPERATURA
*****************************************************************************/

void controle_temp(){
	struct timespec t, t_fim;
	long int periodo = 50000000; // 50ms
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Tarefa periodica iniciará em 1 segundo
	t.tv_sec++;

	while(1) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		const float passo = 0.5;
		const float passo_largo = 1000;

		// Controle de Caldeira
		if(sensor_get_temp_T() > temp_padrao){
			// diminuir temp
			atuador_set_Ni(Ni + passo);
			atuador_set_Na(Na - passo);
			atuador_set_Q(Q - passo_largo);
		}else{
			// aumentar temp
			atuador_set_Ni(Ni - passo);
			atuador_set_Na(Na + passo);
			atuador_set_Q(Q + passo_largo);
		}

		// value bounds
		if(Ni < 0) Ni = 0;
		else if(Ni >= 100) atuador_set_Ni(100);

		if(Na < 0) Na = 0;
		else if(Na >= 10) atuador_set_Na(10);
		
		if(Q < 0) Q = 0;
		else if(Q >= 1010000) atuador_set_Q(1000000);

		// Le a hora atual, coloca em t_fim
		clock_gettime(CLOCK_MONOTONIC ,&t_fim);

		// Calcula o tempo de resposta observado em microsegundos
		double tempResp = (double) 1000000 * (t_fim.tv_sec - t.tv_sec) + (t_fim.tv_nsec - t.tv_nsec)/1000;

		bufduplo_insereLeitura_temperatura(tempResp);

		//Fim do controle 
		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
}

/*****************************************************************************
**                		    CONTROLE DE ALTURA
*****************************************************************************/

int controle_altura(){
	struct timespec t, t_fim;
	long int periodo = 70000000; 	// 70ms
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Tarefa periodica iniciará em 1 segundo
	t.tv_sec++;

	while(1) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		const float passo = 0.5;
		// COMEÇO DA TAREFA
		if(sensor_get_altura_H() > altura_padrao){
			// diminuir nivel de agua
			atuador_set_Nf(Nf + passo);
			atuador_set_Na(Na - passo);
		}else{
			// aumentar nivel de agua
			atuador_set_Nf(Nf - passo);
			atuador_set_Na(Na + passo);
			atuador_set_Ni(Ni + (passo * 20));
		}

		// VALUES BOUNDS	
		if(Nf < 0) atuador_set_Nf(0);
		else if (Nf >= 50) atuador_set_Nf(15);

		if(Na < 0) atuador_set_Na(0);
		else if (Na >= 50) atuador_set_Na(10);

		if(Ni < 0) Ni = 0;
		else if(Ni >= 100) atuador_set_Ni(90);

		// FINAL DA TAREFA

		// Le a hora atual, coloca em t_fim
		clock_gettime(CLOCK_MONOTONIC ,&t_fim);

		// Calcula o tempo de resposta observado em microsegundos
		double tempResp = (double) 1000000 * (t_fim.tv_sec - t.tv_sec) + (t_fim.tv_nsec - t.tv_nsec)/1000;

		bufduplo_insereLeitura_altura(tempResp);

		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
}

/*****************************************************************************
**          		   GRAVAÇÃO DOS DADOS DOS SENSORES
*****************************************************************************/

void grava_sensores(){
	Sensors s = sensor_get_all();
	bufduplo_sensores_insereLeitura((double) s.temp);
	bufduplo_sensores_insereLeitura((double) s.fluxo_saida);
	bufduplo_sensores_insereLeitura((double) s.altura);
}

void tarefa_grava_sensores(){
	struct timespec t;
	long int periodo = 200000000; // 200ms
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Tarefa periodica iniciará em 1 segundo
	t.tv_sec++;

	while(1) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		
		// inserindo dados no buffer
		grava_sensores();
		
		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
}

void tarefa_grava_arquivo(void){
	FILE * dados;
	dados = fopen("dados_sensores.txt", "w");
    if(dados == NULL){
		lock_tela();
        printf("Erro, nao foi possivel abrir o arquivo\n");
		unlock_tela();
        exit(1);    
    }
	fprintf(dados, "TEMPERATURA;");
	fprintf(dados, "FLUXO-SAIDA;");
	fprintf(dados, "ALTURA\n");
	while(1){
		double * buf = bufduplo_sensores_esperaBufferCheio();
		int i = 0;
		while(i < 12){
			fprintf(dados, "%f;", buf[i++]);
			fprintf(dados, "%f;", buf[i++]);
			fprintf(dados, "%f", buf[i++]);
			fprintf(dados, "\n");
		}
		fflush(dados);
	}
	fclose(dados);
}

/*****************************************************************************
**           	 GRAVAÇÃO DOS DADOS DOS TEMPOS DE RESPOSTAS
*****************************************************************************/

void tarefa_grava_arquivo_tempo_resposta_temperatura(void){
	FILE * dados;
	dados = fopen("response-time-temp.txt", "w");
    if(dados == NULL){
		lock_tela();
        printf("Erro, nao foi possivel abrir o arquivo\n");
		unlock_tela();
        exit(1);    
    }

	int nAmostras = 0;

	while(1){
		double * buf = bufduplo_esperaBufferCheio_temperatura();
		int i = 0;
		while(i < 12){
			fprintf(dados, "%f\n", buf[i++]);
			fprintf(dados, "%f\n", buf[i++]);
			fprintf(dados, "%f\n", buf[i++]);
		}
		fflush(dados);
		nAmostras += 3;
		if(nAmostras >= 10000) exit(1);
	}
	fclose(dados);
}

void tarefa_grava_arquivo_tempo_resposta_altura(void){
	FILE * dados;
	dados = fopen("response-time-altura.txt", "w");
    if(dados == NULL){
		lock_tela();
        printf("Erro, nao foi possivel abrir o arquivo\n");
		unlock_tela();
        exit(1);    
    }
	int nAmostras = 0;
	while(1){
		double * buf = bufduplo_esperaBufferCheio_altura();
		int i = 0;
		while(i < 12){
			fprintf(dados, "%f\n", buf[i++]);
			fprintf(dados, "%f\n", buf[i++]);
			fprintf(dados, "%f\n", buf[i++]);
		}
		nAmostras += 3;
		fflush(dados);
		if(nAmostras >= 10000) exit(1);
	}
	fclose(dados);
}



/*****************************************************************************
**                					MAIN
*****************************************************************************/

int main(int argc, char *argv[]){
	if (argc < 3) { 
		fprintf(stderr,"Uso: controlemanual <endereco> <porta>\n");
		fprintf(stderr,"<endereco> eh o endereco IP da caldeira\n");
		fprintf(stderr,"<porta> eh o numero da porta UDP da caldeira\n");
		fprintf(stderr,"Exemplo de uso:\n");
		fprintf(stderr,"   controlemanual localhost 12345\n");
		exit(FALHA);
	}

	ip = argv[1];

	char * porta = argv[2];
	porta_destino = atoi(porta);
	socket_local = cria_socket_local();
	endereco_destino = cria_endereco_destino(ip, porta_destino);
	
	lock_tela();
	
	puts("\nDigite o valor de temperatura ideal");
	scanf("%f", &temp_padrao);

	puts("\nDigite o valor da altura ideal");
	scanf("%f", &altura_padrao);
	
	unlock_tela();

	pthread_t sensores, controle_t, controle_h, tela, alarme;
	
	pthread_t grava_sensores, grava_arquivo, grava_arquivo_temp, grava_arquivo_altura;

	pthread_create(&sensores, NULL,(void *) atualiza_sensores, NULL);
	pthread_create(&controle_t, NULL, (void *) controle_temp, NULL);
	pthread_create(&controle_h, NULL, (void *) controle_altura, NULL);
	pthread_create(&tela, NULL, (void *) tela_sensores_tarefa, NULL);
	pthread_create(&alarme, NULL, (void *) alarme_temperatura, NULL);
	pthread_create(&grava_sensores, NULL, (void *) tarefa_grava_sensores, NULL);
	pthread_create(&grava_arquivo, NULL, (void *) tarefa_grava_arquivo, NULL);
	pthread_create(&grava_arquivo_temp, NULL, (void *) tarefa_grava_arquivo_tempo_resposta_temperatura, NULL);
	pthread_create(&grava_arquivo_altura, NULL, (void *) tarefa_grava_arquivo_tempo_resposta_altura, NULL);

	pthread_join(sensores, NULL);
	pthread_join(tela, NULL);
	pthread_join(controle_t, NULL);
	pthread_join(controle_h, NULL);
	pthread_join(alarme, NULL);
	pthread_join(grava_sensores, NULL);
	pthread_join(grava_arquivo, NULL);
	pthread_join(grava_arquivo_temp, NULL);
	pthread_join(grava_arquivo_altura, NULL);
}
