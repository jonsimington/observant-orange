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
    generate_FEN_array();
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
    // You can do any cleanup of your AI here.  The program ends when this function returns.
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{
    // Here is where you'll want to code your AI.
    generate_FEN_array();

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
                move_pawn(i, j);
            }

            if((FEN_board[i][j] == 'k' && player_lower_case) ||
               (FEN_board[i][j] == 'K' && !player_lower_case))
            {
                move_king(i, j);
            }

            if(((FEN_board[i][j] == 'r' || FEN_board[i][j] == 'q') && player_lower_case) ||
               ((FEN_board[i][j] == 'R' || FEN_board[i][j] == 'Q') && !player_lower_case))
            {
                move_rook_or_queen(i, j);
            }

            if(((FEN_board[i][j] == 'b' || FEN_board[i][j] == 'q') && player_lower_case) ||
               ((FEN_board[i][j] == 'B' || FEN_board[i][j] == 'Q') && !player_lower_case))
            {
                move_bishop_or_queen(i, j);
            }

            if((FEN_board[i][j] == 'n' && player_lower_case) ||
               (FEN_board[i][j] == 'N' && !player_lower_case))
            {
                move_knight(i, j);
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
                    std::cout << "Moving " << piece_to_move-> type << "\n";
                    std::cout << "Move to: " << possible_moves[move_number].new_file << " " << possible_moves[move_number].new_rank << '\n';
                    int ttt = possible_moves[move_number].new_file[0] - 'a';
                    std::cout << "-------- " << possible_moves[move_number].current_FEN[possible_moves[move_number].new_rank - 1][ttt] << " ----------\n";
                    if(!would_space_check(j, i))
                    {
                        piece_to_move->move(possible_moves[move_number].new_file,
                                        possible_moves[move_number].new_rank,
                                        possible_moves[move_number].promotion);
                        move_made = true;
                    }
                }
            }
        }

        possible_moves.erase(possible_moves.begin() + move_number);
    }

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

void AI::move_pawn(int rank, int file_num)
{
    int move_location = 0;
    std::string pawn_file;
    pawn_file = file_num + 97;

    move_location = rank + player->rank_direction;
    if((rank + 1 == 7 && player_lower_case) ||
       (rank + 1 == 2 && !player_lower_case))
    {
        if(empty_space(file_num, move_location) &&
           empty_space(file_num, move_location + player->rank_direction))
        {
            set_up_move(file_num, file_num, rank, move_location + player->rank_direction, "");
        }
    }
    else
    {
        if(empty_space(file_num, move_location))
        {
            if(move_location + player->rank_direction > 7 ||
               move_location + player->rank_direction < 0)
            {
                promote_pawn(file_num, file_num, rank, move_location);
            }
            else
            {
                set_up_move(file_num, file_num, rank, move_location, "");
            }
        }
    }

    int file_location = file_num + 1;
    if(opponent_located(file_location, move_location) ||
       (en_passant_file == file_location && en_passant_rank - 1 == move_location))
    {
        if(move_location + player->rank_direction > 7 ||
           move_location + player->rank_direction < 0)
        {
            promote_pawn(file_num, file_location, rank, move_location);
        }
        else
        {
            set_up_move(file_num, file_location, rank, move_location, "");
        }
    }

    file_location = file_num - 1;
    if(opponent_located(file_location, move_location) ||
       (en_passant_file == file_location && en_passant_rank - 1 == move_location))
    {
        if(move_location + player->rank_direction > 7 ||
           move_location + player->rank_direction < 0)
        {
            promote_pawn(file_num, file_location, rank, move_location);
        }
        else
        {
            set_up_move(file_num, file_location, rank, move_location, "");
        }
    }
}

void AI::move_king(int rank, int file_num)
{
    int move_location = rank;
    int final_file;

    final_file = file_num + 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
       !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    final_file = file_num - 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    move_location = rank + player->rank_direction;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    final_file = file_num + 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    if((empty_space(file_num, move_location) ||
       opponent_located(file_num, move_location)) &&
        !would_space_check(file_num, move_location))
    {
        set_up_move(file_num, file_num, rank, move_location, "");
    }

    move_location = rank - player->rank_direction;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    final_file = file_num - 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    if((empty_space(file_num, move_location) ||
       opponent_located(file_num, move_location)) &&
        !would_space_check(file_num, move_location))
    {
        set_up_move(file_num, file_num, rank, move_location, "");
    }

    check_for_castling(rank, file_num);
}

void AI::move_rook_or_queen(int rank, int file_num)
{
    int move_location = rank;
    std::string final_file;

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_num + 1 + i;
        if(empty_space(file_num + 1 + i, move_location))
        {
            set_up_move(file_num, file_num + 1 + i, rank, move_location, "");
        }
        else if(opponent_located(file_num + 1 + i, move_location))
        {
            set_up_move(file_num, file_num + 1 + i, rank, move_location, "");
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
        if(empty_space(file_num - 1 - i, move_location))
        {
            set_up_move(file_num, file_num - 1 - i, rank, move_location, "");
        }
        else if(opponent_located(file_num - 1 - i, move_location))
        {
            set_up_move(file_num, file_num - 1 - i, rank, move_location, "");
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
        if(empty_space(file_num, move_location))
        {
            set_up_move(file_num, file_num, rank, move_location, "");
        }
        else if(opponent_located(file_num, move_location))
        {
            set_up_move(file_num, file_num, rank, move_location, "");
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
        if(empty_space(file_num, move_location))
        {
            set_up_move(file_num, file_num, rank, move_location, "");
        }
        else if(opponent_located(file_num, move_location))
        {
            set_up_move(file_num, file_num, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }
}

void AI::move_bishop_or_queen(int rank, int file_num)
{
    int move_location = rank;
    int file_location = file_num;
    std::string final_file;

    for(int i = 0; i < 8; ++i)
    {
        final_file = file_location + 1 + i;
        move_location -= player->rank_direction;
        if(empty_space(file_location + 1 + i, move_location))
        {
            set_up_move(file_num, file_location + 1 + i, rank, move_location, "");
        }
        else if(opponent_located(file_location + 1 + i, move_location))
        {
            set_up_move(file_num, file_location + 1 + i, rank, move_location, "");
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
        if(empty_space(file_location + 1 + i, move_location))
        {
            set_up_move(file_num, file_location + 1 + i, rank, move_location, "");
        }
        else if(opponent_located(file_location + 1 + i, move_location))
        {
            set_up_move(file_num, file_location + 1 + i, rank, move_location, "");
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
        if(empty_space(file_location - 1 - i, move_location))
        {
            set_up_move(file_num, file_location - 1 - i, rank, move_location, "");
        }
        else if(opponent_located(file_location - 1 - i, move_location))
        {
            set_up_move(file_num, file_location - 1 - i, rank, move_location, "");
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
        if(empty_space(file_location - 1 - i, move_location))
        {
            set_up_move(file_num, file_location - 1 - i, rank, move_location, "");
        }
        else if(opponent_located(file_location - 1 - i, move_location))
        {
            set_up_move(file_num, file_location - 1 - i, rank, move_location, "");
            break;
        }
        else
        {
            break;
        }
    }
}

void AI::move_knight(int rank, int file_num)
{
    int move_location = rank;
    int file_location = file_num;
    std::string final_file;

    final_file = file_location + 2;
    move_location -= player->rank_direction;
    if(empty_space(file_location + 2, move_location) ||
       opponent_located(file_location + 2, move_location))
    {
        set_up_move(file_num, file_location + 2, rank, move_location, "");
    }

    move_location = rank + player->rank_direction;
    if(empty_space(file_location + 2, move_location) ||
       opponent_located(file_location + 2, move_location))
    {
        set_up_move(file_num, file_location + 2, rank, move_location, "");
    }

    final_file = file_location + 1;
    move_location = rank + player->rank_direction + player->rank_direction;
    if(empty_space(file_location + 1, move_location) ||
       opponent_located(file_location + 1, move_location))
    {
        set_up_move(file_num, file_location + 1, rank, move_location, "");
    }

    move_location = rank - player->rank_direction - player->rank_direction;
    if(empty_space(file_location + 1, move_location) ||
       opponent_located(file_location + 1, move_location))
    {
        set_up_move(file_num, file_location + 1, rank, move_location, "");
    }

    final_file = file_location - 2;
    move_location = rank - player->rank_direction;
    if(empty_space(file_location - 2, move_location) ||
       opponent_located(file_location - 2, move_location))
    {
        set_up_move(file_num, file_location - 2, rank, move_location, "");
    }

    move_location = rank + player->rank_direction;
    if(empty_space(file_location - 2, move_location) ||
       opponent_located(file_location - 2, move_location))
    {
        set_up_move(file_num, file_location - 2, rank, move_location, "");
    }
    
    final_file = file_location - 1;
    move_location = rank + player->rank_direction + player->rank_direction;
    if(empty_space(file_location - 1, move_location) ||
       opponent_located(file_location - 1, move_location))
    {
        set_up_move(file_num, file_location - 1, rank, move_location, "");
    }

    move_location = rank - player->rank_direction - player->rank_direction;
    if(empty_space(file_location - 1, move_location) ||
       opponent_located(file_location - 1, move_location))
    {
        set_up_move(file_num, file_location - 1, rank, move_location, "");
    }
}

void AI::promote_pawn(int file_num, int new_file, int old_rank, int new_rank)
{
    int random_move = rand() % 4;

    if(random_move == 0)
    {
        set_up_move(file_num, new_file, old_rank, new_rank, "Queen");
    }
    else if(random_move == 1)
    {
        set_up_move(file_num, new_file, old_rank, new_rank, "Rook");
    }
    else if(random_move == 2)
    {
        set_up_move(file_num, new_file, old_rank, new_rank, "Bishop");
    }
    else if(random_move == 3)
    {
        set_up_move(file_num, new_file, old_rank, new_rank, "Knight");
    }
}

void AI::set_up_move(int old_file, int new_file, int old_rank,
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

bool AI::would_space_check(int file_num, int rank)
{
    if(file_num < 0 || file_num > 7 || rank > 7 || rank < 0)
    {
        return false;
    }

    for(int j = 0; j < 8; ++j)
    {
        int attack_rank = rank + 1;
        std::string attack_file;
        int file_location = file_num + 97;
        attack_file = file_location;
        Piece piece = player->opponent->pieces[j];

        if(piece->type == "Pawn" || piece->type == "King" ||
           piece->type == "Bishop" || piece->type == "Queen")
        {
            attack_file = file_location + 1;
            if((piece->file == attack_file && piece->rank == attack_rank + 1) ||
               (piece->file == attack_file && piece->rank == attack_rank - 1))
            {
                std::cout << "1" << '\n';
                return true;
            }
            attack_file = file_location - 1;
            if((piece->file == attack_file && piece->rank == attack_rank + 1) ||
               (piece->file == attack_file && piece->rank == attack_rank - 1))
            {
                std::cout << "2" << '\n';

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
                std::cout << piece->type << '\n';
                std::cout << "3" << '\n';

                return true;
            }
            attack_file = file_location - 1;
            if((piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "4" << '\n';

                return true;
            }
            attack_file = file_location + 1;
            if((piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "5" << '\n';

                return true;
            }
        }

        if(piece->type == "Rook" || piece->type == "Queen")
        {
            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location + 1 + i;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "6" << '\n';

                    return true;
                }
                else if(!empty_space(file_num + 1 + i, rank))
                {
                    break;
                }
            }

            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location - 1 - i;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "7" << '\n';

                    return true;
                }
                else if(!empty_space(file_num - 1 - i, rank))
                {

                    break;
                }
            }

            attack_rank = rank + 1;
            attack_file = file_location;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank += 1;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "8" << '\n';

                    return true;
                }
                else if(!empty_space(file_num, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank -= 1;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "9" << '\n';

                    return true;
                }
                else if(!empty_space(file_num, attack_rank - 1))
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
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "10" << '\n';

                    return true;
                }
                else if(!empty_space(file_num + 1 + i, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location - 1 - i;
                attack_rank += 1;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "11" << '\n';

                    return true;
                }
                else if(!empty_space(file_num - 1 - i, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_file = file_location + 1 + i;
                attack_rank -= 1;
                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "12" << '\n';

                    return true;
                }
                else if(!empty_space(file_num + 1 + i, attack_rank - 1))
                {
                    break;
                }
            }

            attack_rank = rank + 1;
            for(int i = 0; i < 8; ++i)
            {
                attack_rank -= 1;
                attack_file = file_location - 1 - i;

                if((piece->file == attack_file && piece->rank == attack_rank) ||
                   (piece->file == attack_file && piece->rank == attack_rank))
                {
                    std::cout << "13" << '\n';

                    return true;
                }
                else if(!empty_space(file_num - 1 - i, attack_rank - 1))
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
                std::cout << "14" << '\n';

                return true;
            }

            attack_file = file_location + 1;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "15" << '\n';

                return true;
            }

            attack_rank = rank + 1;
            attack_rank = attack_rank - player->rank_direction - player->rank_direction;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "16" << '\n';

                return true;
            }

            attack_file = file_location - 1;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "17" << '\n';

                return true;
            }

            attack_rank = rank + 1;
            attack_file = file_location - 2;
            attack_rank = attack_rank + player->rank_direction;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "18" << '\n';

                return true;
            }
            
            attack_file = file_location + 2;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "19" << '\n';

                return true;
            }

            attack_rank = rank + 1;
            attack_rank = attack_rank - player->rank_direction;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "20" << '\n';

                return true;
            }
            
            attack_file = file_location - 2;
            if((piece->file == attack_file && piece->rank == attack_rank) ||
               (piece->file == attack_file && piece->rank == attack_rank))
            {
                std::cout << "21" << '\n';

                return true;
            }
        }
    }
    return false;
}

void AI::check_for_castling(int rank, int file_num)
{
    for(int j = 0; j < player->pieces.size(); ++j)
    {
        Piece piece = player->pieces[j];

        if(piece->type == "Rook" && piece->rank - 1 == rank)
        {
            int rook_file = piece->file[0] - 'a';

            if(((castling[0] == 'K' && !player_lower_case) ||
               (castling[2] == 'k' && player_lower_case)) &&
               rook_file > file_num)
            {
                bool kingside = true;
                int diff = abs(rook_file - file_num) - 1;
                for(int i = 1; i < diff; ++i)
                {
                    if(!empty_space(file_num + i, rank))
                    {
                        kingside = false;
                    }
                }
                if(kingside == true)
                {
                    set_up_move(file_num, file_num + 2, rank, rank, "");
                }
            }

            if(((castling[1] == 'Q' && !player_lower_case) ||
               (castling[3] == 'q' && player_lower_case)) &&
               rook_file < file_num)
            {
                bool queenside = true;
                int diff = abs(rook_file - file_num) - 1;
                for(int i = 1; i < diff; ++i)
                {
                    if(!empty_space(file_num - i, rank))
                    {
                        queenside = false;
                    }
                }
                if(queenside == true)
                {
                    set_up_move(file_num, file_num - 2, rank, rank, "");
                }
            }
        }
    }
}

bool AI::empty_space(int file_num, int rank)
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

void AI::generate_FEN_array()
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

    for(int i = 0; i < 4; ++i)
    {
        castling[i] = '-';
    }

    en_passant_file = -1;
    en_passant_rank = -1;

    int spaces_found = 0;
    while(spaces_found < 4)
    {
        int space = fen.find(' ');
        ++spaces_found;

        if(fen[0] == 'K')
        {
            castling[0] = 'K';
        }
        if(fen[1] == 'Q')
        {
            castling[1] = 'Q';
        }
        if(fen[2] == 'k')
        {
            castling[2] = 'k';
        }
        if(fen[3] == 'q')
        {
            castling[3] = 'q';
        }

        if(spaces_found == 3)
        {
            if(fen[0] != '-')
            {
                en_passant_file = fen[0] - 'a';
                en_passant_rank = fen[1] - '0';
            }
        }
        fen = fen.substr(space+1);
    }
}

bool AI::opponent_located(int file_num, int rank)
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
