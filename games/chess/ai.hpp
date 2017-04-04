#ifndef GAMES_CHESS_AI_HPP
#define GAMES_CHESS_AI_HPP

#include "impl/chess.hpp"
#include "game.hpp"
#include "game_object.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "player.hpp"

#include "../../joueur/src/base_ai.hpp"
#include "../../joueur/src/attr_wrapper.hpp"

// You can add additional #includes here
#include <stdlib.h>
#include <sstream>
#include <ctype.h>
#include <algorithm>

namespace cpp_client
{

namespace chess
{
    //Node used to hold move data for actual
    //moves to be made on the board
    struct node
    {
        std::vector<node> next_moves;
        char current_FEN[8][8];
        int old_file;
        int new_file;
        int old_rank;
        int new_rank;
        std::string promotion;
        int end_score;
        bool is_white;
    };

    //Sort nodes for minimax evaluation once all nodes in the vector
    //have been set up and given an end_score
    struct sortNodes
    {
        inline bool operator() (const node &node1, const node &node2)
        {
            int firstValue = node1.end_score;
            int secondValue = node2.end_score;

            //Sort the values by end score with highest scores
            //first and lowest scores last (white will choose
            //from the front and black from the back of the vector)
            return (firstValue > secondValue);
        }
    };

    //Holds data on past moves made to check
    //for repetition rules
    struct move_data
    {
        int old_file;
        int new_file;
        int old_rank;
        int new_rank;
        std::string type;
    };

/// <summary>
/// This is the header file for building your Chess AI
/// </summary>
class AI : public Base_ai
{
public:
    /// <summary>
    /// This is a reference to the Game object itself, it contains all the information about the current game
    /// </summary>
    Game game;

    /// <summary>
    /// This is a pointer to your AI's player. This AI class is not a player, but it should command this Player.
    /// </summary>
    Player player;

    // You can add additional class variables here.

    /// <summary>
    /// This returns your AI's name to the game server.
    /// Replace the string name.
    /// </summary>
    /// <returns>The name of your AI.</returns>
    virtual std::string get_name() const override;

    /// <summary>
    /// This is automatically called when the game first starts, once the game objects are created
    /// </summary>
    virtual void start() override;

    /// <summary>
    /// This is automatically called when the game ends.
    /// </summary>
    /// <param name="won">true if you won, false otherwise</param>
    /// <param name="reason">An explanation for why you either won or lost</param>
    virtual void ended(bool won, const std::string& reason) override;

    /// <summary>
    /// This is automatically called the game (or anything in it) updates
    /// </summary>
    virtual void game_updated() override;

    /// <summary>
    /// This is called every time it is this AI.player's turn.
    /// </summary>
    /// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
    bool run_turn();

    /// <summary>
    ///  Prints the current board using pretty ASCII art
    /// </summary>
    /// <remarks>
    /// Note: you can delete this function if you wish
    /// </remarks>
    void print_current_board();

    //Comments for below functions and variables can be
    //found in the accompanying C++ file
    void generate_FEN_array();

    //Used for minimax evaluations to find the move to make
    //and regressively search for moves
    int explore_moves(int limit, node *start_board, int *alpha, int *beta);
    int find_move_number(node game_board);
    void find_possible_moves();

    //Used to check for draw situations
    bool insufficient_material(char board_to_score[8][8]);
    bool move_repetition(int old_f, int new_f, int old_r, int new_r, char ctype);

    //Scores the board and finds the given char file for a file number
    int score_board(char board_to_score[8][8]);
    char find_file(int file_num);

    //Move generation functions
    void move_pawn(int rank, int file_num);
    void move_king(int rank, int file_num);
    void move_rook_or_queen(int rank, int file_num);
    void move_bishop_or_queen(int rank, int file_num);
    void move_knight(int rank, int file_num);

    //Rule checking functions
    bool would_space_check(int file_num, int rank);
    void check_for_castling(int rank, int file_num);
    void promote_pawn(int file_num, int new_file, int old_rank, int new_rank);

    //Set up move function and check space functions
    void set_up_move(int old_f, int new_f, int old_r,
                    int new_r, std::string promo);
    bool empty_space(int file_num, int rank);
    bool opponent_located(int file_num, int rank);

    //Vector for holding all possible moves
    std::vector<node> possible_moves;
    std::vector<node> starter_moves;
    std::vector<move_data> moves_made;

    //Double array holding the state of the board
    char FEN_board[8][8];

    //Variables that determine if en passant and castling are possible
    int en_passant_rank;
    int en_passant_file;
    char castling[4];

    //Whether or not the current player is denoted by lower
    //case characters in the FEN string
    bool player_lower_case;
    bool me_lower_case;
    int current_score;

    clock_t startTime;
    clock_t stopTime;
    double timeTaken;

    // ####################
    // Don't edit these!
    // ####################
    /// \cond FALSE
    virtual std::string invoke_by_name(const std::string& name,
                                       const std::unordered_map<std::string, Any>& args) override;
    virtual void set_game(Base_game* ptr) override;
    virtual void set_player(std::shared_ptr<Base_object> obj) override;
    virtual void print_win_loss_info() override;
    /// \endcond
    // ####################
    // Don't edit these!
    // ####################
};

} // CHESS

} // cpp_client

#endif // GAMES_CHESS_AI_HPP
