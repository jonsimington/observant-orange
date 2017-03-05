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

namespace cpp_client
{

namespace chess
{

    struct node
    {
        char current_FEN[8][8];
        std::string old_file;
        std::string new_file;
        int old_rank;
        int new_rank;
        std::string promotion;
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

    // You can add additional methods here.
    void GenerateFENArray();
    void PrintFENArray();
    void MovePawn(int rank, int file_num);
    void MoveKing(int rank, int file_num);
    void MoveRookOrQueen(int rank, int file_num);
    void MoveBishopOrQueen(int rank, int file_num);
    void MoveKnight(int rank, int file_num);
    bool WouldSpaceCheck(int file_num, int rank);
    void PromotePawn(int file_num, int old_rank, int new_rank);
    void SetUpMove(int old_file, int new_file, int old_rank,
                    int new_rank, std::string promo);
    bool EmptySpace(int file_num, int rank);
    bool OpponentLocated(int file_num, int rank);
    std::vector<node> possible_moves;
    std::vector<Piece> temp_pieces;
    char FEN_board[8][8];
    bool player_lower_case;
    int promo_num;

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
