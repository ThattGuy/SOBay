#ifndef ITEMS_LIST_H
#define ITEMS_LIST_H


#include "../../debug.h"
#include "../../globals.h"

#define MAX 40
#define SERVER_KEY "ServerKey"

typedef struct {
    pid_t pid;
    char username[MAX];
    int tempo;
    int ativo;
} INFOS_CLIENTES;

typedef struct {
    INFOS_CLIENTES clientes[MAXCLI];
    int nClientes;
} CLIENTES;

typedef struct object item, *pItem;
struct object
{
    int id;
    char nome[MAX];
    char categoria[MAX];
    int valorBase;
    int compraJa;
    int tempo;
    char vendedor[MAX];
    char comprador[MAX];

    pItem ant;
    pItem prox;
};

// VERIFICA SE A LISTA ESTA VAZIA
int listaVazia( pItem p );

// PROCURA ULTIMO ELEMENTO
pItem procUltimoElemento( pItem p );

// PROCURA POR ID
pItem procPorId( pItem p, int id );

// PROCURA ID ANTERIOR
int procIdAnterior( pItem p, int id );

// PESQUISA CATEGORIA
pItem percorreListaCategoria( pItem p, char *categoria );

// PESQUISA VENDEDOR
pItem percorreListaVendedor( pItem p, char *vendedor );

// PESQUISA VALOR
pItem percorreListaValor( pItem p, int valor );

// PESQUISA TEMPO
pItem percorreListaPrazo( pItem p, int tempo );

// CARREGA ITEM DO FICHEIRO
pItem carregaItem( pItem p,int id, char *nome, char *categoria, int valorBase, int compraJa, int tempo, char *vendedor, char *comprador );

// ADICIONA ITEM A LISTA
pItem addItem( CLIENTES *cli,pItem p, int tempoServer, char *nome, char *categoria, int valorBase, int compraJa, int tempo, char *vendedor, char *comprador );

// REMOVE ITEM DA LISTA
pItem remItem( CLIENTES *cli,pItem p, int id );

// EDITA ITEM
int editPrecoFinal( pItem p, int id, int precoFinal, char user[MAX] );

// MOSTRA INFO DO ITEM
void mostraInfo( pItem p );

// AVANCA TEMPO DOS ITEMS
pItem avancaTempoItems( pItem p, int tempo );

// ITEM E VENDIDO ( TEMPO TERMINOU OU OFERECERAM VALOR COMPRAJA )
pItem vendeItem( CLIENTES *cli, pItem aux, pItem p, int id );

int terminaLista( pItem p );

#endif //ITEMS_LIST_H