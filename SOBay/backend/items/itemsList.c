#include "itemsList.h"
int ultimoId = 0;
char SERVIDOR_CLIENTE_NOTIFICACAO[100];

int listaVazia( pItem p )
{
    if( p == NULL )
        return 1;
    return 0;
}

pItem procUltimoElemento( pItem p )
{
    pItem aux = p;

    while( aux -> prox != NULL )
    {
        aux = aux -> prox;
    }

    return aux;
}

pItem procPorId( pItem p, int id )
{
    pItem aux = p;
    #if DEBUG_SERVIDOR_BUY
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> PROCPORID -> INICIA PROCURAR -> %d\n",id);
        mostraInfo(p);
        // =================================================
    #endif

    if (aux == NULL) {
        #if DEBUG_SERVIDOR_BUY
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> PROCPORID -> NAO ESTA NULL -> %d\n",id);
            // =================================================
        #endif
        return NULL;
    }
    #if DEBUG_SERVIDOR_BUY
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> PROCPORID -> NAO ESTA NULL -> %d\n",id);
        // =================================================
    #endif

    while( aux != NULL ) {
        #if DEBUG_SERVIDOR_BUY
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> PROCPORID -> [%d] ITEM[%d]\n",id,aux->id);
            mostraInfo(aux);
            // =================================================
        #endif
        if (aux->id == id) {
            return aux;
        }
        
        aux = aux -> prox;
            
    }
    #if DEBUG_SERVIDOR_BUY
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> PROCPORID -> NAO ENCONTROU ITEM... -> %d\n",id);
        // =================================================
    #endif

    return aux;
}

int procIdAnterior( pItem p, int id )
{
    pItem aux = p;

    while( aux -> id < id )
    {
        aux = aux -> prox;

        if( aux -> prox == NULL )
            return aux -> id;
    }

    return aux -> id;
}

pItem percorreListaCategoria( pItem p, char *categoria )
{
    pItem aux = p;

    while( aux != NULL ) {
        #if DEBUG_ITEMS_PROCURA_CATEGORIA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEM[%s] -> [%s]\n",aux->nome,aux->categoria);
            // =================================================
        #endif
    	
    	#if DEBUG_ITEMS_PROCURA_CATEGORIA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> PROCURA_CATEGORIA -> PROXIMO NULL?\n");
            // =================================================
        #endif

        if (aux->prox == NULL) {
            #if DEBUG_ITEMS_PROCURA_CATEGORIA
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> PROCURA_CATEGORIA -> SIM\n");
                // =================================================
            #endif
            return NULL;
        } else {
            #if DEBUG_ITEMS_PROCURA_CATEGORIA
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> PROCURA_CATEGORIA -> NAO\n");
                // =================================================
            #endif
            aux = aux->prox;
        }
    }
    return aux;
}

pItem percorreListaVendedor( pItem p, char *vendedor ) {
    pItem aux = p;

    while( aux != NULL ) {
        #if DEBUG_ITEMS_PROCURA_VENDEDOR
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEM[%s] -> [%s]\n",aux->nome,aux->vendedor);
            // =================================================
        #endif
    	
    	if ( strcmp(aux->vendedor,vendedor) == 0 ) {
            return aux;
    	}
        #if DEBUG_ITEMS_PROCURA_VENDEDOR
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> PROCURA_VENDEDOR -> PROXIMO NULL?\n");
            // =================================================
        #endif

        if (aux->prox == NULL) {
            #if DEBUG_ITEMS_PROCURA_VENDEDOR
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> PROCURA_VENDEDOR -> SIM\n");
                // =================================================
            #endif
            return NULL;
        } else {
            #if DEBUG_ITEMS_PROCURA_VENDEDOR
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> PROCURA_VENDEDOR -> NAO\n");
                // =================================================
            #endif
            aux = aux->prox;
        }
        
    }
    return aux;
}

pItem percorreListaValor( pItem p, int valor ) {
    pItem aux = p;

    while( aux != NULL ) {
        #if DEBUG_ITEMS_PROCURA_VALOR
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEM[%s] -> [%d]\n",aux->nome,aux->valorBase);
            // =================================================
        #endif
    	
    	if ( aux->valorBase <= valor ) {
            return aux;
    	}
        aux = aux -> prox;
    }
    return aux;
}

pItem percorreListaPrazo( pItem p, int tempo ) {
    pItem aux = p;

    while( aux != NULL ) {
        #if DEBUG_ITEMS_PROCURA_TEMPO
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEM[%s] -> [%d]\n",aux->nome,aux->tempo);
            // =================================================
        #endif
    	
    	if ( aux->tempo <= tempo ) {
            return aux;
    	}
        aux = aux -> prox;
    }
    return aux;
}

pItem carregaItem( pItem p, int id, char *nome, char *categoria, int valorBase, int compraJa, int tempo, char *vendedor, char *comprador )
{
    #if DEBUG_ITEMS_CARREGA
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> CARREGAITEM -> INICIO -> [%d][%s]\n",id,nome);
        // =================================================
    #endif
    pItem novo, aux = p;

    novo = malloc( sizeof ( item ) );

    // VERIFICA SE FOI BEM ARMAZENADO
    if( novo == NULL )
    {
        printf( "\nErro a criar novo item\n" );
        return 0;
    }

    // ATRIBUI ID SEGUINTE
    novo -> id = id;
    if (ultimoId < novo->id) {   
        ultimoId = novo->id;
    }
    //printf("\nnovo->id=%d\n", novo->id);

    // ATRIBUI NOME
    strcpy( novo -> nome, nome );
    //printf("\nnovo->nome=%s\n", novo->nome);

    // ATRIBUI CATEGORIA
    strcpy( novo -> categoria, categoria );
    //printf("\nnovo->categoria=%s\n", novo->categoria);

    // ATRIBUI VALOR BASE
    novo -> valorBase = valorBase;
    //printf("\nnovo->valorBase=%d\n", novo->valorBase);

    // ATRIBUI VALOR COMPRA JA
    novo -> compraJa = compraJa;
    //printf("\nnovo->compraJa=%d\n", novo->compraJa);

    // ATRIBUI VALOR TEMPO
    novo -> tempo = tempo;
    //printf("\nnovo->tempo=%d\n", novo->tempo);

    // ATRIBUI VALOR VENDEDOR
    strcpy( novo -> vendedor, vendedor );
    //printf("\nnovo->vendedor=%s\n", novo->vendedor);

    // ATRIBUI VALOR COMPRADOR
    strcpy( novo -> comprador, comprador );
    //printf("\nnovo->comprador=%s\n", novo->comprador);

    if( p == NULL )  // a lista está vazia?
    {
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> LISTA VAZIA -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        novo -> prox = NULL;
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> ATRIBUIR NULL -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        p = novo;
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> ATRIBUIR PARA PRIMEIRO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
    }
    else if( novo -> id < aux -> id ){ // é o menor?
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> E O MENOR -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        p = novo;
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> COLOCA NO INICIO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        novo->prox = aux;
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> ATRIBUI PROXIMO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        aux->prox = novo;
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> ATRIBUI AO PROXIMO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        aux->ant = novo;
        #if DEBUG_ITEMS_CARREGA
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> CARREGA -> PRIMEIRO ITEM -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
    } else {
        int avanca = 0;
        while ( aux != NULL ) {
            if (aux->id >= novo->id || aux->prox == NULL) {
                #if DEBUG_ITEMS_CARREGA
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> CARREGA -> PROCURA ANTERIOR -> [%d][%s]\n",aux->id,aux->nome);
                    // =================================================
                #endif
                if (aux->id == novo->id) {
                    ++ultimoId;
                    novo->id = ultimoId;
                }
                if (aux->prox != NULL) {
                    novo->prox = aux->prox;
                    #if DEBUG_ITEMS_CARREGA
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> CARREGA -> ATRIBUI PROXIMO AO NOVO -> [%d][%s] -> [%d][%s]\n",novo->id,novo->nome,aux->prox->id,aux->prox->nome);
                        // =================================================
                    #endif
                    novo->prox->ant = novo;
                    #if DEBUG_ITEMS_CARREGA
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> CARREGA -> ATRIBUI NOVO AO PROXIMO -> [%d][%s] -> [%d][%s]\n",aux->prox->id,aux->prox->nome,novo->id,novo->nome);
                        // =================================================
                    #endif
                } else {
                    novo->prox = NULL;
                    #if DEBUG_ITEMS_CARREGA
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> CARREGA -> ATRIBUI PROXIMO[NULL] AO NOVO -> [%d][%s] -> NULL\n",novo->id,novo->nome);
                        // =================================================
                    #endif
                }
                novo->ant = aux;
                #if DEBUG_ITEMS_CARREGA
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> CARREGA -> ATRIBUI ANTERIOR AO NOVO -> [%d][%s] -> [%d][%s]\n",aux->id,aux->nome,novo->id,novo->nome);
                    // =================================================
                #endif
                aux->prox = novo;
                #if DEBUG_ITEMS_CARREGA
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> CARREGA -> ATRIBUI NOVO AO ANTERIOR -> [%d][%s] -> [%d][%s]\n",aux->id,aux->nome,novo->id,novo->nome);
                    // =================================================
                #endif
                avanca = 1;
            } else {
                aux = aux -> prox;
            }
            if (avanca == 1)
                break;
        }
        #if DEBUG_ITEMS_CARREGA
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> CARREGA -> ULTIMO -> [%d][%s]\n",aux->id,aux->nome);
                // =================================================
        #endif
    }

    //p = insereItem( p, novo );
    #if DEBUG_ITEMS_CARREGA
        // ==================== DEBUG ======================
        mostraInfo(p);
        // =================================================
    #endif
return p;
}

pItem addItem( CLIENTES *cli,pItem p, int tempoServer, char *nome, char *categoria, int valorBase, int compraJa, int tempo, char *vendedor, char *comprador )
{
    #if DEBUG_ITEMS_ADD
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> CARREGAITEM -> INICIO -> [%s]\n",nome);
        // =================================================
    #endif
    pItem novo, aux = p;

    novo = malloc( sizeof ( item ) );

    // VERIFICA SE FOI BEM ARMAZENADO
    if( novo == NULL )
    {
        printf( "\nErro a criar novo item\n" );
        return 0;
    }

    // ATRIBUI ID SEGUINTE
    ++ultimoId;
    novo -> id = ultimoId;
    //printf("\nnovo->id=%d\n", novo->id);

    // ATRIBUI NOME
    strcpy( novo -> nome, nome );
    //printf("\nnovo->nome=%s\n", novo->nome);

    // ATRIBUI CATEGORIA
    strcpy( novo -> categoria, categoria );
    //printf("\nnovo->categoria=%s\n", novo->categoria);

    // ATRIBUI VALOR BASE
    novo -> valorBase = valorBase;
    //printf("\nnovo->valorBase=%d\n", novo->valorBase);

    // ATRIBUI VALOR COMPRA JA
    novo -> compraJa = compraJa;
    //printf("\nnovo->compraJa=%d\n", novo->compraJa);

    // ATRIBUI VALOR TEMPO
    novo->tempo = tempo + tempoServer;
    #if DEBUG_ITEMS_ADD
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> ADD -> TEMPOREAL[%d] = TEMPO[%d] + TEMPOSERVER[%d]\n",novo -> tempo,tempo,tempoServer);
        // =================================================
    #endif
    //printf("\nnovo->tempo=%d\n", novo->tempo);

    // ATRIBUI VALOR VENDEDOR
    strcpy( novo -> vendedor, vendedor );
    //printf("\nnovo->vendedor=%s\n", novo->vendedor);

    // ATRIBUI VALOR COMPRADOR
    strcpy( novo -> comprador, comprador );
    //printf("\nnovo->comprador=%s\n", novo->comprador);

    if( p == NULL )  // a lista está vazia?
    {
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> LISTA VAZIA -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        novo -> prox = NULL;
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> ATRIBUIR NULL -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        p = novo;
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> ATRIBUIR PARA PRIMEIRO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
    }
    else if( novo -> id < aux -> id ){ // é o menor?
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> E O MENOR -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        p = novo;
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> COLOCA NO INICIO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        novo->prox = aux;
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> ATRIBUI PROXIMO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        aux->prox = novo;
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> ATRIBUI AO PROXIMO -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
        aux->ant = novo;
        #if DEBUG_ITEMS_ADD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ADD -> PRIMEIRO ITEM -> [%d][%s]\n",novo->id,novo->nome);
            // =================================================
        #endif
    } else {
        int avanca = 0;
        while ( aux != NULL ) {
            if (aux->id >= novo->id || aux->prox == NULL) {
                #if DEBUG_ITEMS_ADD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> ADD -> PROCURA ANTERIOR -> [%d][%s]\n",aux->id,aux->nome);
                    // =================================================
                #endif
                if (aux->id == novo->id) {
                    ++ultimoId;
                    novo->id = ultimoId;
                }
                if (aux->prox != NULL) {
                    novo->prox = aux->prox;
                    #if DEBUG_ITEMS_ADD
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> ADD -> ATRIBUI PROXIMO AO NOVO -> [%d][%s] -> [%d][%s]\n",novo->id,novo->nome,aux->prox->id,aux->prox->nome);
                        // =================================================
                    #endif
                    novo->prox->ant = novo;
                    #if DEBUG_ITEMS_ADD
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> ADD -> ATRIBUI NOVO AO PROXIMO -> [%d][%s] -> [%d][%s]\n",aux->prox->id,aux->prox->nome,novo->id,novo->nome);
                        // =================================================
                    #endif
                } else {
                    novo->prox = NULL;
                    #if DEBUG_ITEMS_ADD
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> ADD -> ATRIBUI PROXIMO[NULL] AO NOVO -> [%d][%s] -> NULL\n",novo->id,novo->nome);
                        // =================================================
                    #endif
                }
                novo->ant = aux;
                #if DEBUG_ITEMS_ADD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> ADD -> ATRIBUI ANTERIOR AO NOVO -> [%d][%s] -> [%d][%s]\n",aux->id,aux->nome,novo->id,novo->nome);
                    // =================================================
                #endif
                aux->prox = novo;
                #if DEBUG_ITEMS_ADD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> ADD -> ATRIBUI NOVO AO ANTERIOR -> [%d][%s] -> [%d][%s]\n",aux->id,aux->nome,novo->id,novo->nome);
                    // =================================================
                #endif
                avanca = 1;
            } else {
                aux = aux -> prox;
            }
            if (avanca == 1)
                break;
        }
        #if DEBUG_ITEMS_ADD
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> ADD -> ULTIMO -> [%d][%s]\n",aux->id,aux->nome);
                // =================================================
        #endif
    }

    //p = insereItem( p, novo );
    #if DEBUG_ITEMS_ADD
        // ==================== DEBUG ======================
        mostraInfo(p);
        // =================================================
    #endif

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key,SERVER_KEY);
    #if DEBUG_ITEMS_ADD
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> ADD -> NOTIFICAO -> SERVER_KEY\n");
        // =================================================
    #endif
    for (size_t i = 0; i < MAXCLI; i++) {
        if (cli->clientes[i].ativo == 1) {
            sprintf( SERVIDOR_CLIENTE_NOTIFICACAO, CLIENT_FIFO, cli->clientes[i].pid );
            int fdNotificacao = open(SERVIDOR_CLIENTE_NOTIFICACAO, O_WRONLY);
            #if DEBUG_ITEMS_ADD
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> ADD -> NOTIFICAO -> DEFINE MENSAGEM\n");
                // =================================================
            #endif
            sprintf(ppr.msg.umarg.string,"\n%s %d %s %s %s %d %d %d\n","[NOTIFICACAO] NOVO-> ",novo->id, novo->nome, novo->categoria,novo->vendedor,novo->valorBase,novo->compraJa,novo->tempo);
            #if DEBUG_ITEMS_ADD
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> ADD -> NOTIFICAO -> MENSAGEM DEFINIDA\n");
                // =================================================
            #endif
            if (fdNotificacao != -1) {
                if( write(fdNotificacao,&ppr,sizeof (PREPARACAO)) == -1 ){
                    close(fdNotificacao);
                }
            }
        }
    }

return p;
}

pItem remItem( CLIENTES *cli,pItem p, int id )
{
    pItem temp = p, ant;

    #if DEBUG_ITEMS_REMOVE
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> REMOVER ITEM...\n");
        // =================================================
    #endif

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->id == id) {
        p = temp->prox; // Changed head
        if (temp->prox != NULL)
            temp->prox->ant = p;
        free(temp); // free old head
        #if DEBUG_ITEMS_REMOVE
            // ==================== DEBUG ======================
            mostraInfo(p);
            // =================================================
        #endif
        
        return p;
    }

    #if DEBUG_ITEMS_REMOVE
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> REMOVER ITEM1...\n");
        // =================================================
    #endif
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->id != id) {
        ant = temp;
        temp = temp->prox;
    }

    #if DEBUG_ITEMS_REMOVE
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> REMOVER ITEM2...\n");
        // =================================================
    #endif
 
    // If key was not present in linked list
    if (temp == NULL)
        return p;

    #if DEBUG_ITEMS_REMOVE
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> REMOVER ITEM3...\n");
        // =================================================
    #endif
 
    // Unlink the node from linked list
    ant->prox = temp->prox;

    #if DEBUG_ITEMS_REMOVE
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> REMOVER ITEM4...\n");
        // =================================================
    #endif

    if (temp->prox != NULL)
        temp->prox->ant = ant;

    #if DEBUG_ITEMS_REMOVE
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> REMOVER ITEM5...\n");
        // =================================================
    #endif
 
    free(temp); // Free memory

    #if DEBUG_ITEMS_REMOVE
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> REMOVER ITEM6...\n");
        // =================================================
    #endif

    /* aux = procPorId( p, id );

    if( aux == NULL )
        return 0;
    
    mostraInfo(aux);
    printf("\nTESTE1\n");

    pItem temp = aux->ant;

    temp->prox = aux->prox;

    if( aux->prox == NULL )
        temp->prox = NULL;
    else
        aux->prox->ant = aux->ant; */

    #if DEBUG_ITEMS_TEMPO_AVANCA
        // ==================== DEBUG ======================
        mostraInfo(p);
        // =================================================
    #endif

    return p;

}

int editPrecoFinal( pItem p, int id, int precoFinal,char user[MAX] ) {
    int valorPago = precoFinal;
    pItem aux = procPorId( p, id );
    if( aux == NULL ) {
        #if DEBUG_SERVIDOR_BUY
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> SELL -> NAO EXISTE -> %d\n",id);
            // =================================================
        #endif
        return 0;
    }
        
    #if DEBUG_SERVIDOR_BUY
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> SELL -> ENCONTROU -> %d\n",id);
        // =================================================
    #endif
    #if DEBUG_SERVIDOR_BUY
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> %d %d %d %d %d %s\n",id,aux->id,aux->valorBase,aux->compraJa,precoFinal,aux->nome);
        // =================================================
    #endif
    if (aux->valorBase < aux->compraJa && aux->valorBase < precoFinal) {
        #if DEBUG_SERVIDOR_BUY
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> SELL -> PRECO VALIDO -> %d\n",id);
            // =================================================
        #endif
        if (aux->compraJa <= precoFinal) {
            aux->valorBase = aux->compraJa;
        } else {
            aux->valorBase = precoFinal;
        }
        strcpy(aux->comprador,user);
        return 1;
    }
    #if DEBUG_SERVIDOR_BUY
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> SELL -> PRECO INVALIDO -> %d\n",id);
        // =================================================
    #endif
    return 0;
}

void mostraInfo( pItem p )
{
    pItem aux = p;
    if (aux == NULL)
    {
        printf("\nSem Itens\n");
    }
    
    while ( aux != NULL )
    {
        #if DEBUG_ITEMS_MOSTRA_INFO
            // ==================== DEBUG ======================
                if (aux->prox == NULL) {
                    printf("\n[DEBUG] -> MOSTRA_INFO -> FIM ITEMS -> ULTIMO[%d][%s]\n",aux->id,aux->nome);
                }
            // =================================================
        #endif
        printf( "\n%d %s %s %d %d %d %s %s", aux->id, aux->nome, aux->categoria, aux->valorBase, aux->compraJa, aux->tempo, aux->vendedor, aux->comprador );
        aux = aux->prox;
    }
    

    free(aux);
    printf( "\n" );

}

pItem avancaTempoItems( pItem p, int tempoServer )
{
    pItem aux = p;
    int avanca = 0;

    while( aux != NULL )
    {
        if( aux -> prox == NULL )
            avanca = 1;

        if (DEBUG_ITEMS_TEMPO_AVANCA)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> AVANCATEMPOITEMS -> ITEM[%s] -> [%d]\n",aux->nome,aux->tempo);
            // =================================================
        }
    	
    	if ( aux -> tempo == tempoServer || aux->valorBase == aux->compraJa )
    	{
            return aux;
    	}

        if (avanca)
            break;

        aux = aux -> prox;
        
    }

    return aux;
}

pItem vendeItem( CLIENTES *cli,pItem aux, pItem p, int id )
{
    PREPARACAO ppr;
    strcpy(ppr.key,SERVER_KEY);
    ppr.pid = getpid();
    for (size_t i = 0; i < MAXCLI; i++) {
        if (cli->clientes[i].ativo == 1) {
            sprintf( SERVIDOR_CLIENTE_NOTIFICACAO, CLIENT_FIFO, cli->clientes[i].pid );
            int fdNotificacao = open(SERVIDOR_CLIENTE_NOTIFICACAO, O_WRONLY);
            if (strcmp(aux->comprador,"-") == 0) {
                sprintf(ppr.msg.umarg.string,"\n%s %d %s %s %s %d %d\n","[NOTIFICACAO] NAO VENDIDO-> ",aux->id, aux->nome, aux->categoria,aux->vendedor,aux->valorBase,aux->compraJa);
            } else {
                sprintf(ppr.msg.umarg.string,"\n%s %d %s %s %s %d %s\n","[NOTIFICACAO] VENDIDO-> ",aux->id, aux->nome, aux->categoria,aux->vendedor,aux->valorBase,aux->comprador);
            }
            if (fdNotificacao != -1) {
                if( write(fdNotificacao,&ppr,sizeof (PREPARACAO)) == -1 ){
                    close(fdNotificacao);
                }
            }
        }
    }
    if (DEBUG_ITEMS_TEMPO_AVANCA)
        {
            if (strcmp(aux->comprador,"-") == 0) {
                printf("\n%s [%d] de %s nao foi vendido\n",aux->nome,aux->id,aux->vendedor);
            } else {
                printf( "\n%s [%d] vendido por %d euros\n", aux -> nome, aux -> id, aux -> valorBase );
                printf( "%s -> %s\n", aux -> vendedor, aux -> comprador );
            }
        }
    

    if (aux->prox == NULL){
        p = remItem( cli,p, id );
        if (DEBUG_ITEMS_TEMPO_AVANCA)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEM REMOVIDO COM SUCESSO\n");
            // =================================================
        }
        return NULL;
    } else {
        p = remItem( cli,p, id );
        if (DEBUG_ITEMS_TEMPO_AVANCA)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEM REMOVIDO COM SUCESSO\n");
            // =================================================
        }
        return aux->prox;
    }
}

int terminaLista( pItem p )
{
    pItem aux = p;
    FILE *fp;

    fp = fopen( getenv( "FITEMS" ), "w" );

    if( fp == NULL )
    {
        fprintf( stderr, "\nErro ao carregar ficheiro %s\n", getenv( "FITEMS" ) );
        return 0;
    }

    if( !listaVazia( aux ) )
    {

        do
        {
            p = aux->prox;
            aux -> ant = NULL;
            aux -> prox = NULL;
            fprintf( fp, "%d %s %s %d %d %d %s %s\n",
                                        aux->id, aux->nome, aux->categoria,
                                        aux->valorBase, aux->compraJa,
                                        aux->tempo, aux->vendedor, aux->comprador );
            free( aux );
            if( p != NULL )
                aux = p;
        } while( p != NULL );

        fclose( fp );
        free(p);
    }
    return 1;
}
