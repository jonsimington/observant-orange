// AI
// This is where you build your AI

#include "ai.hpp"

// You can add #includes here for your AI.

namespace cpp_client
{

namespace chess
{

/// <summary>
/// This returns your AI's name to the game server.
/// Replace the string name.
/// </summary>
/// <returns>The name of your AI.</returns>
std::string AI::get_name() const
{
    // REPLACE WITH YOUR TEAM NAME!
    return "Chess C++ Player";
}

/// <summary>
/// This is automatically called when the game first starts, once the game objects are created
/// </summary>
void AI::start()
{
    // This is a good place to initialize any variables
    srand(time(NULL));
}

/// <summary>
/// This is automatically called the game (or anything in it) updates
/// </summary>
void AI::game_updated()
{
    // If a function you call triggers an update this will be called before it returns.
}

/// <summary>
/// This is automatically called when the game ends.
/// </summary>
/// <param name="won">true if you won, false otherwise</param>
/// <param name="reason">An explanation for why you either won or lost</param>
void AI::ended(bool won, const std::string& reason)
{
    // You can do any cleanup of your AI here.  The program ends when this function returns.
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{
    // Here is where you'll want to code your AI.

    // We've provided sample code that:
    //    1) prints the board to the console
    //    2) prints the opponent's last move to the console
    //    3) prints how much time remaining this AI has to calculate moves
    //    4) makes a random (and probably invalid) move.

    // 1) print the board to the console
    print_current_board();

    // 2) print the opponent's last move to the console
    if(game->moves.size() > 0)
    {
        std::cout << "Opponent's Last Move: '" << game->moves[game->moves.size() - 1]->san << "'" << std::endl;
    }

    // 3) print how much time remaining this AI has to calculate moves
    std::cout << "Time Remaining: " << player->time_remaining << " ns" << std::endl;

    // 4) make a random (and probably invalid) move.
    /*chess::Piece random_piece = player->pieces[rand() % player->pieces.size()];
    std::string random_file(1, 'a' + rand() % 8);
    int random_rank = (rand() % 8) + 1;
    random_piece->move(random_file, random_rank);*/
    for(int i = 0; i < player->pieces.size(); ++i)
    {
        Piece piece = player->pieces[i];

        if(piece->type == "Pawn")
        {
            MovePawn(piece);
        }

        if(piece->type == "King")
        {
            MoveKing(piece);
        }

        if(piece->type == "Rook")
        {
            MoveRook(piece);
        }
    }

    if(!possible_moves.empty())
    {
        std::cout << "Possible moves exist\n";
        int move_number = rand() % possible_moves.size();
        Piece& piece_to_move = possible_moves[move_number].piece;
        std::string new_file = possible_moves[move_number].new_file;
        int new_rank = possible_moves[move_number].new_rank;
        std::string promo = possible_moves[move_number].promotion;
        piece_to_move->move(new_file, new_rank, promo);
        possible_moves.clear();
    }

    return true; // to signify we are done with our turn.
}

/// <summary>
///  Prints the current board using pretty ASCII art
/// </summary>
/// <remarks>
/// Note: you can delete this function if you wish
/// </remarks>
void AI::print_current_board()
{
    for(int rank = 9; rank >= -1; rank--)
    {
        std::string str = "";
        if(rank == 9 || rank == 0) // then the top or bottom of the board
        {
            str = "   +------------------------+";
        }
        else if(rank == -1) // then show the ranks
        {
            str = "     a  b  c  d  e  f  g  h";
        }
        else // board
        {
            str += " ";
            str += std::to_string(rank);
            str += " |";
            // fill in all the files with pieces at the current rank
            for(int file_offset = 0; file_offset < 8; file_offset++)
            {
                std::string file(1, 'a' + file_offset); // start at a, with with file offset increasing the char;
                chess::Piece current_piece = nullptr;
                for(const auto& piece : game->pieces)
                {
                    if(piece->file == file && piece->rank == rank) // then we found the piece at (file, rank)
                    {
                        current_piece = piece;
                        break;
                    }
                }

                char code = '.'; // default "no piece";
                if(current_piece != nullptr)
                {
                    code = current_piece->type[0];

                    if(current_piece->type == "Knight") // 'K' is for "King", we use 'N' for "Knights"
                    {
                        code = 'N';
                    }

                    if(current_piece->owner->id == "1") // the second player (black) is lower case. Otherwise it's upppercase already
                    {
                        code = tolower(code);
                    }
                }

                str += " ";
                str += code;
                str += " ";
            }

            str += "|";
        }

        std::cout << str << std::endl;
    }
}

// You can add additional methods here for your AI to call

void AI::MovePawn(Piece pawn)
{
    int move_location = 0;

    node move_to_make;

    move_location = pawn->rank + player->rank_direction;
    if(!pawn->has_moved)
    {
        if(MovePossible(pawn->file, move_location) &&
           MovePossible(pawn->file, move_location + player->rank_direction))
        {
            SetUpMove(pawn, pawn->file, move_location, "");
        }
    }
    else
    {
        if(MovePossible(pawn->file, move_location))
        {
            SetUpMove(pawn, pawn->file, move_location, "");
        }
    }

    int file_location = pawn->file[0];
    std::string final_file;
    final_file = file_location + 1;
    move_location = pawn->rank + player->rank_direction;
    if(OpponentLocated(final_file, move_location))
    {
        SetUpMove(pawn, final_file, move_location, "");
    }

    final_file = file_location - 1;
    if(OpponentLocated(final_file, move_location))
    {
        SetUpMove(pawn, final_file, move_location, "");
    }
}

void AI::MoveKing(Piece king)
{
    int move_location = king->rank;
    int file_location = king->file[0];
    std::string final_file;
    node move_to_make;

    final_file = file_location + 1;
    if(MovePossible(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    final_file = file_location - 1;
    if(MovePossible(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    move_location = king->rank + player->rank_direction;
    if(MovePossible(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    final_file = file_location + 1;
    if(MovePossible(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    if(MovePossible(king->file, move_location) ||
       OpponentLocated(king->file, move_location))
    {
        SetUpMove(king, king->file, move_location, "");
    }

    move_location = king->rank - player->rank_direction;
    if(MovePossible(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    final_file = file_location - 1;
    if(MovePossible(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    if(MovePossible(king->file, move_location) ||
       OpponentLocated(king->file, move_location))
    {
        SetUpMove(king, king->file, move_location, "");
    }
}

void AI::MoveRook(Piece rook)
{
    int move_location = rook->rank;
    int file_location = rook->file[0];
    std::string final_file;
    node move_to_make;
    move_to_make.piece = rook;

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location + 1 + i;
        if(MovePossible(final_file, move_location))
        {
            SetUpMove(rook, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(rook, final_file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location - 1 - i;
        if(MovePossible(final_file, move_location))
        {
            SetUpMove(rook, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(rook, final_file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    for(int i = 0; i < 8; ++i)
    {
        move_location += player->rank_direction;
        if(MovePossible(rook->file, move_location))
        {
            SetUpMove(rook, rook->file, move_location, "");
        }
        else if(OpponentLocated(rook->file, move_location))
        {
            SetUpMove(rook, rook->file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = rook->rank;
    for(int i = 0; i < 8; ++i)
    {
        move_location -= player->rank_direction;
        if(MovePossible(rook->file, move_location))
        {
            SetUpMove(rook, rook->file, move_location, "");
        }
        else if(OpponentLocated(rook->file, move_location))
        {
            SetUpMove(rook, rook->file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }
}

void AI::SetUpMove(Piece game_piece, std::string file, int rank, std::string promo)
{
    node move_to_make;
    move_to_make.piece = game_piece;
    move_to_make.new_file = file;
    move_to_make.new_rank = rank;
    move_to_make.promotion = promo;
    possible_moves.push_back(move_to_make);
}

bool AI::MovePossible(std::string file, int rank)
{
    int file_num;
    file_num = file[0];

    if(file_num < 97 || file_num > 104 || rank > 8 || rank < 1)
    {
        return false;
    }

    for(int i = 0; i < game->pieces.size(); ++i)
    {
        if(game->pieces[i]->file == file && game->pieces[i]->rank == rank)
        {
            return false;
        }
    }

    return true;
}

bool AI::OpponentLocated(std::string file, int rank)
{
    int file_num;
    file_num = file[0];
    
    if(file_num < 97 || file_num > 104 || rank > 8 || rank < 1)
    {
        return false;
    }
    
    for(int i = 0; i < game->pieces.size(); ++i)
    {
        if(game->pieces[i]->file == file && game->pieces[i]->rank == rank &&
           game->pieces[i]->owner == player->opponent)
        {
            return true;
        }
    }
    
    return false;
}

} // chess

} // cpp_client
