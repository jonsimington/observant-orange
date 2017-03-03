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
            if(player->in_check)
            {
                possible_moves.clear();
                MoveKing(piece);
                break;
            }
            MoveKing(piece);
        }

        if(piece->type == "Rook")
        {
            MoveRookOrQueen(piece);
        }

        if(piece->type == "Bishop")
        {
            MoveBishopOrQueen(piece);
        }

        if(piece->type == "Queen")
        {
            MoveBishopOrQueen(piece);
            MoveRookOrQueen(piece);
        }

        if(piece->type == "Knight")
        {
            MoveKnight(piece);
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

    move_location = pawn->rank + player->rank_direction;
    if(!pawn->has_moved)
    {
        if(EmptySpace(pawn->file, move_location) &&
           EmptySpace(pawn->file, move_location + player->rank_direction))
        {
            SetUpMove(pawn, pawn->file, move_location, "");
        }
    }
    else
    {
        if(EmptySpace(pawn->file, move_location))
        {
            if(move_location + player->rank_direction > 8 ||
               move_location + player->rank_direction < 1)
            {
                PromotePawn(pawn, pawn->file, move_location);
            }
            else
            {
                SetUpMove(pawn, pawn->file, move_location, "");
            }
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

    final_file = file_location + 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
       !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    final_file = file_location - 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    move_location = king->rank + player->rank_direction;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    final_file = file_location + 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    if((EmptySpace(king->file, move_location) ||
       OpponentLocated(king->file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, king->file, move_location, "");
    }

    move_location = king->rank - player->rank_direction;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    final_file = file_location - 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, final_file, move_location, "");
    }

    if((EmptySpace(king->file, move_location) ||
       OpponentLocated(king->file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(king, king->file, move_location, "");
    }
}

void AI::MoveRookOrQueen(Piece rook_queen)
{
    int move_location = rook_queen->rank;
    int file_location = rook_queen->file[0];
    std::string final_file;

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location + 1 + i;
        if(EmptySpace(final_file, move_location))
        {
            SetUpMove(rook_queen, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(rook_queen, final_file, move_location, "");
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
        if(EmptySpace(final_file, move_location))
        {
            SetUpMove(rook_queen, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(rook_queen, final_file, move_location, "");
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
        if(EmptySpace(rook_queen->file, move_location))
        {
            SetUpMove(rook_queen, rook_queen->file, move_location, "");
        }
        else if(OpponentLocated(rook_queen->file, move_location))
        {
            SetUpMove(rook_queen, rook_queen->file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = rook_queen->rank;
    for(int i = 0; i < 8; ++i)
    {
        move_location -= player->rank_direction;
        if(EmptySpace(rook_queen->file, move_location))
        {
            SetUpMove(rook_queen, rook_queen->file, move_location, "");
        }
        else if(OpponentLocated(rook_queen->file, move_location))
        {
            SetUpMove(rook_queen, rook_queen->file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }
}

void AI::MoveBishopOrQueen(Piece bishop_queen)
{
    int move_location = bishop_queen->rank;
    int file_location = bishop_queen->file[0];
    std::string final_file;

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location + 1 + i;
        move_location -= player->rank_direction;
        if(EmptySpace(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = bishop_queen->rank;
    file_location = bishop_queen->file[0];
    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location + 1 + i;
        move_location += player->rank_direction;
        if(EmptySpace(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = bishop_queen->rank;
    file_location = bishop_queen->file[0];
    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location - 1 - i;
        move_location -= player->rank_direction;
        if(EmptySpace(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = bishop_queen->rank;
    file_location = bishop_queen->file[0];
    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location - 1 - i;
        move_location += player->rank_direction;
        if(EmptySpace(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
        }
        else if(OpponentLocated(final_file, move_location))
        {
            SetUpMove(bishop_queen, final_file, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }
}

void AI::MoveKnight(Piece knight)
{
    int move_location = knight->rank;
    int file_location = knight->file[0];
    std::string final_file;

    final_file = file_location + 2;
    move_location -= player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }

    final_file = file_location + 2;
    move_location = knight->rank + player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }

    final_file = file_location + 1;
    move_location = knight->rank + player->rank_direction + player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }

    final_file = file_location + 1;
    move_location = knight->rank - player->rank_direction - player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }

    final_file = file_location - 2;
    move_location -= player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }
    
    final_file = file_location - 2;
    move_location = knight->rank + player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }
    
    final_file = file_location - 1;
    move_location = knight->rank + player->rank_direction + player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }
    
    final_file = file_location - 1;
    move_location = knight->rank - player->rank_direction - player->rank_direction;
    if(EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location))
    {
        SetUpMove(knight, final_file, move_location, "");
    }
}

void AI::PromotePawn(Piece pawn, std::string file, int rank)
{
    int random_move = rand() % 4;

    if(random_move == 0)
    {
        SetUpMove(pawn, file, rank, "Queen");
    }
    else if(random_move == 1)
    {
        SetUpMove(pawn, file, rank, "Rook");
    }
    else if(random_move == 2)
    {
        SetUpMove(pawn, file, rank, "Bishop");
    }
    else if(random_move == 3)
    {
        SetUpMove(pawn, file, rank, "Knight");
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

bool AI::WouldSpaceCheck(std::string file, int rank)
{
    int attack_rank = rank;
    int king_file = file[0];
    std::string attack_file;

    if(king_file < 97 || king_file > 104 || attack_rank > 8 || attack_rank < 1)
    {
        return false;
    }

    for(int i = 0; i < player->opponent->pieces.size(); ++i)
    {
        Piece piece = player->opponent->pieces[i];
        if(piece->type == "Pawn" || piece->type == "King" ||
           piece->type == "Bishop" || piece->type == "Queen")
        {
            attack_file = king_file + 1;
            if((piece->file == attack_file && piece->rank == attack_rank + 1) ||
               (piece->file == attack_file && piece->rank == attack_rank - 1))
            {
                return true;
            }
            attack_file = king_file - 1;
            if((piece->file == attack_file && piece->rank == attack_rank + 1) ||
               (piece->file == attack_file && piece->rank == attack_rank - 1))
            {
                return true;
            }
        }

        if(piece->type == "Rook" || piece->type == "King" ||
           piece->type == "Queen")
        {
            if((piece->file == file && piece->rank == attack_rank + 1) ||
               (piece->file == file && piece->rank == attack_rank - 1))
            {
                return true;
            }
            attack_file = king_file - 1;
            if((piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }
            attack_file = king_file + 1;
            if((piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }
        }

        if(piece->type == "Rook" || piece->type == "Queen")
        {
            for(int i = 0; i < 8; ++i)
            {
                attack_file = king_file + 1 + i;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(EmptySpace(attack_file, attack_rank))
                {
                    break;
                }
            }
            
            for(int i = 0; i < 8; ++i)
            {
                attack_file = king_file - 1 - i;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(EmptySpace(attack_file, attack_rank))
                {
                    break;
                }
            }

            attack_rank = rank;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank += player->rank_direction;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(EmptySpace(attack_file, attack_rank))
                {
                    break;
                }
            }

            attack_rank = rank;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank -= player->rank_direction;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(EmptySpace(attack_file, attack_rank))
                {
                    break;
                }
            }
        }
    }

    return false;
}

bool AI::EmptySpace(std::string file, int rank)
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
