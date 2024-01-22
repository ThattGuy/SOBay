//
// Created by tiagog on 02-11-2022.
//

/*3 THREADS:
    COMANDOS
    HEARTBEATS
    RECEBER
*/

#ifndef TP_SO_FRONTEND_H
#define TP_SO_FRONTEND_H

#include "../globals.h"
#include "../debug.h"

#define SERVERKEY "ServerKey"

int fdEnvio;
int fdResposta;
char SERVIDOR_CLIENTE_FINAL[100];

typedef struct {
    int stop;
    pthread_mutex_t *m;
} GERAL;

typedef struct {
    pthread_t tRecebe;
    pthread_t tHeartbeats;
} COMANDOS;


int main(int argc, char **argv);

void sair( int sign, siginfo_t *info, void *s );

void terminaClienteInvalido();
void recebeComando(const char user[30]);
void *enviaHeartbeats();
void *recebeMensagens();
int login(char userName[], char userPassword[]);
void separaArgumentos(char cmd[CMDSIZE], char **arg, int tam);
int verificaArgumentos(const char cmd[CMDSIZE], int nArg);
void sell(char cmd[CMDSIZE]);
void list();
void licat(char cmd[CMDSIZE]);
void lisel(char cmd[CMDSIZE]);
void lival(char cmd[CMDSIZE]);
void litime(char cmd[CMDSIZE]);
void timer(char cmd[CMDSIZE]);
void buy(char cmd[CMDSIZE]);
void cash(char cmd[CMDSIZE]);
void add(char cmd[CMDSIZE]);
void help();

#endif //TP_SO_FRONTEND_H