#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <list>

#include <string>
using std::string;
using std::stoul;

#include "../include/text_color.h"

#include <exception>
using std::invalid_argument;
using std::out_of_range;

#define BOARD_SIZE 9

struct Move { //Guarda as informações de cada ação de remover ou colocar um número no tabuleiro
    enum Command {//Determina qual tipo de jogada foi feita
      REMOVE = 0,
      PLACE 
    };

    Command action; 
    int row_played;//Linha em que ocorreu a jogada
    int column_played;//Coluna em que ocorreu a jogada
    int number_played;//Valor que foi alterado na jogada
};

struct Board //Struct utilizado como tabuleiro
    {   
        enum Id {//Identifica os diferentes estados de cada posição
        FIXED = 0, //Valor que não deve ser alterado
        VALID, //Posição com um número válido
        INVALID, //Posição com um úmero inválido
        EMPTY //Posição ainda sem ser valor 
    };
        int value; // Número/valor da posição
        Id position_status;//Identifica o status de cada posição
};

struct BoardSet{ //Struct que guarda os diferentes tabuleiro a serem utilizados no jogo
    Board boards[BOARD_SIZE][BOARD_SIZE];
};

struct GameManager { // 

    
    enum GameState {//Identifica os diferentes estados do jogo
        STARTING = 0, //Jogo iniciou
        HELPING, // Usuário digitou o comando help
        CHECKING_MOVES, // Usuário digitou o comando de check
        FINISHED_PUZZLES, // Usuário acertou todos as posições vazias
        READING_MAIN_OPTION, //Programa esperando a entrada do usuário no menu principal
        PLAYING_MODE, //Programa esperando a entrada do usuário no modo de jogo
        CONFIRMING_QUITTING_MATCH, //Usuário digitou o comando new_game com um jogo já iniciado
        QUITTING_MATCH, //Usuário está saindo do jogo
        UNDOING_PLAY,   // Usuário digitou o comando undo para desfazer a ultima jogada
        INVALID_INPUT_MAIN, //Usuário digitou um comando inválido no menu principal
        INVALID_INPUT_PLAY, //Usuário digitou um comando inválido no modo de jogo
        PLACING_NUMBER, // Usuário digitou o comando place para colocar um numero em uma nova posição
        REMOVING_NUMBER, //Usuário digitou o comando remove para remover um numero de uma posição
        NEW_GAME, //Usuário digitou o comando new_game para receber um novo tabuleiro
        BACK_TO_MENU //Usuário voltou ao menu principal após já estar no modo de jogo
    };
        
    
    //== Public members
    GameState game_state; // Guarda os diferentes estados do jogo
    int n_checks; // armazena o numero de checks disponíveis
    string current_message; //Guarda a mensagem a ser exibida para o usuário
    BoardSet board_answer[100]; //Guarda todos os tabuleiros gabarito
    BoardSet board_playable[100]; //Guarda todos os tabuleiros jogáveis
    std::list <Move> undo_list; // Guarda todos os removes e place realizados
    char user_cmd; //Guarda o comando digitado pelo usuário
    string confirmation_cmd; //Guarda o comando de confirmação de novo jogo do usário
    int row_index; //Guarda o índice da linha do tabuleiro
    int column_index;//Guarda o índice da coluna do tabuleiro
    int number_value; //Guarda o valor do número no tabuleiro
    int main_menu_option; //Guarda o opção escolhida no menu principal
    int digits[10] = {0}; //Guarda os digitos disponíveis no tabuleiro
    int board_counter = -1; // Responsável por determinar qual tabuleiro está sendo usado
    int board_counter_limit = -1; // Responsável por determinar qual é o momento de começar a reexibir tabuleiros
    bool has_already_played = false; // Indica se o tabuleiro já foi alterado pelo usuário
    bool is_starting = true; // Indica se é a primeira rodada do jogo



    //== Public methods
    void initialize();
    void process_events(void);
    void update(void);
    void render(void);
    bool quit_game(void) { return game_state == GameState::QUITTING_MATCH; }



    //== Aux methods
    void generate_board_answer ();// Lê um arquivo de texto e guarda os tabuleiros gabarito
    void generate_board_playable();// Cria os tabuleiros jogáveis a partir dos tabuleiros gabarito
    bool do_check (int row_check, int column_check); //Verifica se o número está na posição correta
    void place(int row, int column, int number_value); //Coloca uma peça no tabuleiro, caso não seja uma posição fixada
    void remove(int row, int column); // Remove uma peça do tabuleiro, caso não seja uma posição fixada
    void undo(); //Desfaz a última jogada, caso exista uma jogada anterior
    void print_welcome();//Imprime a mensagem de bem vindo
    void print_help();// Imprime a mensagem com as instruções do jogo
    void print_board(); // Imprime tabuleiro
    string reading_main_menu_option(); //Lê o comando do menu principal
    string reading_playing_menu_option(); //Lê o comando do modo de jogo
    bool is_valid(int row, int column, int number_value); // Identifica se um número é válido para aquela posição
    void digits_left(); //Identifica quais numeros ainda podem ser jogados
    void print_digits_left(); // Imprime quais numeros ainda podem ser jogados
    void print_msg(); // Imprime mensagem para o usuário, com o feedback da última jogada
    void print_command_syntax(); // Imprime os comandos que podem ser utilizados pelo usuário no modo de jogo
    void print_number_check(); // Imprime os números de check que podem ser realizados pelo usuário
    void print_check_board();  // Imprime o tabuleiro quando o jogo está no estado de CHECK
    void print_main_menu(); // Imprime o menu principal
    bool new_game(); // Troca o tabuleiro jogável, caso o tabuleiro atual não sido alterado
    std::string tokenizer(string line); //Retira os espaços em branco do comando do usuário e os separa para que eles possam servir de parâmetro para outras funções
    bool confirm_quitting_match(string line); //Solicita a confirmação do usuário, i. e., se ele quer trocar um tabuleiro que já foi alterado
    void print_confirm_quitting_match(); // Imprime na tela a mensagem de confirmação de troca do tabuleiro
    void emptying_board(); // Esvazia todas as posições já alteradas de um tabuleiro, para que o usuário possa recomeçá-lo
    bool is_finished(); //Determina se todas as posições do tabuleiro estão com valores válidos
    void print_board_not_fixed();

};

void GameManager::initialize()
{
   game_state = GameState::STARTING;
   n_checks = 3;  
}

bool GameManager::confirm_quitting_match(string line){
    if(line == "y" or line == "Y"){
        return true;
    } else {
        return false;
    }
}

bool GameManager::is_finished(){
    for(int row{0}; row < 9; row++){
        for(int col{0}; col < 9; col++){
            if(board_playable[board_counter].boards[row][col].value != board_answer[board_counter].boards[row][col].value){
                return false;
            }
        }
    }
    return true;
}

void GameManager::print_confirm_quitting_match(){
    std::cout << "\n\nYour choice [y/N] >";
}

bool GameManager::is_valid(int row, int column, int number_value){
    int number = 0;

    for(int i{0}; i < 9; i++){
        if(board_playable[board_counter].boards[row][i].value == number_value) number++;
        if(number > 1) return false;
    }
    number = 0;

    for(int j{0}; j < 9; j++){
        if(board_playable[board_counter].boards[j][column].value == number_value) number++;
        if(number > 1) return false;
    }
    
    number = 0;

    if(column >= 0 && column < 3){
        for(int col1{0}; col1 < 3; col1++){
            if(row >= 0 && row < 3){
                for(int row1{0}; row1 < 3; row1++){
                    if(board_playable[board_counter].boards[row1][col1].value == number_value) number++;
                }
                if(number > 1) return false;

            } else  if(row >= 3 && row < 6) {
                for(int row2{3}; row2 < 6; row2++){
                    if(board_playable[board_counter].boards[row2][col1].value == number_value) number++;
                }
                if(number > 1) return false;

            } else  if(row >= 6 && row < 9) {
                for(int count3{6}; count3 < 9; count3++){
                    if(board_playable[board_counter].boards[count3][col1].value == number_value) number++;
                }
                if(number > 1) return false;
            }
        }

    } else if(column >= 3 && column < 6){

        for(int col2{3}; col2 < 6; col2++){
            if(row >= 0 && row < 3){
                for(int row4{0}; row4 < 3; row4++){
                    if(board_playable[board_counter].boards[row4][col2].value == number_value) number++;
                }
                if(number > 1) return false;

            } else  if(row >= 3 && row < 6) {
                for(int row5{3}; row5 < 6; row5++){
                    if(board_playable[board_counter].boards[row5][col2].value == number_value) number++;
                }
                if(number > 1) return false;

            } else  if(row >= 6 && row < 9) {
                for(int row6{6}; row6 < 9; row6++){
                    if(board_playable[board_counter].boards[row6][col2].value == number_value) number++;
                }
                if(number > 1) return false;
            }
        }

    } else if(column >= 6 && column < 9) {

        for(int col3{6}; col3 < 9; col3++){
            if(row >= 0 && row < 3){
                for(int row7{0}; row7 < 3; row7++){
                    if(board_playable[board_counter].boards[row7][col3].value == number_value) number++;
                }
                if(number > 1) return false;

            } else  if(row >= 3 && row < 6) {
                for(int row8{3}; row8 < 6; row8++){
                    if(board_playable[board_counter].boards[row8][col3].value == number_value) number++;
                }
                if(number > 1) return false;

            } else  if(row >= 6 && row < 9) {
                for(int row9{6}; row9 < 9; row9++){
                    if(board_playable[board_counter].boards[row9][col3].value == number_value) number++;
                }
                if(number > 1) return false;
            }
        }
    }

    if(number == 1) {
        return true;
    } else {
        return false;
    }

}


bool GameManager::new_game()
{
    if(!(has_already_played)){
        if(board_counter < board_counter_limit){
            board_counter++;

        } else {
            board_counter = 0;
        }
        emptying_board();
        return true;
    } else {
        return false;
    }
}

std::string GameManager::tokenizer(string line){
    std::string tokens;
    std::string aux;
    for (auto it = line.cbegin() ; it != line.cend(); ++it) {
        aux.push_back(*it);
        if(*it == ' '){
          aux.clear();
        } else {
            tokens.append(aux);
            aux.clear();
        }
    }

    return tokens;
}

void GameManager::print_help(){
    std::cout << Color::tcolor("\n\n--------------------------------------------------------------------------\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << Color::tcolor("  The goal of Sudoku is to fill a 9x9 grid with number so that each row,\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << Color::tcolor("  column and 3x3 section (nonet) contain all of the digits between 1 and 9.\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << Color::tcolor("\n  The Sudoku rules are:\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << Color::tcolor("  1. Each row, column, and nonet can contain each number (typically 1 to 9)\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << Color::tcolor("     exactly once.\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << Color::tcolor("  2. The sum of all numbers in any nonet, row, or column must be equal to 45.\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << Color::tcolor("--------------------------------------------------------------------------\n", Color::BRIGHT_YELLOW, Color::REGULAR);
    std::cout << "\nPress enter to go back\n";
}
  
void GameManager::print_command_syntax(){
    std::cout << Color::tcolor( "\nCommands syntax:", Color::BRIGHT_GREEN, Color::BOLD);
    std::cout << Color::tcolor("\n   \'enter\' (without typing anything)    -> go back to previous menu.",  Color::BRIGHT_GREEN, Color::BOLD);
    std::cout << Color::tcolor("\n   \'p\' <row> <col> + \'enter\' -> place <number> on board at location (<row>, <col>).", Color::BRIGHT_GREEN, Color::BOLD);
    std::cout << Color::tcolor("\n   \'r\' <row> <col> <number> + \'enter\' -> remove number on board at location (<row>, <col>).", Color::BRIGHT_GREEN, Color::BOLD);
    std::cout << Color::tcolor("\n   \'c\' + \'enter\'                      -> check which moves made are correct.", Color::BRIGHT_GREEN, Color::BOLD);
    std::cout << Color::tcolor("\n   \'u\' + \'enter\'                      -> undo last play.", Color::BRIGHT_GREEN, Color::BOLD);
    std::cout << Color::tcolor("\n   <row> <col> <number> must be in range [1, 9]", Color::BRIGHT_GREEN, Color::BOLD);
    std::cout << Color::tcolor("\n\nEnter command >", Color::BRIGHT_GREEN, Color::BOLD);
}

void GameManager::print_number_check(){
    std::cout << Color::tcolor("\nChecks left: [ ",Color::BRIGHT_YELLOW, Color::REGULAR) << Color::tcolor(std::to_string(n_checks), Color::BRIGHT_YELLOW, Color::REGULAR)<< Color::tcolor(" ]",Color::BRIGHT_YELLOW, Color::REGULAR);
}

void GameManager::print_main_menu(){
    std::cout << "\n  1 - Play  2 - New Game  3 - Quit  4 - Help";
    std::cout << "\n  Select option [1,4] > ";
}

bool GameManager::do_check (int row_check, int column_check)
{
    if (board_answer[board_counter].boards[row_check][column_check].value == board_playable[board_counter].boards[row_check][column_check].value){
        return true;
    } else {
        return false;
    }
}

void GameManager::print_check_board(){
    std::cout << "\n" <<Color::tcolor("\n     1 2 3   4 5 6   7 8 9  \n", Color::CYAN, Color::REGULAR) << "\n";
    std::cout << "  +-------+-------+-------+\n";
    for(int i{0}; i < 9; i++){
        if(i == 3 || i == 6) std::cout << "  +-------+-------+-------+\n";

        for(int j{0}; j< 9; j++){
            if(j == 0) {
                     std::cout <<  " " <<  Color::tcolor(std::to_string(i+1), Color::CYAN, Color::REGULAR) << " ";
            }
            if(j == 0 || j == 3 || j == 6) std::cout << "| ";
            if(board_playable[board_counter].boards[i][j].value > 0) {

                if(board_playable[board_counter].boards[i][j].position_status == Board::FIXED){
                    std::cout << Color::tcolor(std::to_string(board_playable[board_counter].boards[i][j].value), Color::WHITE, Color::BOLD) << " ";
                } else if(board_playable[board_counter].boards[i][j].position_status == Board::INVALID) {
                    std::cout << Color::tcolor(std::to_string(board_playable[board_counter].boards[i][j].value), Color::RED, Color::REGULAR) << " ";
                } else if(board_playable[board_counter].boards[i][j].position_status == Board::VALID) {
                    if(do_check(i, j)){
                        std::cout << Color::tcolor(std::to_string(board_playable[board_counter].boards[i][j].value), Color::GREEN, Color::REGULAR) << " ";
                    } else {
                        std::cout << Color::tcolor(std::to_string(board_playable[board_counter].boards[i][j].value), Color::RED, Color::REGULAR) << " ";
                    }
                }
            
            }
             else std::cout << "  ";
        }
        std::cout << "|";
        std::cout << "\n";
    }
    std::cout << "  +-------+-------+-------+";
    current_message = "Checking done! Press enter to continue";
    n_checks--;
}

void GameManager::emptying_board(){
    for(int row{0}; row < 9; row++){
        for(int col{0}; col < 9; col++){
            if(board_playable[board_counter].boards[row][col].position_status == Board::INVALID || board_playable[board_counter].boards[row][col].position_status == Board::VALID){
                board_playable[board_counter].boards[row][col].position_status = Board::EMPTY;
                board_playable[board_counter].boards[row][col].value = 0;
            }
        }
    }

}

void GameManager::print_board()
{   
    string columns_grid = "     1 2 3   4 5 6   7 8 9  ";
    //Identifica se o valor de column_index está entre 1 e 9, valor só é alterado quando o input é válido e no início de process_events é zerado, assim como o row_index
    if(column_index > 0 && column_index < 10) {
        int found = columns_grid.find_first_of(std::to_string(column_index - 1));
        for(int blank_counter{1}; blank_counter <= found; blank_counter++){
            std::cout << " ";
        }
        std::cout << Color::tcolor("V", Color::BRIGHT_YELLOW, Color::REGULAR);
    }

    std::cout << "\n" <<Color::tcolor(columns_grid, Color::CYAN, Color::REGULAR) << "\n";
    std::cout << "   +-------+-------+-------+\n";
    for(int i{0}; i < 9; i++){
        if(i == 3 || i == 6) std::cout << "   +-------+-------+-------+\n";

        for(int j{0}; j< 9; j++){
            if(j == 0) {
                if((row_index - 1) == i){ 
                    std::cout <<  Color::tcolor(">", Color::BRIGHT_YELLOW, Color::REGULAR) <<  Color::tcolor(std::to_string(i+1), Color::CYAN, Color::REGULAR) << " ";
                }
                else {
                     std::cout <<  " " <<  Color::tcolor(std::to_string(i+1), Color::CYAN, Color::REGULAR) << " ";
                }
            }
            if(j == 0 || j == 3 || j == 6) std::cout << "| ";
            if(board_playable[board_counter].boards[i][j].value > 0) {

                if(board_playable[board_counter].boards[i][j].position_status == Board::FIXED){
                    std::cout << Color::tcolor(std::to_string(board_playable[board_counter].boards[i][j].value), Color::WHITE, Color::BOLD) << " ";
                } 
                else if (board_playable[board_counter].boards[i][j].position_status == Board::VALID){
                    std::cout << Color::tcolor(std::to_string(board_playable[board_counter].boards[i][j].value), Color::BLUE, Color::REGULAR) << " ";
                } else if (board_playable[board_counter].boards[i][j].position_status == Board::INVALID){
                    std::cout << Color::tcolor(std::to_string(board_playable[board_counter].boards[i][j].value), Color::RED, Color::REGULAR) << " ";
                }
            }
             else std::cout << "  ";
        }
        std::cout << "|";
        std::cout << "\n";
    }
    std::cout << "   +-------+-------+-------+";

}


void GameManager::digits_left()
{
    for(int i{0}; i < 9; i++){
        for(int j{0}; j < 9; j++){
            int count = board_playable[board_counter].boards[i][j].value;
            digits[count]++;
        }
    }
}

void GameManager::print_digits_left(){
    digits_left();
    std::cout <<  Color::tcolor("\nDigits left: [ ", Color::YELLOW, Color::BOLD);
    for(int i{1}; i < 10; i++)
    {
        if(digits[i] < 9) std::cout << Color::tcolor(std::to_string(i), Color::YELLOW, Color::BOLD) << " ";
        digits[i] = 0;
    }
    std::cout << Color::tcolor("]", Color::YELLOW, Color::BOLD);
}

void GameManager::print_msg()
{
    std::cout << Color::tcolor("\nMSG: [ ", Color::YELLOW, Color::BOLD);
    std::cout << Color::tcolor(current_message, Color::YELLOW, Color::BOLD);
    std::cout << Color::tcolor(" ]\n", Color::YELLOW, Color::BOLD);
    current_message.clear();

}

string GameManager::reading_main_menu_option(){
    std::string line;
    std::getline(std::cin, line);  

    return line;
}

string GameManager::reading_playing_menu_option(){
    std::string line;
    std::getline(std::cin, line);
    
    return line;
}


void GameManager::print_welcome(void)
{
    std::cout << "================================================\n";
    std::cout << "  Welcome to a terminal version of Sudoku, v1.0 \n";
    std::cout << "  Copyriht (C) 2020, Victor G. S. de Castro     ";
    std::cout << "\n================================================\n";
    std::cout << "\n\nPress enter to start.";

}



void GameManager::generate_board_answer()
{
    string file = "../data/input.txt";
    std::ifstream myFile(file);

    if(myFile.is_open()){
        string line;
        if(game_state == GameState::STARTING){
            std::cout << ">>> Opening input file [" << file << "].\n";
            std::cout << ">>> Processing data, please wait.\n";
            std::cout << ">>> Finished reading input data file.\n\n\n";
        }
        while(!(myFile.eof())){
            int count_row = 0;
            GameManager::board_counter++;
            GameManager::board_counter_limit++;

            while(count_row < 10) { //percorre 10 linhas do arquivo de entrada, as nove primeiras linhas são do sudoku e a ultima linha é vazia
                    getline(myFile, line);
                    int count_column = 0;
                    std::string number_saver;
                    std::string aux;
                    for (auto it = line.cbegin() ; it != line.cend(); ++it) {
                            aux.push_back(*it);
                            if(*it != ' '){
                                number_saver.append(aux);
                                aux.clear();
                            } else if(*it == ' ' && !(number_saver.empty())) {
                                GameManager::board_answer[board_counter].boards[count_row][count_column].value = stoi(number_saver);
                                GameManager::board_answer[board_counter].boards[count_row][count_column].position_status = Board::FIXED;
                                
                                if(GameManager::board_answer[board_counter].boards[count_row][count_column].value > 0) { //Adiciona as posições fixadas a tabela jogavel
                                    board_playable[board_counter].boards[count_row][count_column].value = GameManager::board_answer[board_counter].boards[count_row][count_column].value;
                                    board_playable[board_counter].boards[count_row][count_column].position_status = Board::FIXED;

                                } else if(GameManager::board_answer[board_counter].boards[count_row][count_column].value < 0) { //Adiciona as posições vazias na tabela jogavel
                                    board_playable[board_counter].boards[count_row][count_column].value = 0;
                                    board_playable[board_counter].boards[count_row][count_column].position_status = Board::EMPTY;

                                    GameManager::board_answer[board_counter].boards[count_row][count_column].value *= -1; //Atualiza os valores na tabela gabarito, para que todas fiquem positivas
                                }


                                count_column++;
                                number_saver.clear();
                                aux.clear();
                            }

                        }      
                count_row++;                
            }
        }
        
    } else {
        std::cout << "Unable to open file";
        game_state = GameState::QUITTING_MATCH;
    }
    GameManager::board_counter = 0;

    myFile.close();

}

void GameManager::place(int row, int column, int number_value){
    if(GameManager::board_playable[board_counter].boards[row][column].position_status == Board::FIXED){ //Caso seja uma posição fixada, entra em estado de entrada inválida
        game_state = GameState::INVALID_INPUT_PLAY;
        current_message = "You can't change this number";
        return;
    } else {
        GameManager::board_playable[board_counter].boards[row][column].value = number_value;
        if(is_valid(row, column, number_value)){
            GameManager::board_playable[board_counter].boards[row][column].position_status = Board::VALID;
        } else {
            GameManager::board_playable[board_counter].boards[row][column].position_status = Board::INVALID;
        }
        current_message = "New number placed sucessfully!";
        if(game_state != GameState::UNDOING_PLAY){
            undo_list.push_back({Move::Command::PLACE, row, column, 0});
        }
    }
    
}

void GameManager::remove(int row, int column){
    if(GameManager::board_playable[board_counter].boards[row][column].position_status == Board::FIXED){
        game_state = GameState::INVALID_INPUT_PLAY;
        current_message = "You can't remove this number";
        return;
    } else {
        int undo_value = GameManager::board_playable[board_counter].boards[row][column].value;
        GameManager::board_playable[board_counter].boards[row][column].value = 0;
        GameManager::board_playable[board_counter].boards[row][column].position_status = Board::EMPTY;
        current_message = "Number removed sucessfully!";
        if(game_state != GameState::UNDOING_PLAY){
            undo_list.push_back({Move::Command::REMOVE, row, column, undo_value});
        }
    }
}

void GameManager::undo()
{
    if(undo_list.back().action == Move::Command::PLACE) {
        remove(undo_list.back().row_played, undo_list.back().column_played);
        
    } else {
        place(undo_list.back().row_played, undo_list.back().column_played, undo_list.back().number_played);
    }
    undo_list.pop_back();
} 

void GameManager::generate_board_playable()
{   
    for(auto board{0}; board <= board_counter_limit; board++){
        for(auto i{0}; i < BOARD_SIZE; i++){
            for(auto j{0}; j < BOARD_SIZE; j++){
                if(GameManager::board_answer[board].boards[i][j].value > 0) {
                    GameManager::board_playable[board].boards[i][j].value = GameManager::board_answer[board].boards[i][j].value;
                    GameManager::board_playable[board_counter].boards[i][j].position_status = Board::FIXED;
                }
                else if (GameManager::board_answer[board].boards[i][j].value < 0) {
                    GameManager::board_playable[board].boards[i][j].value = 0;
                    GameManager::board_playable[board].boards[i][j].position_status = Board::EMPTY;
                    GameManager::board_answer[board].boards[i][j].value = (GameManager::board_answer[board].boards[i][j].value) * -1;
                }
            }
        }
    }
}

void GameManager::process_events(void)
{   
    if(is_starting){
            return;
    }
    row_index = 0;
    column_index = 0;
    confirmation_cmd.clear();

    if(game_state == GameState::STARTING ||
       game_state == GameState::HELPING ||
       game_state == GameState::CHECKING_MOVES ||
       game_state == GameState::FINISHED_PUZZLES)
    {   
        //Only reads a simple enter from user
        std::string line;
        std::getline(std::cin, line);
    } 

    if(game_state == GameState::READING_MAIN_OPTION){
        string line;
        line = reading_main_menu_option();

        try {

        main_menu_option = stoi(line); 

        } catch(...) {

        current_message = "You must answer with a number within [1, 4]. Try again!";
        game_state = GameState::INVALID_INPUT_MAIN;
        return;
        }

        if(main_menu_option > 0 && main_menu_option < 5){
            return;
        } else {
            current_message = "You must answer with a number within [1, 4https://github.com/gabodin/Lista02_LPI/tree/master]. Try again!";
            game_state = GameState::INVALID_INPUT_MAIN;
        }

    }

    else if(game_state == GameState::PLAYING_MODE) {
        string line = reading_playing_menu_option();
        

        if(line == ""){
            game_state = GameState::BACK_TO_MENU;
            return;
        }

        std::string tokens = tokenizer(line);

        if(tokens.size() == 0){
            game_state = GameState::BACK_TO_MENU;
            current_message = "Back to main menu!";
            return;
        }

        if(tokens.size() < 5) {
            char aux1;
            string aux2;
            try{
                user_cmd = tokens[0];
            } catch(...){
                game_state = GameState::INVALID_INPUT_PLAY;
                current_message = "Invalid option. Try again";
                return;
            } 

            if(user_cmd == 'p' || user_cmd == 'r' || user_cmd == 'c' || user_cmd == 'u'){
                
                if(user_cmd == 'c' && n_checks == 0) {
                    game_state = GameState::INVALID_INPUT_PLAY;
                    current_message = "You can't use check anymore";
                    return;
                }

                if(user_cmd == 'c' && !(has_already_played)) {
                    game_state = GameState::INVALID_INPUT_PLAY;
                    current_message = "You have to place at least one number before check";
                    return;
                }
                
                if(user_cmd == 'u' && undo_list.size() == 0){
                    game_state = GameState::INVALID_INPUT_PLAY;
                    current_message = "There are no available actions to undo";
                    return;
                } 

                if(user_cmd == 'c' || user_cmd == 'u') {
                    return;
                }
                
                aux1 = tokens[1];
                aux2.push_back(aux1);
                
                try {
                    row_index = std::stoi(aux2);
                } catch(...){
                    game_state = GameState::INVALID_INPUT_PLAY;
                    current_message = "Invalid row. Try again";
                    return;
                }


                aux2.clear();
                aux1 = tokens[2];
                aux2.push_back(aux1);

                try {
                    column_index = std::stoi(aux2);
                } catch(...){
                    game_state = GameState::INVALID_INPUT_PLAY;
                    current_message = "Invalid column. Try again";
                    return;
                }

                if(!(row_index > 0 && row_index < 10 && column_index > 0 && column_index < 10)) {
                    game_state = GameState::INVALID_INPUT_PLAY;
                    row_index = 0;
                    column_index = 0;
                    current_message = "Not a valid index. Try again";
                    return;
                 }

                if(user_cmd == 'r'){
                    return;
                }

                aux2.clear();
                aux1 = tokens[3];
                aux2.push_back(aux1);


                try {
                    number_value = std::stoi(aux2);
                } catch(...){
                    game_state = GameState::INVALID_INPUT_PLAY;
                    current_message = "<number> must be in range [1, 9]. Try again";
                    return;
                }

                if(!(number_value > 0 && number_value < 10)){
                    game_state = GameState::INVALID_INPUT_PLAY;
                    row_index = 0;
                    column_index = 0;
                    number_value = 0;                    
                    current_message = "Not a valid value. Try again";
                    return;
                }

                return;

            } else {
                game_state = GameState::INVALID_INPUT_PLAY;
                current_message = "Invalid command. Try again";
                return;
            }
            
        } else {
           game_state = GameState::INVALID_INPUT_PLAY;
           current_message = "Invalid input. Try again";
           return; 
        }

    } else if(game_state == GameState::CONFIRMING_QUITTING_MATCH){
        string line = reading_playing_menu_option();

        string result = tokenizer(line);
        if(result != "y" && result != "y" && result != "N" && result != "n"){
            current_message = "You must answer with \'y\',  \'Y\', \'N\' or \'n\'";
            game_state = GameState::INVALID_INPUT_MAIN;
            return;
        } else {
            confirmation_cmd = result;
            return;
        }
    }
}

void GameManager::update(void) 
{   
    if(is_starting){
        return;
    }
    if(game_state == GameState::STARTING){
        generate_board_answer();
        game_state = GameState::READING_MAIN_OPTION;

    } else if (game_state == GameState::READING_MAIN_OPTION){
        switch(main_menu_option) {
            case 1: game_state = GameState::PLAYING_MODE;
                    break;
            case 2: game_state = GameState::NEW_GAME;
                    break;
            case 3: game_state = GameState::QUITTING_MATCH;                   
                    break;
            case 4: game_state = GameState::HELPING;
                    break;
            default: break;
        }
    } else if(game_state == GameState::PLAYING_MODE){
            if(user_cmd == 'u'){
                game_state = GameState::UNDOING_PLAY;
                undo();
                has_already_played = true;

            } else if (user_cmd == 'p'){
                game_state = GameState::PLACING_NUMBER;
                place(row_index - 1, column_index - 1, number_value);
                has_already_played = true;

            } else if (user_cmd == 'r') {
                game_state = GameState::REMOVING_NUMBER;
                remove(row_index - 1, column_index - 1);
                has_already_played = true;

            } else if (user_cmd == 'c'){
                game_state = GameState::CHECKING_MOVES;
                has_already_played = true;
            }       
    } else if(game_state == GameState::CHECKING_MOVES){
        game_state = GameState::PLAYING_MODE;

    } else if (game_state == GameState::HELPING){
        game_state = GameState::READING_MAIN_OPTION; 
    } else if (game_state == GameState::FINISHED_PUZZLES){
        game_state = GameState::NEW_GAME;   
    }

    if(game_state == GameState::NEW_GAME){
        bool result = new_game();
        if(result){
            game_state = GameState::READING_MAIN_OPTION;
        }
        else {
            game_state = GameState::CONFIRMING_QUITTING_MATCH;
            current_message = "Are you sure you want to leave?";
        }
    } else if(game_state == GameState::CONFIRMING_QUITTING_MATCH){
        if(confirm_quitting_match(confirmation_cmd)){
            has_already_played = false;
            current_message = "Your new board is ready! Good luck!";
            new_game();
        } else {
            current_message = "New board is cancelled!";
        }
        game_state = GameState::READING_MAIN_OPTION;
    } 


    if(game_state == GameState::INVALID_INPUT_MAIN ||
       game_state == GameState::BACK_TO_MENU )                                                  
    {
        game_state = GameState::READING_MAIN_OPTION; 

    }  else if
      (game_state == GameState::INVALID_INPUT_PLAY ||
       game_state == GameState::PLACING_NUMBER     ||
       game_state == GameState::REMOVING_NUMBER    ||
       game_state == GameState::UNDOING_PLAY) 
    {
           game_state = GameState::PLAYING_MODE;
    }

    if(is_finished()){
        game_state = GameState::FINISHED_PUZZLES;
        current_message = "Congratulations! You solved the puzzle! Press enter to continue";
        has_already_played = false;
    }

    
}

void GameManager::render(void) 
{
    if(game_state == GameState::STARTING){
        print_welcome();
        is_starting = false;
    } else if(game_state == GameState::HELPING){
        print_help();   
    } else if(game_state == GameState::READING_MAIN_OPTION){
        std::cout << "\n|--------[ MAIN SCREEN ]--------|\n\n";
        print_board();
        print_msg();
        print_main_menu();
    } else if(game_state == GameState::PLAYING_MODE) {
           std::cout << "\n|--------[ ACTION MODE ]--------|\n\n";
           print_board();
           print_number_check();
           print_digits_left();
           print_msg();
           print_command_syntax(); 
    } else if(game_state == GameState::CONFIRMING_QUITTING_MATCH){
        std::cout << "\n|--------[ MAIN SCREEN ]--------|\n\n";
        print_board();
        print_msg();
        print_confirm_quitting_match();
           
    } else if(game_state == GameState::CHECKING_MOVES){
        std::cout << "\n|--------[ ACTION MODE ]--------|\n\n";
        print_check_board();
        print_number_check();
        print_digits_left();
        print_msg();
    } else if(game_state == GameState::FINISHED_PUZZLES){
        print_board();
        print_number_check();
        print_msg();
    }


}


int main() {
    GameManager game;

    game.initialize();

    // Keep running until the game ends or user quits.
    while( not game.quit_game() ) 
    {
        game.process_events();
        game.update();
        game.render();
    }

    return EXIT_SUCCESS;
}

