
#include "backend.h"


GERAL geral;
int heartbeats;

void terminaServerInvalido() {
    exit(-1);
}

void acorda(int s, siginfo_t *info, void *c) {}

void sair(int sign, siginfo_t *info, void *s)
{
    geral.stop = 1;
}

void verificaTempo() {
    if (access("backend/tempo.txt", F_OK) != 0) {
        geral.tempo = 0;
    } else {
        FILE *fp;
        fp = fopen("backend/tempo.txt", "r");
        if (fp == NULL) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CARREGAR FICHEIRO [tempo.txt]\n");
            // =================================================
            geral.tempo = 0;
        } else {
            while (fscanf(fp,"%d",&geral.tempo) == 1) {
            }
        }
        fclose(fp);
    }
}

void guardaTempo() {
    FILE *fp;
    fp = fopen("backend/tempo.txt", "w");
        if (fp == NULL) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO ABRIR FICHEIRO [tempo.txt]\n");
            // =================================================
        } else {
            fprintf(fp,"%d",geral.tempo);
        }
    fclose(fp);
}

int leFichItems() {
    // ==================== DEBUG ======================
    if (DEBUG_ITEMS_LE_FICHA_INICIO)
        printf("\n[DEBUG] -> LER FICHEIRO DE ITENS INICIO\n");
    // =================================================

    int id;
    char nome[MAXPAL];
    char categoria[MAXPAL];
    int valorBase;
    int compraJa;
    int tempo;
    char vendedor[MAXPAL];
    char comprador[MAXPAL];

    if (access(getenv("FITEMS"), F_OK) != 0)
    {
        printf("\n[SYSTEM] -> SEM HISTORICO\n");
        return ERRO_SEM_ITEMS;
    }
    else
    {

        FILE *fp;
        fp = fopen(getenv("FITEMS"), "r");

        if (fp == NULL)
        {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CARREGAR FICHEIRO [%s]\n", getenv("FITEMS"));
            // =================================================
            return ERRO_CARREGAR_FICHEIRO_ITEMS;
        }
        

        while (fscanf(fp, "%d %s %s %d %d %d %s %s", &id, nome, categoria, &valorBase, &compraJa, &tempo, vendedor, comprador) == 8)
        {
            geral.itemsList = carregaItem(geral.itemsList, id, nome, categoria, valorBase, compraJa, tempo, vendedor, comprador);
            #if DEBUG_ITEMS_MOSTRA_LIDOS
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> CICLO -> ITEMS LIDOS:\n");
                mostraInfo(geral.itemsList);
                printf("\n[DEBUG] -> CICLO -> FIM LISTA:\n");
                // =================================================
            #endif
            
        }
        #if DEBUG_ITEMS_MOSTRA_LIDOS
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> FIM CICLO\n");
            // =================================================
        #endif

        fclose(fp);

        #if DEBUG_ITEMS_MOSTRA_LIDOS
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEMS LIDOS:\n");
            mostraInfo(geral.itemsList);
            printf("\n[DEBUG] -> FIM LISTA:\n");
            // =================================================
        #endif

        // ==================== DEBUG ======================
        if (DEBUG_ITEMS_LE_FICHA_FIM)
            printf("\n[DEBUG] -> LER FICHEIRO DE ITENS FIM\n");
        // =================================================

        return SUCESSO;
    }
}

void help()
{
    printf("\nComandos Disponiveis: \n\n");
    printf("--Listar Usuários: \n\t users\n\n");
    printf("--Listar Items: \n\t list\n\n");
    printf("--Listar Promotores: \n\t prom\n\n");
    printf("--Update dos Promotores: \n\t reprom\n\n");
    printf("--Cancela Promotor: \n\t cancel <nome-do-executavel-do-promotor>\n\n");
    printf("--Kick User: \n\t kick <username>\n\n");
    printf("--Exit: \n\t close\n\n");
}

void listaUsers(CLIENTES *clientes) { //  users
    pthread_mutex_lock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] RECEBEHEARTBEATS -> -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    printf("\n\n\n\n-> LISTA USERS\n");
    int count = 0;
    for (size_t i = 0; i < MAXCLI; i++) {
        if (clientes->clientes[i].ativo == 1)  {
            printf("\n -> %s [%d]",clientes->clientes[i].username,clientes->clientes[i].pid);
            ++count;
            if (count >= clientes->nClientes)
                break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] RECEBEHEARTBEATS -> -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    
}

void listaItems()
{ //  list
    printf("\n\n\n\n-> LISTA ITEMS\n");
    mostraInfo(geral.itemsList);
}

void listaProms(THREAD_PROMOTORES *prom)
{ // prom

    pthread_mutex_lock(geral.m);
    printf("\n\n\n\n-> LISTA PROMOTORES\n");
    for (int i = 0; i < MAXPROM; i++)
    {
        if (prom[i].promotor.ativo == 1)
            printf("-> %s\n", prom[i].promotor.nome);
    }
    pthread_mutex_unlock(geral.m);
}

void UpdateProms(COMANDOS *pComs)
{ // reprom
    printf("\n\n\n\n-> UPDATE PROMOTORES\n");
    if (pthread_create(&pComs->tPromotoresControl, NULL, &recebePromotores, &pComs->prom) != 0) {}

    if (pthread_join(pComs->tPromotoresControl, NULL) != 0) {}

    printf("\nPromotores Atualizados\n");
}

void cancelProms(char comando[MAXPAL], char *ptr, THREAD_PROMOTORES *prom,CLIENTES *cli)
{ //  cancel <nome-do-executavel-do-promotor>
    char cancelProm[20];

    if (ptr != NULL)
    {
        printf("\n\n\n\n-> CANCELA PROMOTOR\n");
        strcpy(cancelProm, ptr);
        pthread_mutex_lock(geral.m);
        for (int i = 0; i < MAXPROM; i++)
        {
            if (strcmp(prom[i].promotor.nome, cancelProm) == 0)
            {
                prom[i].promotor.stop = 1;

                pthread_mutex_unlock(geral.m);

                pthread_kill(prom[i].tPromotor, SIGUSR1);

                if (pthread_join(prom[i].tPromotor, NULL) != 0) {

                }

                pthread_mutex_lock(geral.m);

                resetPromotor(&prom[i].promotor);
            }
        }
        pthread_mutex_unlock(geral.m);

        printf("\nPromotor Cancelado:%s\n", cancelProm);
        listaProms(prom);
    }
    else
        printf("\nComando Invalido\n");
}

void kickUser(char comando[MAXPAL], char *ptr,CLIENTES *cli) //  kick <username>
{
    char user[20];
    if (ptr != NULL)
    {
        printf("\n\n\n\n-> KICK USER\n");
        strcpy(user, ptr);
        PREPARACAO response;
        strcpy(response.key,SERVER_KEY);
        response.pid = getpid();

        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        printf("\nUser:%s\n", user);
        for (size_t i = 0; i < MAXCLI; i++) {
            if (strcmp(cli->clientes[i].username,user) == 0 && cli->clientes[i].ativo == 1) {
                terminaCliente(&cli->clientes[i]);
                break;
            }
        }
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
    }
    else
        printf("\nComando Invalido\n");
}

void *recebeComando(void *comandos)
{
    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> INICIO COMANDOS\n");
        // =================================================
    }

    COMANDOS *pComandos = (COMANDOS *)comandos;

    char comando[MAXPAL];
    char delim[] = " ";
    char *ptr;

    geral.cli = &pComandos->cliente;

    for (int i = 0; i < MAXPROM; i++)
    {
        resetPromotor(&pComandos->prom[i].promotor);
        if (DEBUG_COMANDOS) {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> COMANDOS -> ATIVO = %d\n", pComandos->prom[i].promotor.ativo);
            // =================================================
        }
    }

    UpdateProms(pComandos);
    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> PROMOTORES CRIADOS\n");
        // =================================================
    }

    pthread_mutex_lock(geral.m);
    if (DEBUG_MUTEXES)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> MUTEX GERAL.M LOCK\n");
        // =================================================
    }

    for (size_t i = 0; i < MAXCLI; i++) {
        resetCliente(&pComandos->cliente.clientes[i]);
    }

    pthread_mutex_unlock(geral.m);
    if (DEBUG_MUTEXES)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    }

    
    if (pthread_create (&pComandos->tCLientesHeartbeats,NULL,&recebeHeartbeats,&pComandos->cliente)!=0) {}
    
    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> HEARTBEATS CRIADOS\n");
        // =================================================
    }

    if (pthread_create (&pComandos->tClientes,NULL,&recebeClientes,&pComandos->cliente)!=0) {}

    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> CLIENTES CRIADOS\n");
        // =================================================
    }

    do
    {
        printf("\n\n\n\n-> help para mais informacao\n\n\nComando:");
        fgets(comando, sizeof(comando), stdin);

        if (geral.stop == 0) {
            // ==================== DEBUG ======================
            if  (DEBUG_GERAL_STOP)
                printf("\n[DEBUG] -> COMANDOS -> GERAL.STOP1 = %d\n",geral.stop);
            // =================================================


            comando[strcspn(comando, "\n")] = 0;

            fflush(stdin);

            if (strcmp(comando, "help") == 0)
                help();
            else if (strcmp(comando, "users") == 0)
                listaUsers(&pComandos->cliente);
            else if (strcmp(comando, "list") == 0)
                listaItems();
            else if (strcmp(comando, "prom") == 0)
                listaProms(pComandos->prom);
            else if (strcmp(comando, "reprom") == 0)
                UpdateProms(pComandos);
            else if (strcmp(comando, "close") != 0)
            {
                ptr = strtok(comando, delim);

                if (strcmp(comando, "cancel") == 0)
                {
                    ptr = strtok(NULL, delim);
                    cancelProms(comando, ptr, pComandos->prom,&pComandos->cliente);
                }
                else if (strcmp(comando, "kick") == 0)
                {
                    ptr = strtok(NULL, delim);
                    kickUser(comando, ptr,&pComandos->cliente);
                    if (DEBUG_COMANDOS) {
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> COMANDOS -> FIM KICK\n");
                        // =================================================
                    }
                }
                else
                    printf("\nComando Invalido!!\n");
            }
        }
        if (DEBUG_COMANDOS) {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> COMANDOS -> FIM COMANDO\n");
            // =================================================
        }
    } while (strcmp(comando, "close") != 0 && geral.stop == 0);

    geral.stop = 1;
    // ==================== DEBUG ======================
    if  (DEBUG_GERAL_STOP)
        printf("\n[DEBUG] -> COMANDOS -> GERAL.STOP2 = %d\n",geral.stop);
    // =================================================

    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> CLIENTES VAO SER LIMPOS\n");
        // =================================================
    }

    pthread_kill(pComandos->tCLientesHeartbeats, SIGUSR1);
        if (pthread_join(pComandos->tCLientesHeartbeats, NULL) != 0)

    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> THREAD HEARTBEATS RECEBIDA\n");
        // =================================================
    }

    if (pComandos->cliente.nClientes <= 0) {
        for (size_t i = 0; i < MAXCLI; i++) {
            if (pComandos->cliente.clientes[i].ativo == 1) {
                pthread_mutex_lock(geral.m);
                if (DEBUG_MUTEXES)
                {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> COMANDOS -> MUTEX GERAL.M LOCK\n");
                    // =================================================
                }
                terminaCliente(&pComandos->cliente.clientes[i]);
                pComandos->cliente.nClientes -= 1;
                pthread_mutex_unlock(geral.m);
                if (DEBUG_MUTEXES)
                {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> COMANDOS -> MUTEX GERAL.M UNLOCK\n");
                    // =================================================
                }
            }
        }
    }

    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> CLIENTES LIMPOS\n");
        // =================================================
    }

    for (int i = 0; i < MAXPROM; i++)
    {
        if (pComandos->prom[i].promotor.ativo == 1)
        {
            pComandos->prom[i].promotor.stop = 1;
            pthread_kill(pComandos->prom[i].tPromotor, SIGUSR1);
            if (pthread_join(pComandos->prom[i].tPromotor, NULL) != 0) {}
        }
        pthread_mutex_lock(geral.m);
        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> COMANDOS -> MUTEX GERAL.M LOCK\n");
            // =================================================
        }

        resetPromotor(&pComandos->prom[i].promotor);

        pthread_mutex_unlock(geral.m);
        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> COMANDOS -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        }
    }

    terminaLista(geral.itemsList);
    saveUsersFile(getenv("FUSERS"));
    guardaTempo();

    if (DEBUG_COMANDOS) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIM COMANDOS\n");
        // =================================================
    }
}

void login(CLIENTES *coms, PREPARACAO ppr) {
    int existente = 0;
    sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
    int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
    if (fdEnvio == -1) {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
        //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
        // =================================================
    }
    PREPARACAO response;
    strcpy(response.key,SERVER_KEY);
    switch (isUserValid(ppr.msg.login.userName, ppr.msg.login.password)) {
        case 0:
            #if DEBUG_CLIENTES_LOGIN
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> LOGIN -> USER[%s] PASSWORD[%s] INCORRETO\n", ppr.msg.login.userName, ppr.msg.login.password);
                // =================================================
            #endif
            response.msg.confirmacao = 0;
            break;
        case 1:
            #if DEBUG_CLIENTES_LOGIN
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> LOGIN -> USER[%s] PASSWORD[%s] VALIDO\n", ppr.msg.login.userName, ppr.msg.login.password);
                // =================================================
            #endif

            if (coms->nClientes < MAXCLI) {
                #if DEBUG_MUTEXES
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
                    // =================================================
                #endif
                pthread_mutex_lock(geral.m);

                for (size_t i = 0; i < MAXCLI; i++) {
                    if (coms->clientes[i].ativo == 1 && strcmp(ppr.msg.login.userName,coms->clientes[i].username) == 0) {
                        existente = 1;
                        break;
                    }
                }

                if (!existente) {
                    for (size_t i = 0; i < MAXCLI; i++) {
                        if (coms->clientes[i].ativo == 0) {
                            strcpy(coms->clientes[i].username, ppr.msg.login.userName);
                            coms->clientes[i].tempo = heartbeats + 2;
                            coms->clientes[i].ativo = 1;
                            coms->clientes[i].pid = ppr.pid;
                            coms->nClientes += 1;
                            strcpy(response.key, SERVER_KEY);
                            response.pid = getpid();
                            response.msg.confirmacao = 1;
                            #if DEBUG_CLIENTES_LOGIN
                                // ==================== DEBUG ======================
                                printf("\n[DEBUG] -> LOGIN -> USER[%s] PASSWORD[%s] TESTE\n", coms->clientes[i].username, ppr.msg.login.password);
                                // =================================================
                            #endif
                            break;
                        }
                    }
                }
                
                pthread_mutex_unlock(geral.m);
                #if DEBUG_MUTEXES
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
                    // =================================================
                #endif
            } else {
                response.msg.confirmacao = 0;
            }

            #if DEBUG_CLIENTES_LOGIN
                if (response.msg.confirmacao == 1) {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LOGIN -> USER[%s] PASSWORD[%s] VALIDO\n",ppr.msg.login.userName, ppr.msg.login.password);
                    // =================================================
                } else {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LOGIN -> USER[%s] PASSWORD[%s] VALIDO MAS NUMERO MAXIMO ATINGIDO\n", ppr.msg.login.userName, ppr.msg.login.password);
                    // =================================================
                }
            #endif
            break;
        default:
            #if DEBUG_CLIENTES_LOGIN
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> LOGIN -> USER[%s] PASSWORD[%s] ERRO A OBTER VERIFICACAO\n", ppr.msg.login.userName, ppr.msg.login.password);
                // =================================================
            #endif
            response.msg.confirmacao = 0;
            break;

    }

    if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
        perror("Erro a Enviar dados login");
    }

    #if DEBUG_CLIENTES_THREAD
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> MENSAGEM ENVIADA AO FIFO[%s]\n", SERVIDOR_CLIENTE_FINAL);
        // =================================================
    #endif
    close(fdEnvio);
}

void *recebeHeartbeats(void *clientes) {
    #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> RECEBEHEARTBEATS -> INICIO HEARTBEATS\n");
        // =================================================
    #endif

    CLIENTES *pClientes = (CLIENTES *)clientes;

    if(mkfifo(SERVER_HEARTBEATS,0666) == -1)
    {
        if(errno == EEXIST)
        {
            printf("FIFO ja existe\n");
        }
        printf("Erro abrir FIFO\n");
        return 1;
    }
    #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> RECEBEHEARTBEATS -> CRIOU FIFO SERVIDOR_HEARTBEATS [%s]\n",SERVER_HEARTBEATS);
        // =================================================
    #endif
    
    int fdRecebeHB = open(SERVER_HEARTBEATS, O_RDWR);
    if (fdRecebeHB == -1) {
        printf("Erro a abrir o Servidor");
        return 1;
    }
    #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> RECEBEHEARTBEATS -> ABRIU FIFO SERVIDOR_HEARTBEATS [%s]\n",SERVER_HEARTBEATS);
        // =================================================
    #endif

    do {
        PREPARACAO ppr;
        ppr.tipo = HEARTBEAT;
        ppr.pid = getpid();
        ppr.msg.confirmacao = 1;
        if( read(fdRecebeHB,&ppr,sizeof(ppr)) == -1 ){
            geral.stop = 1;
            //exit(1);
        }
        #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> RECEBEHEARTBEATS -> RECEBEU HEARTBEAT DE [%d]\n",ppr.pid);
            // =================================================
        #endif

        if (geral.stop == 0) {
            if (ppr.tipo == HEARTBEAT) {
                pthread_mutex_lock(geral.m);
                #if DEBUG_MUTEXES
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] RECEBEHEARTBEATS -> -> MUTEX GERAL.M LOCK\n");
                    // =================================================
                #endif

                for (size_t i = 0; i < MAXCLI; i++) {
                    if (pClientes->clientes[i].ativo == 1 && pClientes->clientes[i].pid == ppr.pid)
                    {
                        #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] RECEBEHEARTBEATS -> -> [%s] -> TEMPO = [%d]\n",pClientes->clientes[i].username,pClientes->clientes[i].tempo);
                            // =================================================
                        #endif
                        pClientes->clientes[i].tempo = heartbeats;
                        #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] RECEBEHEARTBEATS -> -> [%s] -> TEMPO = [%d]\n",pClientes->clientes[i].username,pClientes->clientes[i].tempo);
                            // =================================================
                        #endif
                    }
                }

                pthread_mutex_unlock(geral.m);
                #if DEBUG_MUTEXES
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> RECEBEHEARTBEATS -> MUTEX GERAL.M UNLOCK\n");
                    // =================================================
                #endif
            }
        }

    } while (geral.stop == 0);

    unlink(SERVER_HEARTBEATS);
    #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> RECEBEHEARTBEATS -> FIFO [%s] TERMINADO\n", SERVER_HEARTBEATS);
        // =================================================
    #endif

    #if DEBUG_SERVIDOR_RECEBE_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> RECEBEHEARTBEATS -> FIM HEARTBEATS\n");
        // =================================================
    #endif
}

void list(CLIENTES *coms, PREPARACAO ppr){
    int controlo = 0;
    pthread_mutex_lock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LIST -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LIST -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1) {
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LIST -> PREPARA ENVIO\n");
            // =================================================
        #endif
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        if (fdEnvio == -1) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
            // =================================================
        }
        PREPARACAO response;
        strcpy(response.key,SERVER_KEY);
        response.pid = getpid();
        pItem aux = geral.itemsList;
        char *buffer;
        int avanca = 0;
        sprintf(response.msg.umarg.string,"%s %s","\n[SERVIDOR]","INICIO LISTA DE ITENS\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LIST -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        do {
            response.pid = getpid();
            if (aux == NULL) {
                break;
            }
            if (aux != NULL) {
                sprintf(response.msg.umarg.string,"%d %s %s %d %d %d %s %s\n",aux->id,aux->nome,aux->categoria,aux->valorBase,aux->compraJa,aux->tempo,aux->vendedor,aux->comprador);
                if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
                    perror("Erro a Enviar dados login");
                    exit(1);
                }
                #if DEBUG_SERVIDOR_ENVIA_ITEMS
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> ITEM ENVIADO\n");
                    // =================================================
                #endif
            }
            aux = aux->prox;

        } while(aux != NULL);
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        sprintf(response.msg.umarg.string,"%s %s","\n[SERVIDOR]","FIM LISTA DE ITENS\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        close(fdEnvio);
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LIST -> FECHA FIFO CLIENTE\n");
            // =================================================
        #endif
        //free(aux);
    }
}

void licat(CLIENTES *coms, PREPARACAO ppr){
    int controlo = 0;
    pthread_mutex_lock(geral.m);
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1){
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        if (fdEnvio == -1) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
            // =================================================
        }
        PREPARACAO response;
        strcpy(response.key,SERVER_KEY);
        response.pid = getpid();
        pItem aux = geral.itemsList;
        char *buffer;
        int avanca = 0;
        #if DEBUG_SERVIDOR_LICAT
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LICAT -> CATEGORIA[%s]\n",ppr.msg.umarg.string);
            // =================================================
        #endif
        sprintf(response.msg.umarg.string,"%s %s %s %s","[SERVIDOR]","INICIO LISTA DE ITENS DA CATEGORIA [",ppr.msg.umarg.string,"]\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        do {
            response.pid = getpid();
            aux = percorreListaCategoria(aux,ppr.msg.umarg.string);
            #if DEBUG_SERVIDOR_LICAT
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> LICAT -> NULL?\n");
                // =================================================
            #endif
            if (aux != NULL) {
                sprintf(response.msg.umarg.string,"%d %s %s %d %d %d %s %s\n",aux->id,aux->nome,aux->categoria,aux->valorBase,aux->compraJa,aux->tempo,aux->vendedor,aux->comprador);
                #if DEBUG_SERVIDOR_LICAT
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LICAT -> RESPOSTA[%s]\n",response.msg.umarg.string);
                    // =================================================
                #endif
                if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
                    perror("Erro a Enviar dados login");
                    exit(1);
                }
                aux = aux->prox;
                #if DEBUG_SERVIDOR_LICAT
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LICAT -> aux = aux->prox\n");
                    printf("\n[DEBUG] -> %s\n",aux->nome);
                    // =================================================
                #endif
            }
            if (aux == NULL || aux->prox == NULL)
                    break;
        } while(aux != NULL);
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        sprintf(response.msg.umarg.string,"%s %s %s %s","\n[SERVIDOR]","FIM LISTA DE ITENS DA CATEGORIA [",ppr.msg.umarg.string,"]");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        close(fdEnvio);
        free(aux);
    }
}

void lisel(CLIENTES *coms, PREPARACAO ppr){
    int controlo = 0;
    pthread_mutex_lock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1){
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        if (fdEnvio == -1) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
            // =================================================
        }
        PREPARACAO response;
        strcpy(response.key,SERVER_KEY);
        response.pid = getpid();
        pItem aux = geral.itemsList;
        char *buffer;
        int avanca = 0;
        sprintf(response.msg.umarg.string,"%s %s %s %s","\n[SERVIDOR]","INICIO LISTA DE ITENS DE VENDEDOR [",ppr.msg.umarg.string,"]");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        do {
            response.pid = getpid();
            aux = percorreListaVendedor(aux,ppr.msg.umarg.string);
            if (aux != NULL) {
                sprintf(response.msg.umarg.string,"%d %s %s %d %d %d %s %s\n",aux->id,aux->nome,aux->categoria,aux->valorBase,aux->compraJa,aux->tempo,aux->vendedor,aux->comprador);
                if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
                    perror("Erro a Enviar dados login");
                    exit(1);
                }
                aux = aux->prox;
            }
        } while(aux != NULL);
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        sprintf(response.msg.umarg.string,"%s %s %s %s","[SERVIDOR]","FIM LISTA DE ITENS DE VENDEDOR [",ppr.msg.umarg.string,"]\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        close(fdEnvio);
        free(aux);
    }
}

void lival(CLIENTES *coms, PREPARACAO ppr){
    int controlo = 0;
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    pthread_mutex_lock(geral.m);
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1){
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        if (fdEnvio == -1) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
            // =================================================
        }
        PREPARACAO response;
        strcpy(response.key,SERVER_KEY);
        response.pid = getpid();
        pItem aux = geral.itemsList;
        char *buffer;
        int avanca = 0;
        
        sprintf(response.msg.umarg.string,"%s %s %d %s","[SERVIDOR]","INICIO DE ITENS ATE VALOR [",ppr.msg.umarg.numero,"]\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        do {
            response.pid = getpid();
            aux = percorreListaValor(aux,ppr.msg.umarg.numero);
            if (aux != NULL) {
                sprintf(response.msg.umarg.string,"%d %s %s %d %d %d %s %s\n",aux->id,aux->nome,aux->categoria,aux->valorBase,aux->compraJa,aux->tempo,aux->vendedor,aux->comprador);
                if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
                    perror("Erro a Enviar dados login");
                    exit(1);
                }
                aux = aux->prox;
            }
        } while(aux != NULL);
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        sprintf(response.msg.umarg.string,"%s %s %d %s","[SERVIDOR]","FIM DE ITENS ATE VALOR [",ppr.msg.umarg.numero,"]\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            perror("Erro a Enviar dados login");
            exit(1);
        }
        free(aux);
        close(fdEnvio);
    }
}

void litime(CLIENTES *coms, PREPARACAO ppr){
    int controlo = 0;
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    pthread_mutex_lock(geral.m);
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1){
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        if (fdEnvio == -1) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
            // =================================================
        }
        PREPARACAO response;
        strcpy(response.key,SERVER_KEY);
        response.pid = getpid();
        pItem aux = geral.itemsList;
        char *buffer;
        int avanca = 0;
        response.pid = getpid();
        sprintf(response.msg.umarg.string,"%s %s %d %s","[SERVIDOR]","INICIO DE ITENS ATE TEMPO [",ppr.msg.umarg.numero,"]\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            printf("\n[ERROR] -> ERRO AO COMUNICAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            exit(1);
        }
        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        do {
            aux = percorreListaPrazo(aux,ppr.msg.umarg.numero);
            if (aux != NULL) {
                sprintf(response.msg.umarg.string,"%d %s %s %d %d %d %s %s",aux->id,aux->nome,aux->categoria,aux->valorBase,aux->compraJa,aux->tempo,aux->vendedor,aux->comprador);
                if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
                    printf("\n[ERROR] -> ERRO AO COMUNICAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
                    exit(1);
                }
                aux = aux->prox;
            }
        } while(aux != NULL);
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        sprintf(response.msg.umarg.string,"%s %s %d %s","[SERVIDOR]","FIM DE ITENS ATE TEMPO [",ppr.msg.umarg.numero,"]\n");
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            printf("\n[ERROR] -> ERRO AO COMUNICAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            exit(1);
        }
        free(aux);
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MENSAGEM ENVIADA AO FIFO[%s]\n", SERVIDOR_CLIENTE_FINAL);
            // =================================================
        #endif
        close(fdEnvio);
    }

}

void buy(CLIENTES *coms, PREPARACAO ppr) {
    int controlo = 0;
    char username[MAX];
    pthread_mutex_lock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            #if DEBUG_SERVIDOR_BUY
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> SELL -> %d %d %s\n",ppr.msg.buy.id,ppr.msg.buy.valor,coms->clientes[i].username,username);
                // =================================================
            #endif
            strcpy(username,coms->clientes[i].username);
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1){
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        #if DEBUG_SERVIDOR_BUY
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> SELL -> VAI PROCURAR -> %d %d %s\n",ppr.msg.buy.id,ppr.msg.buy.valor,username);
            // =================================================
        #endif
        if (getUserBalance(username) >= ppr.msg.buy.valor)  {
            int valido = editPrecoFinal(geral.itemsList,ppr.msg.buy.id,ppr.msg.buy.valor,username);
            #if DEBUG_SERVIDOR_BUY
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> SELL -> SALDO SUFICIENTE -> VALIDO = %d\n",valido);
                // =================================================
            #endif
        } else {
            #if DEBUG_SERVIDOR_BUY
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> SELL -> SALDO INSUFICIENTE\n");
                // =================================================
            #endif
        }
        
        pthread_mutex_unlock(geral.m);
        #if DEBUG_SERVIDOR_BUY
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> SELL -> PROCUROU -> %d %d %s\n",ppr.msg.buy.id,ppr.msg.buy.valor,username);
            // =================================================
        #endif
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
    }
}

void timer(CLIENTES *coms, PREPARACAO ppr){
    int controlo = 0;
    pthread_mutex_lock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1) {
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        if (fdEnvio == -1) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
            // =================================================
        }
        PREPARACAO response;
        strcpy(response.key,SERVER_KEY);
        response.pid = getpid();
        response.msg.umarg.numero = geral.tempo;
        sprintf(response.msg.umarg.string,"%s %d %s\n","[SERVIDOR] TEMPO[",geral.tempo,"]");
        #if DEBUG_CLIENTES_TIMER
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> TIMER [%s]\n",response.msg.umarg.string);
            // =================================================
        #endif
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            printf("\n[ERROR] -> ERRO AO COMUNICAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            exit(1);
        }

        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MENSAGEM ENVIADA AO FIFO[%s]\n", SERVIDOR_CLIENTE_FINAL);
            // =================================================
        #endif
        close(fdEnvio);

    }

}

void cash(CLIENTES *coms, PREPARACAO ppr) {
    int controlo = 0;
    char username[MAX];
    pthread_mutex_lock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            strcpy(username,coms->clientes[i].username);
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1){
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        int fdEnvio = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
        if (fdEnvio == -1) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> ERRO AO CONECTAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            //return ERRO_FIFO_SERVIDOR_JA_EXISTE;
            // =================================================
        }
        PREPARACAO response;
        response.pid = getpid();
        strcpy(response.key,SERVER_KEY);
        pthread_mutex_lock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        sprintf(response.msg.umarg.string,"%s %d %s\n","[SERVIDOR] TEMPO[",getUserBalance(username),"]");
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        if (write(fdEnvio, &response, sizeof(PREPARACAO)) == -1) {
            printf("\n[ERROR] -> ERRO AO COMUNICAR COM CLIENTE [%s]\n", SERVIDOR_CLIENTE_FINAL);
            exit(1);
        }
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MENSAGEM ENVIADA AO FIFO[%s]\n", SERVIDOR_CLIENTE_FINAL);
            // =================================================
        #endif
        close(fdEnvio);
    }
}

void add(CLIENTES *coms, PREPARACAO ppr) {
    int controlo = 0;
    char username[MAX];
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    pthread_mutex_lock(geral.m);
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            strcpy(username,coms->clientes[i].username);
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1) {
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        pthread_mutex_lock(geral.m);
        int aux = updateUserBalance(username,getUserBalance(username) + ppr.msg.umarg.numero);
        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MENSAGEM ENVIADA AO FIFO[%s]\n", SERVIDOR_CLIENTE_FINAL);
            // =================================================
        #endif
    }
}

void sell(CLIENTES *coms, PREPARACAO ppr) {
    int controlo = 0;
    char username[MAX];
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
        // =================================================
    #endif
    pthread_mutex_lock(geral.m);
    for (size_t i = 0; i < MAXCLI; i++) {
        if (coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
            strcpy(username,coms->clientes[i].username);
            controlo = 1;
            break;
        }
    }
    pthread_mutex_unlock(geral.m);
    #if DEBUG_MUTEXES
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
        // =================================================
    #endif
    if(controlo == 1) {
        sprintf(SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M LOCK\n");
            // =================================================
        #endif
        pthread_mutex_lock(geral.m);
        #if DEBUG_SERVIDOR_SELL
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> SELL -> %s %s %d %d %d %s\n",ppr.msg.sell.nome,ppr.msg.sell.categoria,ppr.msg.sell.precoBase,ppr.msg.sell.precoCompreJa,ppr.msg.sell.duracao,username);
            // =================================================
        #endif
        geral.itemsList = addItem(coms,geral.itemsList,geral.tempo,ppr.msg.sell.nome,ppr.msg.sell.categoria,ppr.msg.sell.precoBase,ppr.msg.sell.precoCompreJa,ppr.msg.sell.duracao,username,"-");

        pthread_mutex_unlock(geral.m);
        #if DEBUG_MUTEXES
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> LOGIN -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        #endif
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MENSAGEM ENVIADA AO FIFO[%s]\n", SERVIDOR_CLIENTE_FINAL);
            // =================================================
        #endif
    }
}

void exitCliente(CLIENTES *coms,PREPARACAO ppr){
    for (size_t i = 0; i < MAXCLI; i++) {
        if (strcmp(coms->clientes[i].username,ppr.msg.login.userName) == 0 && coms->clientes[i].ativo == 1 && coms->clientes[i].pid == ppr.pid) {
           resetCliente(&coms->clientes[i]);
        }
    }
}

void *recebeClientes(void *clientes){

    #if DEBUG_CLIENTES_THREAD
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> INICIO CLIENTES\n");
        // =================================================
    #endif

    CLIENTES *pClientes = (CLIENTES *)clientes;

    // ABRE NAMED PIPE PARA RECEBER MENSAGENS
    int fdRecebe = open(SERVER_FIFO, O_RDWR);
    if (fdRecebe == -1) {
        printf("Erro");
        exit(1);
    }

    do {
        PREPARACAO ppr;
        int nBytes;
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> EM ESPERA DE CLIENTE...\n");
            // =================================================
        #endif
        if ((nBytes = read(fdRecebe, &ppr, sizeof(PREPARACAO))) == -1) {
            fprintf(stderr, "Erro ao ler do pipe %s!\n", SERVER_FIFO);
            exit(1);
        }
        #if DEBUG_CLIENTES_THREAD
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MENSAGEM RECEBIDA\n");
            // =================================================
        #endif

        switch (ppr.tipo) {
            case EXIT:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> EXIT ...\n");
                    // =================================================
                #endif
                exitCliente(pClientes, ppr);
                break;

            case LOGINCODE:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LOGIN ...\n");
                    // =================================================
                #endif
                login(pClientes, ppr);
                break;

            case SELLCODE:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> SELLCODE ...\n");
                    // =================================================
                #endif
                sell(pClientes, ppr);
                break;

            case LIST:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LIST ...\n");
                    // =================================================
                #endif
                list(pClientes, ppr);
                break;

            case LISEL:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LISEL ...\n");
                    // =================================================
                #endif
                lisel(pClientes, ppr);
                break;

            case LICAT:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LICAT ...\n");
                    // =================================================
                #endif
                licat(pClientes, ppr);
                break;

            case LIVAL:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LIVAL ...\n");
                    // =================================================
                #endif
                lival(pClientes, ppr);
                break;

            case LITIME:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> LITIME ...\n");
                    // =================================================
                #endif
                litime(pClientes, ppr);
                break;

            case TIME:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> TIME ...\n");
                    // =================================================
                #endif
                timer(pClientes, ppr);
                break;

            case BUYCODE:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> BUYCODE ...\n");
                    // =================================================
                #endif
                buy(pClientes, ppr);
                break;

            case CASH:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> CASH ...\n");
                    // =================================================
                #endif
                cash(pClientes, ppr);
                break;

            case ADD:
                #if DEBUG_CLIENTES_THREAD
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> ADD ...\n");
                    // =================================================
                #endif
                add(pClientes, ppr);
                break;

            default:
                break;
        }


    } while(geral.stop == 0);

    #if DEBUG_CLIENTES_THREAD
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIM CLIENTES\n");
        // =================================================
    #endif
}

int leFichProms(THREAD_PROMOTORES *prom)
{
    // ==================== DEBUG ======================
    if (DEBUG_PROMOTORES_LE_FICHA)
        printf("\n[DEBUG] -> LER FICHEIRO DOS PROMOTORES INICIO\n");
    // =================================================

    if (access(getenv("FPROMOTERS"), F_OK) != 0)
    {
        printf("\n[SYSTEM] -> SEM PROMOTORES\n");
        return ERRO_SEM_PROMOTORES;
    }
    else
    {
        FILE *fp;
        fp = fopen(getenv("FPROMOTERS"), "r");

        if (fp == NULL)
        {
            fprintf(stderr, "\n[ERROR] -> ERRO AO CARREGAR FICHEIRO %s\n", getenv("FPROMOTERS"));
            return ERRO_LEITURA_FICHEIRO_PROMOTORES;
        }

        char lista[MAXPROM][20];

        pthread_mutex_lock(geral.m);
        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MUTEX GERAL.M LOCK\n");
            // =================================================
        }

        for (size_t i = 0; i < MAXPROM; i++)
        {
            if (prom[i].promotor.ativo == 1)
            {
                prom[i].promotor.stop = 1;
                if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
                {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> prom[%d].promotor.%s.stop = %d\n", i, prom[i].promotor.nome, prom[i].promotor.stop);
                    // =================================================
                }
            }
        }

        for (size_t i = 0; i < MAXPROM; i++)
        {
            if (fscanf(fp, "%s", lista[i]) != EOF)
            {
                printf("\n%s\n", lista[i]);
                for (size_t j = 0; j < MAXPROM; j++)
                {
                    if (strcmp(lista[i], prom[j].promotor.nome) == 0)
                    {
                        prom[j].promotor.stop = 0;
                        prom[j].promotor.ativo = 1;
                        strcpy(lista[i], "SEMPROMOTOR");
                        if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
                        {
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] -> NOVO prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                            // =================================================
                        }
                    }
                }
            }
            else
            {
                strcpy(lista[i], "SEMPROMOTOR");
                if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
                {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> SEM PROMOCAO\n");
                    // =================================================
                }
            }
        }

        if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
        {
            for (int i = 0; i < MAXPROM; i++)
            {
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                // =================================================
            }
        }

        pthread_mutex_unlock(geral.m);
        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        }

        for (size_t i = 0; i < MAXPROM; i++)
        {
            if (prom[i].promotor.stop == 1)
            {
                if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
                {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> VAI PARAR prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                    // =================================================
                }
                pthread_kill(prom[i].tPromotor, SIGUSR1);
                if (pthread_join(prom[i].tPromotor, NULL) != 0) {}
            }
        }

        if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
        {
            for (int i = 0; i < MAXPROM; i++)
            {
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                // =================================================
            }
        }

        pthread_mutex_lock(geral.m);
        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MUTEX GERAL.M LOCK\n");
            // =================================================
        }

        for (size_t i = 0; i < MAXPROM; i++)
        {
            if (prom[i].promotor.ativo == 1 && prom[i].promotor.stop == 1)
            {
                if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
                {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> RESET prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                    // =================================================
                }
                resetPromotor(&prom[i].promotor);
            }
        }

        if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
        {
            for (int i = 0; i < MAXPROM; i++)
            {
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                // =================================================
            }
        }

        for (size_t i = 0; i < MAXPROM; i++)
        {
            if (strcmp(lista[i], "SEMPROMOTOR") != 0)
            {
                if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
                {
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> CATEGORIA[%d]: %s\n", i, lista[i]);
                    // =================================================
                }
                for (size_t j = 0; j < MAXPROM; j++)
                {
                    if (prom[j].promotor.ativo == 0)
                    {
                        prom[j].promotor.ativo = 1;
                        prom[j].promotor.stop = 1;
                        strcpy(prom[j].promotor.nome, lista[i]);
                        strcpy(lista[i], "SEMPROMOTOR");
                        if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
                        {
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] -> RESET prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                            // =================================================
                        }
                        break;
                    }
                }
            }
        }

        if (DEBUG_PROMOTORES_VERIFICA_ATIVOS)
        {
            for (int i = 0; i < MAXPROM; i++)
            {
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> prom[%d].promotor.%s.ativo[%d].stop[%d]\n", i, prom[i].promotor.nome, prom[i].promotor.ativo, prom[i].promotor.stop);
                // =================================================
            }
        }

        pthread_mutex_unlock(geral.m);
        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        }

        fclose(fp);
    }

    // ==================== DEBUG ======================
    if (DEBUG_PROMOTORES_LE_FICHA)
        printf("\n[DEBUG] -> LER FICHEIRO DOS PROMOTORES FIM\n");
    // =================================================

    return SUCESSO;
}

void *iniciaPromotores(void *promo)
{
    PROMOTOR *ppromo = (PROMOTOR *)promo;
    if (strcmp(ppromo->nome, "VAZIO") == 0)
        ppromo->stop = 1;

    if ( ppromo->stop == 0) {
        int pid = fork();
        if (pid < 0)
        {
            perror("Erro fork nao criado!");
            exit(1);
        }

        if (pid == 0)
        {

            // child
            ppromo->pid = getpid();
            close(ppromo->fdProms[0]);
            close(STDOUT_FILENO);
            dup(ppromo->fdProms[1]);
            close(ppromo->fdProms[1]);
            char nome[50];
            sprintf(nome, "./backend/promoters/%s", ppromo->nome);

            execl(nome, nome, NULL);

            exit(1);
        }
        if (pid > 0)
        {
            // parent

            char categoria[20];
            int desconto;
            int segundos;

            close(ppromo->fdProms[1]);
            ppromo->pid = pid;
            while (ppromo->stop == 0 && geral.stop == 0)
            {
                char buffer[70];
                char delim[] = " ";

                // ==================== DEBUG ======================
                if (DEBUG_GERAL_STOP)
                    printf("\n[DEBUG] -> INICIA_PROMOTORES -> GERAL.STOP = %d\n", geral.stop);
                // =================================================

                // ==================== DEBUG ======================
                if (DEBUG_PROMOTORES)
                    printf("\n[DEBUG] -> EM ESPERA DE PROMOTOR COM PID [%d] E NOME [%s]\n", ppromo->pid, ppromo->nome);
                // =================================================

                int size = 0;
                size = read(ppromo->fdProms[0], buffer, sizeof(buffer));

                // ==================== DEBUG ======================
                if (DEBUG_PROMOTORES)
                    printf("\n[DEBUG] -> RECEBIDO [%d] DE PROMOTOR COM PID [%d] E NOME [%s]\n", size,ppromo->pid, ppromo->nome);
                // =================================================

                // ==================== DEBUG ======================
                if (DEBUG_GERAL_STOP)
                    printf("\n[DEBUG] -> INICIA_PROMOTORES -> GERAL.STOP = %d\n", geral.stop);
                // =================================================

                // ==================== DEBUG ======================
                if (DEBUG_PROMOTORES)
                    printf("\n[DEBUG] -> PROMOTOR.STOP = %d\n", ppromo->stop);
                // =================================================

                if (geral.stop == 0 && ppromo->stop == 0)
                {
                    int repetido = 0;
                    buffer[size - 1] = '\0';

                    // ==================== DEBUG ======================
                    if (DEBUG_PROMOTORES)
                        printf("\n[DEBUG] -> RECEBIDO [%s] DE PROMOTOR COM PID [%d] E NOME [%s]\n", buffer,ppromo->pid, ppromo->nome);
                    // =================================================

                    char *ptr = strtok(buffer, delim);

                    char categoria[30];
                    int desconto;
                    int segundos;

                    strcpy(categoria, ptr);
                    ptr = strtok(NULL, delim);
                    desconto = atoi(ptr);
                    ptr = strtok(NULL, delim);
                    segundos = atoi(ptr);

                    pthread_mutex_lock(geral.m);
                    if (DEBUG_MUTEXES) {
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> MUTEX GERAL.M LOCK\n");
                        // =================================================
                    }           

                    for (int i = 0; i < MAXPROM; i++)
                    {
                        if (ppromo->prom[i].ativo == 1 && strcmp(ppromo->prom[i].categoria, buffer) == 0)
                        {
                            repetido = 1;
                            // ==================== DEBUG ======================
                            if (DEBUG_PROMOCOES)
                                printf("\n[DEBUG] -> REPETIDO [%s] DE PROMOTOR COM PID [%d] E NOME [%s]\n", buffer,ppromo->pid, ppromo->nome);
                            // =================================================
                            break;
                        }
                    }

                    if (!repetido)
                    {
                        ++ppromo->nPromocoes;
                        // ==================== DEBUG ======================
                        if (DEBUG_PROMOCOES)
                            printf("\n[DEBUG] -> NUMERO DE PROMOCOES [%d] DE PROMOTOR COM PID [%d] E NOME [%s]\n", ppromo->nPromocoes,ppromo->pid, ppromo->nome);
                        // =================================================

                        for (int i = 0; i < MAXPROM; i++)
                        {
                            if (ppromo->prom[i].ativo == 0)
                            {
                                strcpy(ppromo->prom[i].categoria, categoria);
                                ppromo->prom[i].desconto = desconto;
                                ppromo->prom[i].segundos = segundos;
                                ppromo->prom[i].ativo = 1;
                                i = ppromo->nPromocoes;

                                // ==================== DEBUG ======================
                                if (DEBUG_PROMOCOES)
                                    printf("\n[DEBUG] -> NOVO prom[%d].categoria[%s].desconto[%d].segundos[%d]\n", i, ppromo->prom[i].categoria, ppromo->prom[i].desconto, ppromo->prom[i].segundos);
                                // =================================================

                                PREPARACAO ppr;
                                ppr.pid = getpid();
                                strcpy(ppr.key,SERVER_KEY);
                                for (size_t j = 0; j < MAXCLI; j++) {
                                    if (geral.cli->clientes[j].ativo == 1) {
                                        sprintf( SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, geral.cli->clientes[j].pid );
                                        int fdNotificacao = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
                                        sprintf(ppr.msg.umarg.string,"\n%s %s %s %d %s %d %s\n","[NOTIFICACAO] NOVA PROMOCAO NA CATEGORIA[",categoria,"] DE", desconto,"% DURANTE",segundos,"SEGUNDOS");
                                        if (fdNotificacao != -1) {
                                            if( write(fdNotificacao,&ppr,sizeof (PREPARACAO)) == -1 ){
                                                //perror("Erro a Enviar dados login");
                                                close(fdNotificacao);
                                            }
                                        }
                                    }
                                }
                                //geral.itemsList = colocaPromocao(geral.itemsList, categoria, desconto);
                                break;
                            }
                        }
                    }
                    
                    if (DEBUG_PROMOCOES)
                    {
                        printf("\n[DEBUG] -> LISTA:\n");
                        for (int i = 0; i < MAXPROM; i++)
                        {
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] -> prom[%d].categoria[%s].desconto[%d].segundos[%d]\n", i, ppromo->prom[i].categoria, ppromo->prom[i].desconto, ppromo->prom[i].segundos);
                            // =================================================
                        }
                        printf("\n[DEBUG] -> FIM\n");
                    }

                    pthread_mutex_unlock(geral.m);
                    if (DEBUG_MUTEXES) {
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> MUTEX GERAL.M LOCK\n");
                        // =================================================
                    }    
                }
            }

            // ==================== DEBUG ======================
            if (DEBUG_GERAL_STOP)
                printf("\n[DEBUG] -> INICIA_PROMOTORES -> GERAL.STOP = %d\n", geral.stop);
            // =================================================

            // ==================== DEBUG ======================
            if (DEBUG_PROMOTORES)
                printf("\n[DEBUG] -> PROMOTOR [%s] VAI TERMINAR\n", ppromo->nome);
            // =================================================

            kill(ppromo->pid, SIGUSR1);
            wait(&ppromo->pid);

            // ==================== DEBUG ======================
            if (DEBUG_PROMOTORES)
                printf("\n[DEBUG] -> PROMOTOR [%s] VAI TERMINOU\n", ppromo->nome);
            // =================================================
        }

    }
}

void terminaCliente(INFOS_CLIENTES * c) {
    kill(c->pid,SIGINT);
    //wait(&c->pid);
    resetCliente(c);
}

void resetCliente(INFOS_CLIENTES *c){
    strcpy(c->username,"Vazio");
    c->tempo = 0;
    c->ativo = 0;
    c->pid = 0;
}

int procPromocao(char categoria[40],COMANDOS *coms,int preco) {

    for (size_t i = 0; i < MAXPROM; i++) {
        for (size_t j = 0; j < MAXPROM; j++) {
            if (coms->prom[i].promotor.ativo == 1) {
                if (coms->prom[i].promotor.prom[j].ativo == 1) {
                    if (strcmp(coms->prom[i].promotor.prom[j].categoria,categoria) == 0) {
                        int desconto1 = 100 - coms->prom[i].promotor.prom[j].desconto;
                        float desconto2 = (float)desconto1 / 100;
                        float precoFinal = desconto2 * (float)preco;
                        if (DEBUG_ITEMS_APLICA_PROMOCAO) {
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] -> PRECO INICIAL = %d\n",preco);
                            printf("\n[DEBUG] -> DESCONTO[%s] = %d\n",coms->prom[i].promotor.prom[j].categoria,coms->prom[i].promotor.prom[j].desconto);
                            printf("\n[DEBUG] -> DESCONTO1 = %d\n",desconto1);
                            printf("\n[DEBUG] -> DESCONTO2 = %.2f\n",desconto2);
                            printf("\n[DEBUG] -> PRECO FINAL = %2.f\n",precoFinal);
                            // =================================================
                        }
                        preco = precoFinal;
                        if (DEBUG_ITEMS_APLICA_PROMOCAO) {
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] -> PRECO PARA RETURN = %d\n",preco);
                            // =================================================
                        }
                    }
                }
            }
        }
    }
    return preco;
}

void trataTempoItems(COMANDOS *coms) {
    int avanca = 0;
    pItem aux = geral.itemsList;

    do {
        aux = avancaTempoItems(geral.itemsList,geral.tempo);

        #if DEBUG_ITEMS_TEMPO_AVANCA
        // ==================== DEBUG ======================
            printf("\n[DEBUG] -> AVANCA TEMPO ITEM\n");
            // =================================================
        #endif

        if (aux != NULL && (aux->tempo == geral.tempo || aux->valorBase == aux->compraJa)) {
            #if DEBUG_ITEMS_TEMPO_AVANCA
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> TEMPO[%d]\n",geral.tempo);
                mostraInfo(aux);
                // =================================================
            #endif
            if (strcmp(aux->comprador,"-") == 0){
                #if DEBUG_ITEMS_VENDA_COMPRADOR 
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> ITEM [%d] SEM COMPRADOR\n", aux->id);
                    // =================================================
                #endif
                // NOTIFICACAO
            } else {
                #if DEBUG_ITEMS_VENDA_COMPRADOR
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> ITEM [%d] COM COMPRADOR [%s]\n", aux->id,aux->comprador);
                    // =================================================
                #endif
                // NOTIFICACAO

                int saldo = getUserBalance(aux->comprador);

                #if DEBUG_ITEMS_VENDA_COMPRADOR_SALDO
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> SALDO COMPRADOR [%s]: %d\n", aux->comprador,saldo);
                    // =================================================
                #endif
                
                switch (saldo) {
                    case -1:
                        #if DEBUG_ITEMS_VENDA_COMPRADOR_SALDO
                            // ==================== ERROR ======================
                            printf("\n[ERROR] -> ERRO A RECEBER SALDO DO COMPRADOR [%s]\n",aux->comprador);
                            // =================================================
                        #endif
                        break;
                    
                    default:
                        aux->valorBase = procPromocao(aux->categoria,coms,aux->valorBase);
                        #if DEBUG_ITEMS_APLICA_PROMOCAO
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] -> PRECO DEPOIS DE PROMOCAO = %d\n",aux->valorBase);
                            // =================================================
                        #endif
                        updateUserBalance(aux->comprador,saldo - aux->valorBase);
                        saldo = getUserBalance(aux->comprador);
                        #if DEBUG_ITEMS_VENDA_COMPRADOR_SALDO
                            // ==================== DEBUG ======================
                            printf("\n[DEBUG] -> SALDO DEPOIS DA COMPRA DO COMPRADOR [%s]: %d\n", aux->comprador,saldo);
                            // =================================================
                        #endif
                        break;
                }
            }

            int id = aux->id;
            if (geral.itemsList == aux) {
                geral.itemsList = vendeItem(&coms->cliente,aux,geral.itemsList,id);
            } else {
                aux = vendeItem(&coms->cliente,aux,geral.itemsList,id);
            }
        }

        if ( aux == NULL || aux->prox == NULL)
            avanca = 1;
        
    } while (avanca == 0);
    
}

void trataTempoClientes(COMANDOS *coms){

    if (DEBUG_CLIENTE_TEMPO_AVANCA) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> AVANCA TEMPO CLIENTES\n");
        // =================================================
    }
    
    if (coms->cliente.nClientes > 0) {
        for (size_t i = 0; i < MAXCLI; i++) {
            if (coms->cliente.clientes[i].ativo == 1) {
                coms->cliente.clientes[i].tempo -= 1;
                if (coms->cliente.clientes[i].tempo <= 0) {
                    if (DEBUG_CLIENTE_TEMPO_AVANCA) {
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> [%s] VAI TERMINAR\n",coms->cliente.clientes[i].username);
                        // =================================================
                    }
                    terminaCliente(&coms->cliente.clientes[i]);
                    coms->cliente.nClientes -= 1;
                }
                
            }
        } 
    }
    
}

void resetPromocao(PROMOCAO *p)
{
    // printf("\nRESET PROMOCAO\n");
    //geral.itemsList = ppr.pidretiraPromocao(geral.itemsList, p->categoria);
    PREPARACAO ppr;
    strcpy(ppr.key,SERVER_KEY);
    for (size_t i = 0; i < MAXCLI; i++) {
        if (geral.cli->clientes[i].ativo == 1) {
            ppr.pid = geral.cli->clientes[i].pid;
            sprintf( SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, ppr.pid );
            int fdNotificacao = open(SERVIDOR_CLIENTE_FINAL, O_WRONLY);
            sprintf(ppr.msg.umarg.string,"\n%s %s %s %d %s\n","[NOTIFICACAO] PROMOCAO NA CATEGORIA[",p->categoria,"] DE", p->desconto,"% TERMINOU");
            if (fdNotificacao != -1) {
                if( write(fdNotificacao,&ppr,sizeof (PREPARACAO)) == -1 ){
                    //perror("Erro a Enviar dados login");
                    close(fdNotificacao);
                }
            }
        }
    }

    p->ativo = 0;
    strcpy(p->categoria, "Vazio");
    p->desconto = 0;
    p->segundos = 0;
}

void resetPromotor(PROMOTOR *p)
{
    // printf("\nRESET PROMOTOR\n");
    p->pid = getpid();
    strcpy(p->nome, "VAZIO");
    p->nPromocoes = 0;
    for (size_t i = 0; i < MAXPROM; i++)
    {
        resetPromocao(&p->prom[i]);
    }
    p->stop = 0;
    p->ativo = 0;
}

void *recebePromotores(void *tProms)
{

    THREAD_PROMOTORES *ptProms = (THREAD_PROMOTORES *)tProms;

    if (leFichProms(ptProms) != SUCESSO)
        geral.stop = 1;

    if (geral.stop == 0)
    {
        if (DEBUG_PROMOTORES) {
            for (int i = 0; i < MAXPROM; i++)
            {
                printf("\n[DEBUG] -> ATIVO = %d\n", ptProms[i].promotor.ativo);
            }
        }

        for (int i = 0; i < MAXPROM; i++)
        {
            if (ptProms[i].promotor.ativo == 1 && ptProms[i].promotor.stop == 1)
            {
                ptProms[i].promotor.stop = 0;
                pipe(ptProms[i].promotor.fdProms);
                if (pthread_create(&ptProms[i].tPromotor, NULL, &iniciaPromotores, &ptProms[i].promotor) != 0) {}
            }
        }
    }
    
}

int avancaTempo(TEMPO *pTempo)
{
    if (DEBUG_TEMPO)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> INICIO TEMPO\n");
        // =================================================
    }

    if (geral.nClientes > 0)
    {
        printf("\n[SERVER] %d UTILIZADORES CARREGADOS\n", loadUsersFile(getenv("FUSERS")));
    }
    else
    {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> NAO EXISTEM UTILIZADORES\n");
        // =================================================
        return ERRO_NUMERO_INSUFICIENTE_CLIENTES;
    }

    geral.fdServ = open(SERVER_FIFO, O_RDWR);
    if (geral.fdServ == -1)
    {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> ERRO ABRIR FIFO\n");
        // =================================================
        return ERRO_ABRIR_FIFO;
    }

    // ==================== DEBUG ======================
    if (DEBUG_SERVIDOR_FIFO_ABRIR)
        printf("\n[DEBUG] -> FIFO [%s] ABERTO\n", SERVER_FIFO);
    // =================================================

    leFichItems();

    if (pthread_create(&pTempo->tComandos, NULL, &recebeComando, &pTempo->comandos) != 0) {}

    #if DEBUG_ITEMS_MOSTRA_LIDOS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> ITEMS LIDOS -> DEPOIS DA THREAD COMANDOS\n");
        mostraInfo(geral.itemsList);
        printf("\n[DEBUG] -> FIM LISTA:\n");
        // =================================================
    #endif

    if (DEBUG_COMANDOS_THREAD_CRIA)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> THREAD COMANDOS CRIADA\n");
        // =================================================
    }

    if (DEBUG_CLIENTES_THREAD_CRIA)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> THREAD CLIENTES CRIADA\n");
        // =================================================
    }

    do
    {
        sleep(1);
        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> TEMPO -> MUTEX GERAL.M LOCK\n");
            // =================================================
        }
        pthread_mutex_lock(geral.m);
        
        #if DEBUG_ITEMS_MOSTRA_LIDOS
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEMS LIDOS -> INICIO TEMPO\n");
            mostraInfo(geral.itemsList);
            printf("\n[DEBUG] -> FIM LISTA:\n");
            // =================================================
        #endif

        ++pTempo->tempo;
        geral.tempo = pTempo->tempo;

        if (DEBUG_TEMPO_MOSTRA)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> TEMPO [%d]\n", pTempo->tempo);
            // =================================================
        }
        // pItem aux = itemsList;

        if (DEBUG_ITEMS_TEMPO_AVANCA)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> AVANCA TEMPO ITEMS INICIO\n");
            // =================================================
        }

        trataTempoItems(&pTempo->comandos);
        #if DEBUG_ITEMS_MOSTRA_LIDOS
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ITEMS LIDOS -> DEPOIS TRATA TEMPO\n");
            mostraInfo(geral.itemsList);
            printf("\n[DEBUG] -> FIM LISTA:\n");
            // =================================================
        #endif

        if (DEBUG_ITEMS_TEMPO_AVANCA)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> AVANCA TEMPO ITEMS FIM\n");
            // =================================================
        }

        for (int i = 0; i < MAXPROM; i++)
        {
            for (int j = 0; j < MAXPROM; j++)
            {
                if (pTempo->comandos.prom[i].promotor.prom[j].ativo == 1)
                {
                    if (DEBUG_PROMOCOES_TEMPO_AVANCA)
                    {
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> AVANCA TEMPO PROMOCAO INICIO\n");
                        // =================================================
                    }

                    --pTempo->comandos.prom[i].promotor.prom[j].segundos;
                    if (pTempo->comandos.prom[i].promotor.prom[j].segundos == 0)
                        resetPromocao(&pTempo->comandos.prom[i].promotor.prom[j]);

                    if (DEBUG_PROMOCOES_TEMPO_AVANCA)
                    {
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> AVANCA TEMPO PROMOCAO FIM\n");
                        // =================================================
                    }
                }
            }
        }

        if (DEBUG_CLIENTE_TEMPO_AVANCA) {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> TEMPO -> INICIO AVANCA TEMPO CLIENTES\n");
            // =================================================
        }
        trataTempoClientes(&pTempo->comandos);
        if (DEBUG_CLIENTE_TEMPO_AVANCA) {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> TEMPO -> FIM AVANCA TEMPO CLIENTES\n");
            // =================================================
        }

        if (DEBUG_MUTEXES)
        {
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> TEMPO -> MUTEX GERAL.M UNLOCK\n");
            // =================================================
        }
        pthread_mutex_unlock(geral.m);
        

        // ==================== DEBUG ======================
        if (DEBUG_GERAL_STOP)
            printf("\n[DEBUG] -> TEMPO -> GERAL.STOP = %d\n", geral.stop);
        // =================================================
    } while (geral.stop == 0);

    if (DEBUG_COMANDOS_THREAD_CRIA)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> THREAD COMANDOS JOINS\n");
        // =================================================
    }

    pthread_kill(pTempo->tComandos, SIGUSR1);
    if (pthread_join(pTempo->tComandos, NULL) != 0) {}

    if (DEBUG_COMANDOS_THREAD_CRIA)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> THREAD COMANDOS FIM JOINS\n");
        // =================================================
    }

    if (DEBUG_TEMPO)
    {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIM TEMPO\n");
        // =================================================
    }
}

int inicializaServidor(INICIALIZADOR *inicia, char *envp[]) {

    if (getenv("FPROMOTERS") == NULL) {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> VARIAVEL AMBIENTE DO FICHEIRO DE PROMOTORES NAO DECLARADA\n");
        terminaServerInvalido();
        // =================================================
    }

    if (getenv("FITEMS") == NULL) {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> VARIAVEL AMBIENTE DO FICHEIRO DE ITEMS NAO DECLARADA\n");
        terminaServerInvalido();
        // =================================================
    }

    if (getenv("FUSERS") == NULL) {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> VARIAVEL AMBIENTE DO FICHEIRO DE USERS NAO DECLARADA\n");
        terminaServerInvalido();
        // =================================================
    }

    if (getenv("HEARTBEAT") == NULL) {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> VARIAVEL AMBIENTE DOS HEARTBEATS NAO DECLARADA\n");
        terminaServerInvalido();
        // =================================================
    }
    
    sscanf(getenv("HEARTBEAT"),"%d",&heartbeats);

    // CRIA NAMED PIPE VERIFICANDO SE JA EXISTE UM SERVIDOR EM EXECUÇAO
    if (mkfifo(SERVER_FIFO, 0666) == -1) {
        if (errno == EEXIST) {
            // ==================== ERROR ======================
            printf("\n[ERROR] -> SERVIDOR EM EXECUCAO OU FIFO JA EXISTE\n");
            // sigqueue( getpid(), SIGINT, geral.intMain );
            terminaServerInvalido();
            // =================================================
        }
        // ==================== ERROR ======================
        printf("\n[ERROR] -> ERRO CRIAR FIFO\n");
        // sigqueue( getpid(), SIGINT, geral.intMain );
        terminaServerInvalido();
        // =================================================
    }
    
    #if DEBUG_SERVIDOR_CRIA_FIFO
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIFO [%s] SERVIDOR CRIADO\n", SERVER_FIFO);
        // =================================================
    #endif
    

    pthread_mutex_init(&inicia->mutex, NULL);
    #if DEBUG_MUTEX_STOP_GERAL_INICIA
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> MUTEX GERAL.STOP INICIALIZADO\n");
        // =================================================
    #endif

    geral.stop = 0;
    printf("\nPID[%d]\n", getpid());

    // VERIFICA QUANTIDADE DE UTILIZADORES GUARDADOS
    geral.nClientes = loadUsersFile(getenv("FUSERS"));
    if (geral.nClientes == -1) {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> ERRO AO LER FICHEIRO DOS CLIENTES\n");
        return ERRO_LEITURA_FICHEIRO_CLIENTES;
        // =================================================
    }

    verificaTempo();
    inicia->tempo.tempo = geral.tempo;

    geral.m = &inicia->mutex;
    #if DEBUG_MUTEX_STOP_GERAL_COLOCA
    // ==================== DEBUG ======================
        printf("\n[DEBUG] -> MUTEX GERAL.STOP COLOCADO\n");
    // =================================================
    #endif

    #if DEBUG_SERVIDOR_INICIALIZA
    // ==================== DEBUG ======================
        printf("\n[DEBUG] -> SERVIDOR INICIALIZADO\n");
    // =================================================
    #endif

    return SUCESSO;
}

int main(int argc, char **argv, char *envp[])
{
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_sigaction = sair;
    // sa.sa_flags = SA_SIGINFO;
    // sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    struct sigaction act;
    act.sa_sigaction = acorda;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    INICIALIZADOR inicia;

    if (inicializaServidor(&inicia, envp)) {
        avancaTempo(&inicia.tempo);
    }
    else
    {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> INICIALIZAR SERVIDOR\n");
        // =================================================
    }

    geral.stop = 1;
    // ==================== DEBUG ======================
    if (DEBUG_GERAL_STOP)
        printf("\n[DEBUG] -> MAIN -> GERAL.STOP = %d\n", geral.stop);
    // =================================================

    pthread_mutex_destroy(&inicia.mutex);
    if (DEBUG_DESTROI_MUTEX_STOP_GERAL) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> MUTEX GERAL.STOP TERMINADO\n");
        // =================================================
    }

    unlink(SERVER_FIFO);
    if (DEBUG_TERMINA_FIFO_SERVIDOR) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIFO [%s] TERMINADO\n", SERVER_FIFO);
        // =================================================
    }

    if (DEBUG_FIM) {
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIM PROGRAMA\n");
        // =================================================
    }
    return FIM;
}
