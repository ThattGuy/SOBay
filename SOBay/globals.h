#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/wait.h>

#ifndef TP_SO_GLOBALS_H
#define TP_SO_GLOBALS_H

#define CMDSIZE 40
#define SELLSIZE 6
#define LICATSIZE 2
#define LISELSIZE 2
#define LIVALSIZE 2
#define LITIMESIZE 2
#define BUYSIZE 3
#define ADDSIZE 2

#define SELLCODE 1
#define LIST 2
#define LICAT 3
#define LISEL 4
#define LIVAL 5
#define LITIME 6
#define TIME 7
#define BUYCODE 8
#define CASH 9
#define ADD 10
#define LOGINCODE 11
#define HEARTBEAT 12
#define EXIT 13

#define MAXPAL 50
#define MAXCLI 20
#define MAXPROM 10

#define SERVER_FIFO "SERVIDOR"
#define SERVER_HEARTBEATS "SERVIDOR_HEARTBEATS"
#define CLIENT_FIFO "CLIENTE%d"


typedef struct {
    char userName[15];
    char password[20];
} LOGIN;

typedef struct {
    char nome[20];
    char categoria[20];
    int precoBase;
    int precoCompreJa;
    int duracao;
} SELL;

typedef struct {
    int id;
    int valor;
} BUY;

typedef union {
    char string[500];
    int  numero;
} UMARG;

typedef union {
    LOGIN login;
    SELL sell;
    BUY buy;
    UMARG umarg;
    int confirmacao;
} MENSAGEM;

typedef struct {
    pid_t pid;
    int tipo;
    char key[10];
    MENSAGEM msg;
} PREPARACAO;

#endif