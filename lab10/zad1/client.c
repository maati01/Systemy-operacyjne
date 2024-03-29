#include "common.h"

int server_socket;
int is_client_O;
char buff[MAX_MESSAGE + 1];
char *name, *command, *arg;
game_board board;
state current_state = START;

pthread_mutex_t mutex_t = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_t = PTHREAD_COND_INITIALIZER;

void parse_command(char* msg){
    command = strtok(msg, ":");
    arg = strtok(NULL, ":");
}

game_board create_board(){
    game_board board = {1,{FREE}};
    return board;
}

int move(game_board *board, int position){
    if (position < 0 || position > 9 || board->objects[position] != FREE) return 0;

    board->objects[position] = board->move ? O : X;
    board->move = !board->move;
    return 1;
}

sign check_winner(game_board *board){
    sign column = FREE;

    for (int x = 0; x < 3; x++){
        sign first = board->objects[x];
        sign second = board->objects[x + 3];
        sign third = board->objects[x + 6];

        if (first == second && first == third && first != FREE)
            column = first;
    }

    if (column != FREE)
        return column;

    sign row = FREE;
    for (int y = 0; y < 3; y++){
        sign first = board->objects[3 * y];
        sign second = board->objects[3 * y + 1];
        sign third = board->objects[3 * y + 2];
        if (first == second && first == third && first != FREE)
            row = first;
    }

    if (row != FREE)
        return row;

    sign low_diagonal = FREE;

    sign first = board->objects[0];
    sign second = board->objects[4];
    sign third = board->objects[8];

    if (first == second && first == third && first != FREE) low_diagonal = first;
    if (low_diagonal != FREE) return low_diagonal;

    sign up_diagonal = FREE;
    first = board->objects[2];
    second = board->objects[4];
    third = board->objects[6];
    if (first == second && first == third && first != FREE)
        up_diagonal = first;

    return up_diagonal;
}

void finish(){
    char buff[MAX_MESSAGE + 1];
    send(server_socket, buff, MAX_MESSAGE, 0);
    exit(EXIT_SUCCESS);
}

void check_game_status(){
    int win = 0;
    sign winner = check_winner(&board);

    if (winner != FREE){
        if ((is_client_O && winner == O) || (!is_client_O && winner == X)) printf("You won!\n");
        else printf("You lost!\n");

        win = 1;
    }

    int draw = 1;
    for (int i = 0; i < 9; i++){
        if (board.objects[i] == FREE){
            draw = 0;
            break;
        }
    }

    if (draw && !win) printf("Draw!\n");

    if (win || draw) current_state = QUIT;
}


void draw_state(){
    char val;

    for (int y = 0; y < 3; y++){
        for (int x = 0; x < 3; x++){
            if (board.objects[y * 3 + x] == FREE) val = y * 3 + x + 1 + '0';
            else if (board.objects[y * 3 + x] == O) val = 'O';
            else val = 'X';

            printf("  %c  ", val);
        }
        printf("\n___________________\n");
    }
}

void start_game(){
    while (1){
        if (current_state == START){
            if (strcmp(arg, "name_taken") == 0) exit(EXIT_FAILURE);
            else if (strcmp(arg, "no_enemy") == 0) current_state = WAIT_FOR_OPPONENT;
            else{
                board = create_board();
                is_client_O = arg[0] == 'O';
                current_state = is_client_O ? MOVE : WAIT_FOR_MOVE;
            }
        }

        else if (current_state == WAIT_FOR_OPPONENT){

            pthread_mutex_lock(&mutex_t);

            while (current_state != START && current_state != QUIT) pthread_cond_wait(&cond_t, &mutex_t);

            pthread_mutex_unlock(&mutex_t);

            board = create_board();
            is_client_O = arg[0] == 'O';
            current_state = is_client_O ? MOVE : WAIT_FOR_MOVE;
        }
        else if (current_state == WAIT_FOR_MOVE){
            pthread_mutex_lock(&mutex_t);
            while (current_state != OPPONENT_MOVE && current_state != QUIT){
                pthread_cond_wait(&cond_t, &mutex_t);
            }
            pthread_mutex_unlock(&mutex_t);
        }
        else if (current_state == OPPONENT_MOVE){
            int pos = atoi(arg);
            move(&board, pos);
            check_game_status();
            if (current_state != QUIT){
                current_state = MOVE;
            }
        }
        else if (current_state == MOVE){
            draw_state();

            int pos;
            do{
                printf("Select a cell: ");
                scanf("%d", &pos);
                pos--;
            } while (!move(&board, pos));

            draw_state();

            char buff[MAX_MESSAGE + 1];
            sprintf(buff, "move:%d:%s", pos, name);
            send(server_socket, buff, MAX_MESSAGE, 0);

            check_game_status();
            if (current_state != QUIT){
                current_state = WAIT_FOR_MOVE;
            }
        }
        else if (current_state == QUIT) finish();
    }
}


void listen_server()
{
    int game_thread_running = 0;
    while (1)
    {
        recv(server_socket, buff, MAX_MESSAGE, 0);
        parse_command(buff);

        pthread_mutex_lock(&mutex_t);
        if (strcmp(command, "add") == 0)
        {
            current_state = START;
            if (!game_thread_running)
            {
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *))start_game, NULL);
                game_thread_running = 1;
            }
        }
        else if (strcmp(command, "move") == 0)
        {
            current_state = OPPONENT_MOVE;
        }
        else if (strcmp(command, "end") == 0)
        {
            current_state = QUIT;
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(command, "ping") == 0)
        {
            sprintf(buff, "pong: :%s", name);
            send(server_socket, buff, MAX_MESSAGE, 0);
        }
        pthread_cond_signal(&cond_t);
        pthread_mutex_unlock(&mutex_t);
    }
}


void connect_local(char* path){
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, path);
    
    check_error(connect(server_socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)), "Connect error");
}

void connect_inet(char* port){
    struct addrinfo *info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("localhost", port, &hints, &info);

    server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    check_error(connect(server_socket, info->ai_addr, info->ai_addrlen), "Connect error");

    freeaddrinfo(info);
}

void handle_input(char** argv){
    if (strcmp(argv[2], "unix") == 0){
        char* path = argv[3];
        connect_local(path);
    }

    else if (strcmp(argv[2], "inet") == 0){
        char* port = argv[3];
        connect_inet(port);
    }

    else{
        printf("Wrong arguments!");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]){
    check_error((argc == 4), "Wrong number of arguments!");

    name = argv[1];
    signal(SIGINT, finish);

    handle_input(argv);

    char msg[MAX_MESSAGE];
    sprintf(msg, "add: :%s", name);
    send(server_socket, msg, MAX_MESSAGE, 0);

    listen_server();

    return 0;
}