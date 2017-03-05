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
    player_lower_case = false;
    promo_num = 0;
    GenerateFENArray();
    Piece temp = player->pieces[0];
    int file_num = temp->file[0] - 'a';
    char my_piece = FEN_board[temp->rank - 1][file_num];
    if(islower(my_piece))
    {
        player_lower_case = true;
    }
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
    std::cout << "Number of promotions: " << promo_num << '\n';
    // You can do any cleanup of your AI here.  The program ends when this function returns.
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{
    // Here is where you'll want to code your AI.
    GenerateFENArray();
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
    for(int i = 0; i < 8; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            if((FEN_board[i][j] == 'p' && player_lower_case) ||
               (FEN_board[i][j] == 'P' && !player_lower_case))
            {
                MovePawn(i, j);
            }

            if((FEN_board[i][j] == 'k' && player_lower_case) ||
               (FEN_board[i][j] == 'K' && !player_lower_case))
            {
                MoveKing(i, j);
            }

            if(((FEN_board[i][j] == 'r' || FEN_board[i][j] == 'q') && player_lower_case) ||
               ((FEN_board[i][j] == 'R' || FEN_board[i][j] == 'Q') && !player_lower_case))
            {
                MoveRookOrQueen(i, j);
            }

            if(((FEN_board[i][j] == 'b' || FEN_board[i][j] == 'q') && player_lower_case) ||
               ((FEN_board[i][j] == 'B' || FEN_board[i][j] == 'Q') && !player_lower_case))
            {
                MoveBishopOrQueen(i, j);
            }

            if((FEN_board[i][j] == 'n' && player_lower_case) ||
               (FEN_board[i][j] == 'N' && !player_lower_case))
            {
                MoveKnight(i, j);
            }
        }
    }

    bool move_made = false;
    while(!possible_moves.empty() && !move_made)
    {
        Piece piece_to_move;
        int move_number = rand() % possible_moves.size();
        for(int i = 0; i < player->pieces.size(); ++i)
        {
            if(player->pieces[i]->rank == possible_moves[move_number].old_rank &&
               player->pieces[i]->file == possible_moves[move_number].old_file)
            {
                piece_to_move = player->pieces[i];
                break;
            }
        }
        //std::cout << "------------------------------\n";

        for(int i = 0; i < 8; ++i)
        {
            for(int j = 0; j < 8; ++j)
            {
                FEN_board[i][j] = possible_moves[move_number].current_FEN[i][j];
            }
        }

        for(int i = 0; i < 8 && !move_made; ++i)
        {
            for(int j = 0; j < 8 && !move_made; ++j)
            {
                if(((possible_moves[move_number].current_FEN[i][j] == 'k'
                     && player_lower_case) ||
                   (possible_moves[move_number].current_FEN[i][j] == 'K'
                    && !player_lower_case)))
                {
                   // std::cout << "Moving " << piece_to_move-> type << "\n";
                    if(!WouldSpaceCheck(j, i))
                    {
                        if(possible_moves[move_number].promotion != "")
                        {
                            ++promo_num;
                        }
                        piece_to_move->move(possible_moves[move_number].new_file,
                                        possible_moves[move_number].new_rank,
                                        possible_moves[move_number].promotion);
                        move_made = true;
                    }
                    break;
                    i = 8;
                }
            }
        }

        possible_moves.erase(possible_moves.begin() + move_number);
    }

    //std::cout << "++++++++++++++++++++++++++++++\n";
    possible_moves.clear();
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

void AI::MovePawn(int rank, int file_num)
{
    int move_location = 0;
    Piece pawn;
    std::string pawn_file;
    pawn_file = file_num + 97;

    for(int i = 0; i < player->pieces.size(); ++i)
    {
        if(player->pieces[i]->rank == rank + 1 &&
           player->pieces[i]->file == pawn_file &&
           player->pieces[i]->type == "Pawn")
        {
            pawn = player->pieces[i];
            break;
        }
    }

    move_location = rank + player->rank_direction;
    if(!pawn->has_moved)
    {
        if(EmptySpace(file_num, move_location) &&
           EmptySpace(file_num, move_location + player->rank_direction))
        {
            SetUpMove(file_num, file_num, rank, move_location + player->rank_direction, "");
        }
    }
    else
    {
        if(EmptySpace(file_num, move_location))
        {
            if(move_location + player->rank_direction > 7 ||
               move_location + player->rank_direction < 0)
            {
                PromotePawn(file_num, rank, move_location);
            }
            else
            {
                SetUpMove(file_num, file_num, rank, move_location, "");
            }
        }
    }

    int file_location = file_num + 1;
    if(OpponentLocated(file_location, move_location))
    {
        SetUpMove(file_num, file_location, rank, move_location, "");
    }

    file_location = file_num - 1;
    if(OpponentLocated(file_location, move_location))
    {
        SetUpMove(file_num, file_location, rank, move_location, "");
    }
}

void AI::MoveKing(int rank, int file_num)
{
    int move_location = rank;
    int final_file;

    final_file = file_num + 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
       !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(file_num, final_file, rank, move_location, "");
    }

    final_file = file_num - 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(file_num, final_file, rank, move_location, "");
    }

    move_location = rank + player->rank_direction;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(file_num, final_file, rank, move_location, "");
    }

    final_file = file_num + 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(file_num, final_file, rank, move_location, "");
    }

    if((EmptySpace(file_num, move_location) ||
       OpponentLocated(file_num, move_location)) &&
        !WouldSpaceCheck(file_num, move_location))
    {
        SetUpMove(file_num, file_num, rank, move_location, "");
    }

    move_location = rank - player->rank_direction;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(file_num, final_file, rank, move_location, "");
    }

    final_file = file_num - 1;
    if((EmptySpace(final_file, move_location) ||
       OpponentLocated(final_file, move_location)) &&
        !WouldSpaceCheck(final_file, move_location))
    {
        SetUpMove(file_num, final_file, rank, move_location, "");
    }

    if((EmptySpace(file_num, move_location) ||
       OpponentLocated(file_num, move_location)) &&
        !WouldSpaceCheck(file_num, move_location))
    {
        SetUpMove(file_num, file_num, rank, move_location, "");
    }
}

void AI::MoveRookOrQueen(int rank, int file_num)
{
    int move_location = rank;
    std::string final_file;

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_num + 1 + i;
        if(EmptySpace(file_num + 1 + i, move_location))
        {
            SetUpMove(file_num, file_num + 1 + i, rank, move_location, "");
        }
        else if(OpponentLocated(file_num + 1 + i, move_location))
        {
            SetUpMove(file_num, file_num + 1 + i, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_num - 1 - i;
        if(EmptySpace(file_num - 1 - i, move_location))
        {
            SetUpMove(file_num, file_num - 1 - i, rank, move_location, "");
        }
        else if(OpponentLocated(file_num - 1 - i, move_location))
        {
            SetUpMove(file_num, file_num - 1 - i, rank, move_location, "");
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
        if(EmptySpace(file_num, move_location))
        {
            SetUpMove(file_num, file_num, rank, move_location, "");
        }
        else if(OpponentLocated(file_num, move_location))
        {
            SetUpMove(file_num, file_num, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = rank;
    for(int i = 0; i < 8; ++i)
    {
        move_location -= player->rank_direction;
        if(EmptySpace(file_num, move_location))
        {
            SetUpMove(file_num, file_num, rank, move_location, "");
        }
        else if(OpponentLocated(file_num, move_location))
        {
            SetUpMove(file_num, file_num, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }
}

void AI::MoveBishopOrQueen(int rank, int file_num)
{
    int move_location = rank;
    int file_location = file_num;
    std::string final_file;

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location + 1 + i;
        move_location -= player->rank_direction;
        if(EmptySpace(file_location + 1 + i, move_location))
        {
            SetUpMove(file_num, file_location + 1 + i, rank, move_location, "");
        }
        else if(OpponentLocated(file_location + 1 + i, move_location))
        {
            SetUpMove(file_num, file_location + 1 + i, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = rank;
    file_location = file_num;
    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location + 1 + i;
        move_location += player->rank_direction;
        if(EmptySpace(file_location + 1 + i, move_location))
        {
            SetUpMove(file_num, file_location + 1 + i, rank, move_location, "");
        }
        else if(OpponentLocated(file_location + 1 + i, move_location))
        {
            SetUpMove(file_num, file_location + 1 + i, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = rank;
    file_location = file_num;
    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location - 1 - i;
        move_location -= player->rank_direction;
        if(EmptySpace(file_location - 1 - i, move_location))
        {
            SetUpMove(file_num, file_location - 1 - i, rank, move_location, "");
        }
        else if(OpponentLocated(file_location - 1 - i, move_location))
        {
            SetUpMove(file_num, file_location - 1 - i, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }

    move_location = rank;
    file_location = file_num;
    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location - 1 - i;
        move_location += player->rank_direction;
        if(EmptySpace(file_location - 1 - i, move_location))
        {
            SetUpMove(file_num, file_location - 1 - i, rank, move_location, "");
        }
        else if(OpponentLocated(file_location - 1 - i, move_location))
        {
            SetUpMove(file_num, file_location - 1 - i, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }
}

void AI::MoveKnight(int rank, int file_num)
{
    int move_location = rank;
    int file_location = file_num;
    std::string final_file;

    final_file = file_location + 2;
    move_location -= player->rank_direction;
    if(EmptySpace(file_location + 2, move_location) ||
       OpponentLocated(file_location + 2, move_location))
    {
        SetUpMove(file_num, file_location + 2, rank, move_location, "");
    }

    move_location = rank + player->rank_direction;
    if(EmptySpace(file_location + 2, move_location) ||
       OpponentLocated(file_location + 2, move_location))
    {
        SetUpMove(file_num, file_location + 2, rank, move_location, "");
    }

    final_file = file_location + 1;
    move_location = rank + player->rank_direction + player->rank_direction;
    if(EmptySpace(file_location + 1, move_location) ||
       OpponentLocated(file_location + 1, move_location))
    {
        SetUpMove(file_num, file_location + 1, rank, move_location, "");
    }

    move_location = rank - player->rank_direction - player->rank_direction;
    if(EmptySpace(file_location + 1, move_location) ||
       OpponentLocated(file_location + 1, move_location))
    {
        SetUpMove(file_num, file_location + 1, rank, move_location, "");
    }

    final_file = file_location - 2;
    move_location = rank - player->rank_direction;
    if(EmptySpace(file_location - 2, move_location) ||
       OpponentLocated(file_location - 2, move_location))
    {
        SetUpMove(file_num, file_location - 2, rank, move_location, "");
    }

    move_location = rank + player->rank_direction;
    if(EmptySpace(file_location - 2, move_location) ||
       OpponentLocated(file_location - 2, move_location))
    {
        SetUpMove(file_num, file_location - 2, rank, move_location, "");
    }
    
    final_file = file_location - 1;
    move_location = rank + player->rank_direction + player->rank_direction;
    if(EmptySpace(file_location - 1, move_location) ||
       OpponentLocated(file_location - 1, move_location))
    {
        SetUpMove(file_num, file_location - 1, rank, move_location, "");
    }

    move_location = rank - player->rank_direction - player->rank_direction;
    if(EmptySpace(file_location - 1, move_location) ||
       OpponentLocated(file_location - 1, move_location))
    {
        SetUpMove(file_num, file_location - 1, rank, move_location, "");
    }
}

void AI::PromotePawn(int file_num, int old_rank, int new_rank)
{
    int random_move = rand() % 4;

    if(random_move == 0)
    {
        SetUpMove(file_num, file_num, old_rank, new_rank, "Queen");
    }
    else if(random_move == 1)
    {
        SetUpMove(file_num, file_num, old_rank, new_rank, "Rook");
    }
    else if(random_move == 2)
    {
        SetUpMove(file_num, file_num, old_rank, new_rank, "Bishop");
    }
    else if(random_move == 3)
    {
        SetUpMove(file_num, file_num, old_rank, new_rank, "Knight");
    }
}

void AI::SetUpMove(int old_file, int new_file, int old_rank,
                   int new_rank, std::string promo)
{
    node move_to_make;
    for(int i = 0; i <8; ++i)
    {
        for(int j =0; j < 8; ++j)
        {
            move_to_make.current_FEN[i][j] = FEN_board[i][j];
        }
    }

    move_to_make.current_FEN[new_rank][new_file] = FEN_board[old_rank][old_file];
    move_to_make.current_FEN[old_rank][old_file] = '.';

    move_to_make.old_file = old_file + 'a';
    move_to_make.new_file = new_file + 'a';
    move_to_make.old_rank = old_rank + 1;
    move_to_make.new_rank = new_rank + 1;
    move_to_make.promotion = promo;
    possible_moves.push_back(move_to_make);
}

bool AI::WouldSpaceCheck(int file_num, int rank)
{
    if(file_num < 0 || file_num > 7 || rank > 7 || rank < 0)
    {
        return false;
    }

    for(int j = 0; j < player->opponent->pieces.size(); ++j)
    {
        int attack_rank = rank + 1;
        std::string attack_file;
        int file_location = file_num + 97;
        attack_file = file_location;
        Piece piece = player->opponent->pieces[j];
        //std::cout << "Type: " << piece->type << '\n';

        if(piece->type == "Pawn" || piece->type == "King" ||
           piece->type == "Bishop" || piece->type == "Queen")
        {
            attack_file = file_location + 1;
            if((piece->file == attack_file && piece->rank == attack_rank + 1) ||
               (piece->file == attack_file && piece->rank == attack_rank - 1))
            {
                return true;
            }
            attack_file = file_location - 1;
            if((piece->file == attack_file && piece->rank == attack_rank + 1) ||
               (piece->file == attack_file && piece->rank == attack_rank - 1))
            {
                return true;
            }
        }

        attack_file = file_location;
        if(piece->type == "Rook" || piece->type == "King" ||
           piece->type == "Queen")
        {
            if((piece->file == attack_file && piece->rank == attack_rank + 1) ||
               (piece->file == attack_file && piece->rank == attack_rank - 1))
            {
                return true;
            }
            attack_file = file_location - 1;
            if((piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }
            attack_file = file_location + 1;
            if((piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }
        }

        if(piece->type == "Rook" || piece->type == "Queen")
        {
            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location + 1 + i;
                //std::cout << "8Check Space: " << attack_file << " " << attack_rank << '\n';

                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num + 1 + i, rank))
                {
                    break;
                }
            }

            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location - 1 - i;
                //std::cout << "7Check Space: " << attack_file << " " << attack_rank << '\n';

                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num - 1 - i, rank))
                {

                    break;
                }
            }

            attack_rank = rank + 1;
            attack_file = file_location;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank += 1;
                //std::cout << "6Check Space: " << attack_file << " " << attack_rank << '\n';

                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank -= 1;
                //std::cout << "5Check Space: " << attack_file << " " << attack_rank << '\n';

                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num, attack_rank - 1))
                {
                    break;
                }
            }
        }

        attack_rank = rank + 1;
        attack_file = file_location;
        if(piece->type == "Bishop" || piece->type == "Queen")
        {
            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location + 1 + i;
                attack_rank += 1;
                //std::cout << "4Check Space: " << attack_file << " " << attack_rank << '\n';
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num + 1 + i, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location - 1 - i;
                attack_rank += 1;
                //std::cout << "3Check Space: " << attack_file << " " << attack_rank << '\n';
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num - 1 - i, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location + 1 + i;
                attack_rank -= 1;
                //std::cout << "2Check Space: " << attack_file << " " << attack_rank << '\n';

                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num + 1 + i, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank -= 1;
                attack_file = file_location - 1 - i;
                //std::cout << "1Check Space: " << attack_file << " " << attack_rank << '\n';

                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    return true;
                }
                else if(!EmptySpace(file_num - 1 - i, attack_rank - 1))
                {
                    break;
                }
            }
        }

        attack_rank = rank + 1;
        if(piece->type == "Knight")
        {
            attack_file = file_location - 1;
            attack_rank = attack_rank + player->rank_direction + player->rank_direction;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }

            attack_file = file_location + 1;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }

            attack_rank = rank + 1;
            attack_rank = attack_rank - player->rank_direction - player->rank_direction;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }

            attack_file = file_location - 1;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }

            attack_rank = rank + 1;
            attack_file = file_location - 2;
            attack_rank = attack_rank + player->rank_direction;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }
            
            attack_file = file_location + 2;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }

            attack_rank = rank + 1;
            attack_rank = attack_rank - player->rank_direction;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }
            
            attack_file = file_location - 2;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                return true;
            }
        }
    }
    //std::cout << "Nothing here\n";
    return false;
}

bool AI::EmptySpace(int file_num, int rank)
{
    if(file_num < 0 || file_num > 7 || rank > 7 || rank < 0)
    {
        return false;
    }

    if(FEN_board[rank][file_num] == '.')
    {
        return true;
    }

    return false;
}

void AI::GenerateFENArray()
{
    std::string fen = game->fen;

    for(int rank = 7; rank >= 0; --rank)
    {
        for(int i = 0; i < 8; ++i)
        {
            FEN_board[rank][i] = '0';
        }
    }

    int piece_num = 0;

    for(int rank = 7; rank >= 0; --rank)
    {
        int slash = fen.find('/');
        if(slash == -1)
        {
            slash = fen.find(' ');
        }
        int column = 0;
        for(int i = 0; i < slash; ++i)
        {
            if(isdigit(fen[i]))
            {
                int empty_spaces = fen[i] - '0';
                for(int j = 0; j < empty_spaces; ++j)
                {
                    FEN_board[rank][j + column] = '.';
                    ++piece_num;
                }
                column += empty_spaces;
            }
            else
            {
                FEN_board[rank][column] = fen[i];
                ++column;
                ++piece_num;
            }
        }
        fen = fen.substr(slash+1);
    }
}

void AI::PrintFENArray()
{
    std::cout << "++++++++++++++++++++\n";
    for(int rank = 7; rank >= 0; --rank)
    {
        for(int i = 0; i < 8; ++i)
        {
            std::cout << FEN_board[rank][i];
            std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "++++++++++++++++++++\n";
}

bool AI::OpponentLocated(int file_num, int rank)
{
    if(file_num < 0 || file_num > 7 || rank > 7 || rank < 0)
    {
        return false;
    }

    if(islower(FEN_board[rank][file_num]) && !player_lower_case &&
       FEN_board[rank][file_num] != '.')
    {
        return true;
    }
    else if(!islower(FEN_board[rank][file_num]) && player_lower_case &&
            FEN_board[rank][file_num] != '.')
    {
        return true;
    }

    return false;
}

} // chess

} // cpp_client
