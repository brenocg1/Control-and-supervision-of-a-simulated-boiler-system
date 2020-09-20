#ifndef SOCKET_H
#define SOCKET_H

void lock_mutex(void);
void unlock_mutex(void);

int cria_socket_local(void);
struct sockaddr_in cria_endereco_destino(char *destino, int porta_destino);
void envia_mensagem(int socket_local, struct sockaddr_in endereco_destino, char *mensagem);
int recebe_mensagem(int socket_local, char *buffer, int TAM_BUFFER);

#endif