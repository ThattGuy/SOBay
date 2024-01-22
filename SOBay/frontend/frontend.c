#include "frontend.h"
GERAL geral;
char SERVIDOR_CLIENTE_FINAL[100];
int heartbeats;

void terminaClienteInvalido() {
    unlink(SERVIDOR_CLIENTE_FINAL);
    #if DEBUG_TERMINA_FIFO_SERVIDOR
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIFO [%s] TERMINADO\n", SERVIDOR_CLIENTE_FINAL);
        // =================================================
    #endif

    #if DEBUG_FIM
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIM PROGRAMA\n");
        // =================================================
    #endif
    exit(-1);
}

void acorda(int s, siginfo_t *info, void *c) {}


void sair(int sign, siginfo_t *info, void *s) {
    geral.stop = 1;
    unlink(SERVIDOR_CLIENTE_FINAL);
    unlink(SERVER_HEARTBEATS);
    exit(1);
}

void help() {

    printf("\nComandos Disponiveis: \n");
    printf("--Colocar um item a leilão: \n\t sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\n\n");
    printf("--Listar todos os produtos atualmente a venda: \n\t list\n\n");
    printf("--Listar todos os itens de uma categoria (categoria = 1 palavra apenas): \n\t licat <nome-categoria>\n\n");
    printf("--Listar todos os itens de um vendedor: \n\t lisel <username do vendedor>\n\n");
    printf("--Listar todos os itens com preço ate um determinado valor: \n\t lival <preço-máximo>\n\n");
    printf("--Listar todos os itens com prazo ate a uma determinada hora: \n\t litime <hora-em-segundos>\n\n");
    printf("--Obter a hora atual (em segundos): \n\t time\n\n");
    printf("--Licitar um item: \n\t buy <id> <valor>\n\n");
    printf("--Consultar o saldo: \n\t cash\n\n");
    printf("--Carregar o saldo: \n\t add <valor>\n\n");
    printf("--Sair: \n\t exit\n\n");

}

int verificaArgumentos(const char cmd[CMDSIZE], int nArg) {

    int count = 1;
    int i = 0;

    while (cmd[i] != '\0') {
        i++;
        if ((cmd[i] == ' ') && (cmd[i + 1] != ' ') && (cmd[i + 1] != '\0'))
            count++;
    }

    if (count != nArg) { //caso o numero de arg seja o necessario retorna 0
        return 1;
    }
    return 0;
}

void separaArgumentos(char cmd[CMDSIZE], char **arg, int tam) {

    char delim[] = " ";
    char *ptr = strtok(cmd, delim);
    arg[0] = ptr;

    for (int i = 1; i < tam; ++i) {
        ptr = strtok(NULL, delim);
        arg[i] = ptr;
    }

}

void sell(char cmd[CMDSIZE]) {

    if (verificaArgumentos(cmd, SELLSIZE)) {
        printf("Erro: Numero de argumentos errado, escreva 'help' para ajuda\n");
        return;
    }
    int num;

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = SELLCODE;

    char *comando[SELLSIZE];

    separaArgumentos(cmd, comando, SELLSIZE);
    for (int i = 0; i < SELLSIZE; ++i) {

        switch (i) {
            case 1:
                if (sscanf(comando[i], "%d", &num) == 1) {
                    printf("Atencao: o nome deve ser uma palavra.\n");
                    return;
                } else {
                    strcpy(ppr.msg.sell.nome, comando[i]);
                }
                break;

            case 2:
                if (sscanf(comando[i], "%d", &num) == 1) {
                    printf("Atencao: a categoria deve ser uma palavra.\n");
                    return;
                } else {
                    strcpy(ppr.msg.sell.categoria, comando[i]);
                }
                break;

            case 3:
                if (sscanf(comando[i], "%d", &num) != 1) {
                    printf("Atencao: o preço-base deve ser um numero inteiro maior que 0.\n");
                    return;
                } else if (num <= 0) {
                    printf("Atencao: o preço-base deve ser um numero inteiro maior que 0.\n");
                    return;
                } else {
                    ppr.msg.sell.precoBase = num;
                }
                break;

            case 4:
                if (sscanf(comando[i], "%d", &num) != 1) {
                    printf("Atencao: o preço-compre-ja deve ser um numero interiro maior que 0.\n");
                    return;
                } else if (num <= 0) {
                    printf("Atencao: o preço-compre-ja deve ser um numero inteiro maior que 0.\n");
                    return;
                }
                else{
                    ppr.msg.sell.precoCompreJa = num;
                }
                break;

            case 5:
                if (sscanf(comando[i], "%d", &num) != 1) {
                    printf("Atencao: a duracao deve ser um numero interiro maior que 0.\n");
                    return;
                } else if (num <= 0) {
                    printf("Atencao: a duracao deve ser um numero inteiro maior que 0.\n");
                    return;
                }
                else{
                    ppr.msg.sell.duracao = num;
                }
                break;

            default:
                break;
        }
    }

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
}

void list() {
    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = LIST;

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
}

void licat(char cmd[CMDSIZE]) {

    if (verificaArgumentos(cmd, LICATSIZE)) {
        printf("Erro: Numero de argumentos errado, escreva 'help' para ajuda\n");
        return;
    }

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = LICAT;

    char *comando[LICATSIZE];
    int num;

    separaArgumentos(cmd, comando, LICATSIZE);

    if (sscanf(comando[1], "%d", &num) == 1) {
        printf("Atencao: a categoria deve ser uma palavra.\n");
        return;
    }
    else{
        comando[1][strcspn(comando[1], "\n")] = '\0';
        strcpy(ppr.msg.umarg.string, comando[1]);
    }

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
}

void lisel(char cmd[CMDSIZE]) {

    if (verificaArgumentos(cmd, LISELSIZE)) {
        printf("Erro: Numero de argumentos errado, escreva 'help' para ajuda\n");
        return;
    }

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = LISEL;

    int num;
    char *comando[LISELSIZE];

    separaArgumentos(cmd, comando, LISELSIZE);

    if (sscanf(comando[1], "%d", &num) == 1) {
        printf("Atencao: o username deve ser uma palavra.\n");
        return;
    }
    else{
        comando[1][strcspn(comando[1], "\n")] = '\0';
        strcpy(ppr.msg.umarg.string, comando[1]);
    }

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
}

void lival(char cmd[CMDSIZE]) {

    if (verificaArgumentos(cmd, LIVALSIZE)) {
        printf("Erro: Numero de argumentos errado, escreva 'help' para ajuda\n");
        return;
    }

    char *comando[LIVALSIZE];
    int num;

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = LIVAL;

    separaArgumentos(cmd, comando, LIVALSIZE);

    if (sscanf(comando[1], "%d", &num) != 1) {
        printf("Atencao: o preco maximo deve ser um numero inteiro maior que 0.\n");
        return;
    } else if (num <= 0) {
        printf("Atencao: o preco maximo deve ser um numero inteiro maior que 0.\n");
        return;
    } else{
        ppr.msg.umarg.numero = num;
    }

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
}

void litime(char cmd[CMDSIZE]) {

    if (verificaArgumentos(cmd, LITIMESIZE)) {
        printf("Erro: Numero de argumentos errado, escreva 'help' para ajuda\n");
        return;
    }

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = LITIME;

    char *comando[LITIMESIZE];
    int num;

    separaArgumentos(cmd, comando, LITIMESIZE);

    if (sscanf(comando[1], "%d", &num) != 1) {
        printf("Atencao: a hora em segundos deve ser um numero inteiro maior que 0.\n");
        return;
    } else if (num <= 0) {
        printf("Atencao: a hora em segundos deve ser um numero inteiro maior que 0.\n");
        return;
    } else{
        ppr.msg.umarg.numero = num;
    }

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }

}

void timer(char cmd[CMDSIZE]) {
    #if DEBUG_CLIENTES_TIMER
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> TIME...\n");
        // =================================================
    #endif
    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = TIME;

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
}

void buy(char cmd[CMDSIZE]) {

    if (verificaArgumentos(cmd, BUYSIZE)) {
        printf("Erro: Numero de argumentos errado, escreva 'help' para ajuda\n");
        return;
    }

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = BUYCODE;

    char *comando[BUYSIZE];
    int num;


    separaArgumentos(cmd, comando, BUYSIZE);

    for (int i = 0; i < BUYSIZE; ++i) {

        switch (i) {
            case 1:
                if (sscanf(comando[i], "%d", &num) != 1) {
                    printf("Atencao: o id deve ser um numero inteiro.\n");
                    return;
                }else{
                    ppr.msg.buy.id = num;
                }
                break;

            case 2:
                if (sscanf(comando[i], "%d", &num) != 1) {
                    printf("Atencao: o valor deve ser um numero inteiro maior que 0.\n");
                    return;
                }
                if (num <= 0) {
                    printf("Atencao: o valor deve ser um numero inteiro maior que 0.\n");
                    return;
                }else{
                    ppr.msg.buy.valor = num;
                }
                break;

            default:
                break;
        }
    }


    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
}

void cash(char cmd[CMDSIZE]) {

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = CASH;

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }

}

void add(char cmd[CMDSIZE]) {

    if (verificaArgumentos(cmd, ADDSIZE)) {
        printf("Erro: Numero de argumentos errado, escreva 'help' para ajuda\n");
        return;
    }

    PREPARACAO ppr;
    ppr.pid = getpid();
    strcpy(ppr.key, "FrontEndKey");
    ppr.tipo = ADD;

    char *comando[ADDSIZE];
    int num;

    separaArgumentos(cmd, comando, ADDSIZE);

    if (sscanf(comando[1], "%d", &num) != 1) {
        printf("Atencao: o valor deve ser um numero inteiro maior que 0.\n");
        return;
    } else if (num <= 0) {
        printf("Atencao: o valor deve ser um numero inteiro maior que 0.\n");
        return;
    }else{
        ppr.msg.umarg.numero = num;
    }

    if( write(fdEnvio,&ppr,sizeof(PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }

}

int login(char userName[], char userPassword[]) {
    #if DEBUG_CLIENTES_LOGIN
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> INICIO LOGIN\n");
        // =================================================
    #endif
    PREPARACAO ppr;
    ppr.pid = getpid();
    ppr.tipo = LOGINCODE;
    strcpy(ppr.msg.login.userName, userName);
    strcpy(ppr.msg.login.password, userPassword);
    #if DEBUG_CLIENTES_LOGIN
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> lOGIN -> VAI ENVIAR MENSAGEM AO FIFO [%s]\n",SERVER_FIFO);
        // =================================================
    #endif
    if( write(fdEnvio,&ppr,sizeof (PREPARACAO)) == -1 ){
        perror("Erro a Enviar dados login");
        exit(1);
    }
    #if DEBUG_CLIENTES_LOGIN
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> lOGIN -> ENVIOU MENSAGEM AO FIFO [%s]\n",SERVER_FIFO);
        // =================================================
    #endif

    #if DEBUG_CLIENTES_LOGIN
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> lOGIN -> VAI RECEBER MENSAGEM DO FIFO [%s]\n",SERVER_FIFO);
        // =================================================
    #endif

    PREPARACAO response;
    do {
        if( read(fdResposta,&response,sizeof(PREPARACAO)) == -1 ){
            close(fdResposta);
            geral.stop = 1;
        }
        if (geral.stop == 1) {
            return 0;
        }
        #if DEBUG_CLIENTES_LOGIN
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> lOGIN -> RECEBEU MENSAGEM DO FIFO [%d][%s]\n",response.msg.confirmacao,SERVER_FIFO);
            // =================================================
        #endif
    } while(strcmp(response.key,SERVERKEY) != 0);
    return response.msg.confirmacao;
}

void *recebeMensagens() {
    do {
        PREPARACAO ppr;

        #if DEBUG_CLIENTE_RECEBE_MENSAGEM
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> RECEBEMENSAGENS -> VAI RECEBER MENSAGEM DO FIFO [%s]\n",SERVER_FIFO);
            // =================================================
        #endif

        if(read(fdResposta,&ppr,sizeof(PREPARACAO)) == -1 ){
        }

        if (geral.stop == 0) {
            if (!strcmp(ppr.key, SERVERKEY)){
                if (geral.stop == 0) {
                    #if DEBUG_CLIENTE_RECEBE_MENSAGEM
                        // ==================== DEBUG ======================
                        printf("\n[DEBUG] -> RECEBEMENSAGENS -> RECEBEU MENSAGEM DO FIFO [%s]\n",SERVER_FIFO);
                        // =================================================
                    #endif
                    printf("%s",ppr.msg.umarg.string);
                    /*if (ppr.tipo == FIM)
                        geral.stop = 1;*/
                }
            }
        }
        
    } while (geral.stop == 0);
    close(fdResposta);
    #if DEBUG_CLIENTE_RECEBE_MENSAGEM
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> CLOSE DE SERVIDOR [%s]\n",SERVER_FIFO);
        // =================================================
    #endif
}

void *enviaHeartbeats(){
    #if DEBUG_CLIENTE_ENVIA_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> VAI ABRIR FIFO SERVIDOR_HEARTBEATS [%s]\n",SERVER_HEARTBEATS);
        // =================================================
    #endif

    int fdEnvioHB = open(SERVER_HEARTBEATS, O_WRONLY);
    if (fdEnvioHB == -1) {
        printf("Erro a abrir para Heartbeats");
        return 1;
    }
    #if DEBUG_CLIENTE_ENVIA_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> ABRIU FIFO SERVIDOR_HEARTBEATS [%s]\n",SERVER_HEARTBEATS);
        // =================================================
    #endif
    do {
        PREPARACAO ppr;
        ppr.pid = getpid();
        ppr.tipo = HEARTBEAT;
        if( write(fdEnvioHB,&ppr,sizeof (PREPARACAO)) == -1 ){
        }
        #if DEBUG_CLIENTE_ENVIA_HEARTBEATS
            // ==================== DEBUG ======================
            printf("\n[DEBUG] -> ENVIOU HEARTBEAT AO SERVIDOR_HEARTBEATS [%s]\n",SERVER_HEARTBEATS);
            // =================================================
        #endif
        if (geral.stop == 0)
            sleep(heartbeats);

    } while (geral.stop == 0);
    

    close(fdEnvioHB);
    #if DEBUG_CLIENTE_ENVIA_HEARTBEATS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> CLOSE DE SERVIDOR_HEARTBEATS [%s]\n",SERVER_HEARTBEATS);
        // =================================================
    #endif
}

void recebeComando(const char user[30]) {
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    geral.stop = 0;
    geral.m = &mutex;
    COMANDOS coms;
    if (pthread_create (&coms.tRecebe,NULL,&recebeMensagens,NULL)!=0)
        exit(1);//sigqueue( getpid(), SIGINT, geral.intMain );//return -1;
    if (pthread_create (&coms.tHeartbeats,NULL,&enviaHeartbeats,NULL)!=0)
        exit(1);//sigqueue( getpid(), SIGINT, geral.intMain );//return -1;
    char inpt[CMDSIZE];
    char nomeCmd[CMDSIZE];
    #if DEBUG_CLIENTE_COMANDOS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> STOP.GERAL [%d]\n",geral.stop);
        // =================================================
    #endif
    do {
        printf("SOBay-%s:  ", user);
        fgets(inpt, CMDSIZE, stdin);//recebe comando do utilzador
        if (geral.stop == 0) {
            strcpy(nomeCmd, inpt);// copia o comando para o nomeCmd
            strtok(nomeCmd, " "); //remove todas as palavras menos a primeira do nomeCmd
            nomeCmd[strcspn(nomeCmd, "\n")] = 0;
            #if DEBUG_CLIENTE_COMANDOS
                // ==================== DEBUG ======================
                printf("\n[DEBUG] -> COMANDO [%s]\n",nomeCmd);
                printf("\n[DEBUG] -> COMANDO [%s]\n",nomeCmd);
                // =================================================
            #endif
            if (!strcmp(nomeCmd, "sell")) {
                sell(inpt);
            } else if (!strcmp(nomeCmd, "list")) {
                list();
            } else if (!strcmp(nomeCmd, "licat")) {
                licat(inpt);
            } else if (!strcmp(nomeCmd, "lisel")) {
                lisel(inpt);
            } else if (!strcmp(nomeCmd, "lival")) {
                lival(inpt);
            } else if (!strcmp(nomeCmd, "litime")) {
                litime(inpt);
            } else if (!strcmp(nomeCmd, "time")) {
                #if DEBUG_CLIENTES_TIMER
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> TIME...\n");
                    // =================================================
                #endif
                timer(inpt);
            } else if (!strcmp(nomeCmd, "buy")) {
                buy(inpt);
            } else if (!strcmp(nomeCmd, "cash")) {
                cash(inpt);
            } else if (!strcmp(nomeCmd, "add")) {
                add(inpt);
            } else if (!strcmp(nomeCmd, "help")) {
                help();
            } else if (strcmp(nomeCmd,"exit")) {
                    printf("Comando invalido, escreva 'help' para obter ajuda.\n\n");
            }
                #if DEBUG_CLIENTE_COMANDOS
                    // ==================== DEBUG ======================
                    printf("\n[DEBUG] -> STOP.GERAL [%d]\n",geral.stop);
                    // =================================================
                #endif
        }
    } while (strcmp(nomeCmd, "exit") != 0 && geral.stop == 0);

    close(fdEnvio);

    geral.stop = 1;

    #if DEBUG_CLIENTE_COMANDOS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> FECHAR HEARTBEATS\n");
        // =================================================
    #endif
    pthread_kill(coms.tHeartbeats, SIGUSR1);
    if (pthread_join(coms.tHeartbeats, NULL)!=0)
        exit(1);
    #if DEBUG_CLIENTE_COMANDOS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> HEARTBEATS FECHADOS\n");
        // =================================================
    #endif
    #if DEBUG_CLIENTE_COMANDOS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> FECHAR THREAD QUE RECEBE MENSAGENS\n");
        // =================================================
    #endif
    pthread_kill(coms.tRecebe, SIGUSR1);
    if (pthread_join(coms.tRecebe, NULL)!=0)
        exit(1);
    #if DEBUG_CLIENTE_COMANDOS
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> COMANDOS -> THREAD QUE RECEBE MENSAGENS FECHADA\n");
        // =================================================
    #endif
}

int main(int argc, char **argv) {

    if (getenv("HEARTBEAT") == NULL) {
        // ==================== ERROR ======================
        printf("\n[ERROR] -> VARIAVEL AMBIENTE DOS HEARTBEATS NAO DECLARADA\n");
        exit(1);
        // =================================================
    }
    sscanf(getenv("HEARTBEAT"),"%d",&heartbeats);

    setbuf(stdout, NULL);
    printf("\nPID[%d]\n", getpid());

    struct sigaction sa;
    sa.sa_sigaction = sair;
    sigaction(SIGINT, &sa, NULL);

    struct sigaction act;
    act.sa_sigaction = acorda;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    geral.stop = 0;


    if (argc < 3 || argc > 3) {
        printf("Erro: Numero de argumentos errado! \nInsira o nome e a password para fazer login\n");
        exit(1);
    }

    sprintf( SERVIDOR_CLIENTE_FINAL, CLIENT_FIFO, getpid() );


    if(mkfifo(SERVIDOR_CLIENTE_FINAL,0666) == -1)
    {
        if(errno == EEXIST)
        {
            printf("FIFO ja existe\n");
        }
        printf("Erro abrir FIFO\n");
        terminaClienteInvalido();
    }

    #if DEBUG_CLIENTE_CRIA_FIFO
    // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIFO [%s] CLIENTE CRIADO\n", SERVIDOR_CLIENTE_FINAL);
    // =================================================
    #endif

    #if DEBUG_SERVIDOR_FIFO_ABRIR
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> VAI ABRIR FIFO SERVIDOR [%s]\n",SERVER_FIFO);
        // =================================================
    #endif
    fdEnvio = open(SERVER_FIFO, O_WRONLY);
    if (fdEnvio == -1) {
        printf("Erro a abrir o Servidor");
        terminaClienteInvalido();
    }
    #if DEBUG_SERVIDOR_FIFO_ABRIR
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> ABRIU FIFO SERVIDOR [%s]\n",SERVER_FIFO);
        // =================================================
    #endif

    #if DEBUG_CLIENTE_FIFO_ABRIR
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> VAI ABRIR FIFO CLIENTE [%s]\n",SERVIDOR_CLIENTE_FINAL);
        // =================================================
    #endif
    fdResposta = open(SERVIDOR_CLIENTE_FINAL, O_RDWR);
    if (fdResposta == -1) {
        printf("Erro a abrir o Cliente");
        terminaClienteInvalido();
    }
    #if DEBUG_CLIENTE_FIFO_ABRIR
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> ABRIU FIFO CLIENTE [%s]\n",SERVIDOR_CLIENTE_FINAL);
        // =================================================
    #endif


    char user[30];
    char password[30];

    strcpy(user, argv[1]);
    strcpy(password, argv[2]);

    while(!login(user, password) && geral.stop == 0){
        printf("Nome de utilizador ou password Errada!\n");
        printf("Nome de utilizador: ");
        gets(user);
        printf("Password: ");
        gets(password);
    }

    #if DEBUG_CLIENTES_LOGIN
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> LOGIN COM SUCESSO\n");
        // =================================================
    #endif

    if (geral.stop == 0) {
        printf("----------------- SOBay -----------------\n");
        recebeComando(user);
    }
    

    unlink(SERVIDOR_CLIENTE_FINAL);
    #if DEBUG_TERMINA_FIFO_SERVIDOR
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIFO [%s] TERMINADO\n", SERVIDOR_CLIENTE_FINAL);
        // =================================================
    #endif

    #if DEBUG_FIM
        // ==================== DEBUG ======================
        printf("\n[DEBUG] -> FIM PROGRAMA\n");
        // =================================================
    #endif

    return 0;
}
