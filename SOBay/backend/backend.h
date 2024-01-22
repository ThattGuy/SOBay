#ifndef TP_SO_BACKEND_H
#define TP_SO_BACKEND_H

#include "items/itemsList.h"
#include "users/users_lib.h"
char SERVIDOR_CLIENTE_FINAL[100];

typedef struct{
    int tempo;
    char CLIENT_FIFO_FINAL[100];
    int nClientes;
    pItem itemsList;
    int fdServ;
    int stop;
    pthread_mutex_t *m;
    CLIENTES *cli;
} GERAL;

typedef struct{
    char categoria[20];
    int desconto;
    int segundos;
    int ativo;
} PROMOCAO;

typedef struct {
    pid_t pid;
    char nome[50];
    int fdProms[2];
    int nPromocoes;
    PROMOCAO prom[MAXPROM];
    int stop;   // SE ESTIVER EM FUNCIONAMENTO E FOR PARA O TERMINAR FICA A 1
    int ativo;  // SE ESTIVER EM FUNCIONAMENTO FICA A 1
} PROMOTOR; 

typedef struct {
    pthread_t tPromotor;
    PROMOTOR promotor;
} THREAD_PROMOTORES;

typedef struct {
    pthread_t tPromotoresControl;
    THREAD_PROMOTORES prom[MAXPROM];
    pthread_t tClientes;
    pthread_t tCLientesHeartbeats;
    CLIENTES cliente;
} COMANDOS;

typedef struct{
    int tempo;
    int stop;
    COMANDOS comandos;
    pthread_t tComandos;
} TEMPO;

typedef struct {
    TEMPO tempo;
    pthread_mutex_t mutex;
} INICIALIZADOR;

void terminaServerInvalido();
int inicializaServidor(INICIALIZADOR *inicia, char *envp[]);
int avancaTempo(TEMPO *tempo);
int procPromocao(char categoria[40],COMANDOS *coms,int preco);
void trataTempoItems(COMANDOS *coms);
void *recebeHeartbeats(void *clientes);
int leFichProms(THREAD_PROMOTORES *prom);
void *iniciaPromotores(void *promo);
void *recebePromotores();
void resetPromocao(PROMOCAO *p);
void resetPromotor(PROMOTOR *p);
void login(CLIENTES *coms, PREPARACAO ppr);
void terminaCliente(INFOS_CLIENTES *c);
void resetCliente(INFOS_CLIENTES *c);
void sell(CLIENTES *coms, PREPARACAO ppr);
void add(CLIENTES *coms, PREPARACAO ppr);
void cash(CLIENTES *coms, PREPARACAO ppr);
void timer(CLIENTES *coms, PREPARACAO ppr);
void buy(CLIENTES *coms, PREPARACAO ppr);
void litime(CLIENTES *coms, PREPARACAO ppr);
void lival(CLIENTES *coms, PREPARACAO ppr);
void lisel(CLIENTES *coms, PREPARACAO ppr);
void licat(CLIENTES *coms, PREPARACAO ppr);
void list(CLIENTES *coms, PREPARACAO ppr);
void exitCliente(CLIENTES *coms,PREPARACAO ppr);


void acorda(int s, siginfo_t *info, void *c);
void sair( int sign, siginfo_t *info, void *s );
void verificaTempo();
void guardaTempo();
int leFichItems();
void help();
void listaUsers(CLIENTES *clientes);
void listaItems();
void listaProms(THREAD_PROMOTORES *prom);
void UpdateProms(COMANDOS *pComs);
void cancelProms( char comando[ MAXPAL ], char *ptr, THREAD_PROMOTORES *prom,CLIENTES *cli );
void kickUser( char comando[ MAXPAL ], char *ptr,CLIENTES* cli );
void *recebeComando(void *comandos);
void *recebeClientes(void *clientes);

#endif