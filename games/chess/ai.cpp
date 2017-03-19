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
    srand(time(NULL));

    //Initialize variables
    player_lower_case = false;
    en_passant_file = -1;
    en_passant_rank = -1;

    //Call generate_FEN_array to genarate the double array
    //that will hold the board status
    generate_FEN_array();

    //Determine if the current player is upper or lower case
    //in the FEN string by looking at a current piece
    Piece temp = player->pieces[0];
    int file_num = temp->file[0] - 'a';
    char my_piece = FEN_board[temp->rank - 1][file_num];

    if(islower(my_piece))
    {
        player_lower_case = true;
    }
    me_lower_case = player_lower_case;
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
    player_lower_case = me_lower_case;

    current_score = score_board(FEN_board);

    //Set up current state of the board
    node board_start;

    //Set the move's FEN board to the original one
    for(int i = 0; i <8; ++i)
    {
        for(int j =0; j < 8; ++j)
        {
            board_start.current_FEN[i][j] = FEN_board[i][j];
        }
    }

    board_start.old_file = -1;
    board_start.new_file = -1;
    board_start.old_rank = -1;
    board_start.new_rank = -1;
    board_start.end_score = score_board(FEN_board);
    if(player_lower_case)
    {
        board_start.is_white = false;
    }
    else
    {
        board_start.is_white = true;
    }

    // 4) Generate all the possible moves by looking at each piece and
    //finding its specific moves
    find_possible_moves();
    board_start.next_moves = possible_moves;

    bool end_game_found = false;
    int limit = 0;
    while(!end_game_found)
    {
        ++limit;

        //DFGS starting at the top board using the
        //calculated depth limit
        if(limit > 4 || explore_moves(limit, &board_start) != current_score)
            end_game_found = true;
    }

    int move_num = find_move_number(board_start);
    Piece piece_to_move;
    std::string str;
    str = find_file(board_start.next_moves[move_num].old_file);

    //Find the piece to move
    for(int i = 0; i < player->pieces.size(); ++i)
    {
        if(player->pieces[i]->rank == board_start.next_moves[move_num].old_rank &&
           player->pieces[i]->file == str)
        {
            piece_to_move = player->pieces[i];
            break;
        }
    }

    str = find_file(board_start.next_moves[move_num].new_file);

    piece_to_move->move(str,
                        board_start.next_moves[move_num].new_rank,
                        board_start.next_moves[move_num].promotion);
    std::cout << "------------------------\n";


    //Clear moves so we start fresh next turn
    possible_moves.clear();
    return true; // to signify we are done with our turn.
}

int AI::explore_moves(int limit, node *start_board)
{
    int move_index = 0;
    std::cout << "+++++ 1 +++++\n";


    if(limit <= 0)
    {
        int score = score_board(start_board->current_FEN);
        if(insufficient_material(start_board->current_FEN))
        {
            if(start_board->is_white)
            {
                score += 100;
            }
            else
            {
                score += -100;
            }
        }
        return score;
    }
    std::cout << "+++++ 2 +++++\n";


    if(start_board->is_white)
    {
        player_lower_case = true;
    }
    else
    {
        player_lower_case = false;
    }

    if(!start_board->next_moves.empty())
    {
        std::cout << "+++++ 4 +++++\n";

        for(int z = 0; z < start_board->next_moves.size(); ++z)
        {
            //Clear the tracked boards for the next search
            possible_moves.clear();

            //node current_move = start_board->next_moves[z];

            //Set the move's FEN board to the original one
            for(int i = 0; i < 8; ++i)
            {
                for(int j = 0; j < 8; ++j)
                {
                    FEN_board[i][j] = start_board->next_moves[z].current_FEN[i][j];
                }
            }

            find_possible_moves();

            start_board->next_moves[z].next_moves = possible_moves;

            start_board->next_moves[z].end_score = explore_moves(limit - 1, &start_board->next_moves[z]);
        }

        sort(start_board->next_moves.begin(), start_board->next_moves.end(), sortNodes());

        int move_num = find_move_number(*start_board);
        std::cout << "+++++ 5 +++++\n";
        std::cout << "move_num: " << move_num << '\n';
        start_board->end_score = start_board->next_moves[move_num].end_score;
        std::cout << "+++++ 6 +++++\n";
    }
    else
    {
        //Loop through until we find the king so we can see if its in check
        bool checked = false;
        for(int i = 0; i < 8; ++i)
        {
            for(int j = 0; j < 8; ++j)
            {
                if((start_board->current_FEN[i][j] == 'k'
                    && !start_board->is_white) ||
                   (start_board->current_FEN[i][j] == 'K'
                    && start_board->is_white))
                {
                    //Found the king, if it is safe with this move,
                    //make the move.  Otherwise, loop back through
                    //and try a different move.
                    if(would_space_check(j, i))
                    {
                        if(start_board->is_white)
                        {
                            start_board->end_score = 100;
                        }
                        else
                        {
                            start_board->end_score = -100;
                        }
                        checked = true;
                    }
                }
            }
        }
        if(checked == false)
        {
            if(start_board->is_white)
            {
                start_board->end_score = -100;
            }
            else
            {
                start_board->end_score = 100;
            }
        }
    }
    std::cout << "+++++ 7 +++++\n";

    return start_board->end_score;
}

int AI::find_move_number(node game_board)
{
    int move_num = 0;

    if(!game_board.is_white)
    {
        move_num = game_board.next_moves.size() - 1;
    }

    int best_score = game_board.next_moves[move_num].end_score;
    bool found_move = false;
    int moves = 0;

    for(int i = 0; i < game_board.next_moves.size(); ++i)
    {
        if(game_board.next_moves[i].end_score == best_score)
        {
            ++moves;
        }
    }

    if(moves > 1)
    {
        int random = rand() % moves;

        if(!game_board.is_white)
        {
            move_num -= random;
        }
        else
        {
            move_num += random;
        }
    }

    return move_num;
}

void AI::find_possible_moves()
{
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

    for(int x = 0; x < possible_moves.size(); ++x)
    {
        //Reset the FEN_board to appear as if the move has been
        //made in order to see check status
        for(int i = 0; i < 8; ++i)
        {
            for(int j = 0; j < 8; ++j)
            {
                FEN_board[i][j] = possible_moves[x].current_FEN[i][j];
            }
        }
        
        //Loop through until we find the king so we can see if its in check
        for(int i = 0; i < 8; ++i)
        {
            for(int j = 0; j < 8; ++j)
            {
                if((possible_moves[x].current_FEN[i][j] == 'k'
                     && player_lower_case) ||
                    (possible_moves[x].current_FEN[i][j] == 'K'
                     && !player_lower_case))
                {
                    //Found the king, if it is safe with this move,
                    //make the move.  Otherwise, loop back through
                    //and try a different move.
                    if(would_space_check(j, i))
                    {
                        possible_moves.erase(possible_moves.begin() + x);
                        --x;
                    }
                }
            }
        }
    }
}

bool AI::insufficient_material(char board_to_score[8][8])
{
    int white_count = 0;
    int black_count = 0;

    for(int i = 0; i < 8; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            if(board_to_score[i][j] == 'P' || board_to_score[i][j] == 'N' || board_to_score[i][j] == 'B' ||
               board_to_score[i][j] == 'R' || board_to_score[i][j] == 'Q' || board_to_score[i][j] == 'K')
            {
                ++white_count;
            }
            else if(board_to_score[i][j] == 'p' || board_to_score[i][j] == 'n' || board_to_score[i][j] == 'b' ||
                    board_to_score[i][j] == 'r' || board_to_score[i][j] == 'q' || board_to_score[i][j] == 'k')
            {
                ++black_count;
            }
        }
    }

    if(black_count <= 2 && white_count <= 2)
    {
        return true;
    }

    return false;
}

int AI::score_board(char board_to_score[8][8])
{
    int white_score = 0;
    int black_score = 0;
    int score = 0;

    for(int i = 0; i < 8; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            if(board_to_score[i][j] == 'P')
            {
                white_score += 1;
            }
            else if(board_to_score[i][j] == 'p')
            {
                black_score += 1;
            }
            else if(board_to_score[i][j] ==  'N' || board_to_score[i][j] == 'B')
            {
                white_score += 3;
            }
            else if(board_to_score[i][j] == 'n' || board_to_score[i][j] == 'b')
            {
                black_score += 3;
            }
            else if(board_to_score[i][j] == 'R')
            {
                white_score += 5;
            }
            else if(board_to_score[i][j] == 'r')
            {
                black_score += 5;
            }
            else if(board_to_score[i][j] == 'Q')
            {
                white_score += 9;
            }
            else if(board_to_score[i][j] == 'q')
            {
                black_score += 9;
            }
        }
    }

    score = white_score - black_score;
    return score;
}

//Finds the possible moves of a given pawn on the current
//game board by looking at it's rank and file_num and the
//open spaces around it.
void AI::move_pawn(int rank, int file_num)
{
    int move_location = 0;
    std::string pawn_file;
    //Find the letter of the pawn file by converting to ascii
    pawn_file = file_num + 97;

    //Move the pawn in the player's direction by 1
    move_location = rank + player->rank_direction;

    //If the pawn is in the original starting spot, it may move
    //two spaces if both are empty
    if((rank + 1 == 7 && player_lower_case) ||
       (rank + 1 == 2 && !player_lower_case))
    {
        if(empty_space(file_num, move_location) &&
           empty_space(file_num, move_location + player->rank_direction))
        {
            set_up_move(file_num, file_num, rank, move_location + player->rank_direction, "");
        }
    }

    //Move the pawn forward one space if it is empty
    if(empty_space(file_num, move_location))
    {
        if(move_location + player->rank_direction > 7 ||
           move_location + player->rank_direction < 0)
        {
            //If the pawn reached the edge of the board, promote it
            promote_pawn(file_num, file_num, rank, move_location);
        }
        else
        {
            set_up_move(file_num, file_num, rank, move_location, "");
        }
    }

    //If the pawn can move to an en passant square OR there is an
    //opponent diagonal from it, it can move to the side one and forward one
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

    //If the pawn can move to an en passant square OR there is an
    //opponent diagonal from it, it can move to the side one and forward one
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

//Finds the possible moves of the king on the current
//game board by looking at it's rank and file_num and the
//open spaces around it that would not result in a check.
void AI::move_king(int rank, int file_num)
{
    int move_location = rank;
    int final_file;

    //The king can move to the right one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    final_file = file_num + 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
       !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    //The king can move to the left one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    final_file = file_num - 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    //The king can move diagonal one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    move_location = rank + player->rank_direction;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    //The king can move diagonal one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    final_file = file_num + 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    //The king can move forward one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    if((empty_space(file_num, move_location) ||
       opponent_located(file_num, move_location)) &&
        !would_space_check(file_num, move_location))
    {
        set_up_move(file_num, file_num, rank, move_location, "");
    }

    //The king can move diagonal one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    move_location = rank - player->rank_direction;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    //The king can move diagonal one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    final_file = file_num - 1;
    if((empty_space(final_file, move_location) ||
       opponent_located(final_file, move_location)) &&
        !would_space_check(final_file, move_location))
    {
        set_up_move(file_num, final_file, rank, move_location, "");
    }

    //The king can move backwards one if there is an
    //opponent there or the space is empty, and this would
    //not put the king in check
    if((empty_space(file_num, move_location) ||
       opponent_located(file_num, move_location)) &&
        !would_space_check(file_num, move_location))
    {
        set_up_move(file_num, file_num, rank, move_location, "");
    }

    //Check to see if kingside or queenside castling is allowed
    check_for_castling(rank, file_num);
}

//Finds the possible moves of a given rook or the queen on the current
//game board by looking at it's rank and file_num and the
//open spaces around it.
void AI::move_rook_or_queen(int rank, int file_num)
{
    int move_location = rank;
    std::string final_file;

    //Add every space to the right of the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

    //Add every space to the left of the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

    //Add every space in front of the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

    //Add every space behind the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

//Finds the possible moves of a given bishop or the queen on the current
//game board by looking at it's rank and file_num and the
//open spaces around it.
void AI::move_bishop_or_queen(int rank, int file_num)
{
    int move_location = rank;
    int file_location = file_num;
    std::string final_file;

    //Add every space diagonally behind to the right of the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

    //Add every space diagonally in front of to the right of the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

    //Add every space diagonally behind to the left of the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

    //Add every space diagonally in front of to the left of the piece to
    //the possible moves space if it can get there without
    //hitting another piece
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

//Finds the possible moves of a given knight on the current
//game board by looking at it's rank and file_num and the
//open spaces around it.
void AI::move_knight(int rank, int file_num)
{
    int move_location = rank;
    int file_location = file_num;
    std::string final_file;

    //L shaped move to the right 2 and behind 1
    final_file = file_location + 2;
    move_location -= player->rank_direction;
    if(empty_space(file_location + 2, move_location) ||
       opponent_located(file_location + 2, move_location))
    {
        set_up_move(file_num, file_location + 2, rank, move_location, "");
    }

    //L shaped move to the right 2 and in front 1
    move_location = rank + player->rank_direction;
    if(empty_space(file_location + 2, move_location) ||
       opponent_located(file_location + 2, move_location))
    {
        set_up_move(file_num, file_location + 2, rank, move_location, "");
    }

    //L shaped move to the right 1 and in front 2
    final_file = file_location + 1;
    move_location = rank + player->rank_direction + player->rank_direction;
    if(empty_space(file_location + 1, move_location) ||
       opponent_located(file_location + 1, move_location))
    {
        set_up_move(file_num, file_location + 1, rank, move_location, "");
    }

    //L shaped move to the right 1 and behind 2
    move_location = rank - player->rank_direction - player->rank_direction;
    if(empty_space(file_location + 1, move_location) ||
       opponent_located(file_location + 1, move_location))
    {
        set_up_move(file_num, file_location + 1, rank, move_location, "");
    }

    //L shaped move to the left 2 and behind 1
    final_file = file_location - 2;
    move_location = rank - player->rank_direction;
    if(empty_space(file_location - 2, move_location) ||
       opponent_located(file_location - 2, move_location))
    {
        set_up_move(file_num, file_location - 2, rank, move_location, "");
    }

    //L shaped move to the left 2 and in front 1
    move_location = rank + player->rank_direction;
    if(empty_space(file_location - 2, move_location) ||
       opponent_located(file_location - 2, move_location))
    {
        set_up_move(file_num, file_location - 2, rank, move_location, "");
    }

    //L shaped move to the left 1 and in front 2
    final_file = file_location - 1;
    move_location = rank + player->rank_direction + player->rank_direction;
    if(empty_space(file_location - 1, move_location) ||
       opponent_located(file_location - 1, move_location))
    {
        set_up_move(file_num, file_location - 1, rank, move_location, "");
    }

    //L shaped move to the left 1 and behind 2
    move_location = rank - player->rank_direction - player->rank_direction;
    if(empty_space(file_location - 1, move_location) ||
       opponent_located(file_location - 1, move_location))
    {
        set_up_move(file_num, file_location - 1, rank, move_location, "");
    }
}

//If the pawn moved to the edge of the board, it should be promoted to
//a queen, rook, bishop, or knight at random, set up the corresponding move
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

//This function sets up a move given a pieces old location and new location
//by storing both those and the possible pawn promotion in a small structure
//to be referenced later when deciding which move to make
void AI::set_up_move(int old_f, int new_f, int old_r,
                   int new_r, std::string promo)
{
    node move_to_make;

    //Set the move's FEN board to the original one
    for(int i = 0; i <8; ++i)
    {
        for(int j =0; j < 8; ++j)
        {
            move_to_make.current_FEN[i][j] = FEN_board[i][j];
        }
    }

    //Make the move on the FEN board (in the array)
    move_to_make.current_FEN[new_r][new_f] = FEN_board[old_r][old_f];
    move_to_make.current_FEN[old_r][old_f] = '.';

    //Set up the old and new locations in the structure for later look
    //up and moving of the piece
    move_to_make.old_file = old_f;
    move_to_make.new_file = new_f;
    move_to_make.old_rank = old_r + 1;
    move_to_make.new_rank = new_r + 1;
    move_to_make.promotion = promo;

    //Will the next player be white?
    //Move made was by black, set the board to white
    //or visa versa
    move_to_make.is_white = player_lower_case;

    //Add the move to the list of possible moves
    possible_moves.push_back(move_to_make);
}

char AI::find_file(int file_num)
{
    if(file_num == 0)
    {
        return 'a';
    }
    else if(file_num == 1)
    {
        return 'b';
    }
    else if(file_num == 2)
    {
        return 'c';
    }
    else if(file_num == 3)
    {
        return 'd';
    }
    else if(file_num == 4)
    {
        return 'e';
    }
    else if(file_num == 5)
    {
        return 'f';
    }
    else if(file_num == 6)
    {
        return 'g';
    }
    else if(file_num == 7)
    {
        return 'h';
    }
}

//Determine if the king is in check while in the current given location
bool AI::would_space_check(int file_num, int rank)
{
    //If given a bad location, return true since this is not
    //an allowed possible move
    if(file_num < 0 || file_num > 7 || rank > 7 || rank < 0)
    {
        return true;
    }

    //Loop through all the spaces on the board and find opponent pieces
    //that may have the king in check
    for(int k = 0; k < 8; ++k)
    {
        for(int j = 0; j < 8; ++j)
        {
            int attack_rank = rank;
            int attack_file;

            //If the opponent piece (opposite case of the current player) is a pawn, bishop,
            //king, or queen and it is in either of the two diagonal spaces in front of the king,
            //the king is in check OR if it is a bishop, king, or queen and in on of the diagonal
            //spaces behind the king, the king is in check
            if(((FEN_board[k][j] == 'P' || FEN_board[k][j] == 'K' ||
               FEN_board[k][j] == 'B' || FEN_board[k][j] == 'Q') && player_lower_case) ||
               ((FEN_board[k][j] == 'p' || FEN_board[k][j] == 'k' ||
                 FEN_board[k][j] == 'b' || FEN_board[k][j] == 'q') && !player_lower_case))
            {
                attack_file = file_num + 1;
                if((j == attack_file && k == attack_rank + player->rank_direction &&
                    (FEN_board[k][j] != 'P' || FEN_board[k][j] != 'p')) ||
                   (j == attack_file && k == attack_rank - player->rank_direction))
                {
                    return true;
                }
                attack_file = file_num - 1;
                if((j == attack_file && k == attack_rank + player->rank_direction &&
                    (FEN_board[k][j] != 'P' || FEN_board[k][j] != 'p')) ||
                   (j == attack_file && k == attack_rank - player->rank_direction))
                {
                    return true;
                }
            }

            //If the opponent piece is a rook, bishop, king, or queen
            //and it is in any of the squares directly in front of, behind,
            //to the right or left of the king, the king is in check
            attack_file = file_num;
            if(((FEN_board[k][j] == 'R' || FEN_board[k][j] == 'K' ||
               FEN_board[k][j] == 'Q') && player_lower_case) ||
               ((FEN_board[k][j] == 'r' || FEN_board[k][j] == 'k' ||
                 FEN_board[k][j] == 'q') && !player_lower_case))
            {
                if((j == attack_file && k == attack_rank + 1) ||
                   (j == attack_file && k == attack_rank - 1))
                {
                    return true;
                }
                attack_file = file_num - 1;
                if((j == attack_file && k == attack_rank))
                {
                    return true;
                }
                attack_file = file_num + 1;
                if((j == attack_file && k == attack_rank))
                {
                    return true;
                }
            }

            //If the opponent piece is a rook or queen
            //and it is in any of the squares in any straight direction
            //from the king without any other pieces between them, the king is in check
            if(((FEN_board[k][j] == 'R' || FEN_board[k][j] == 'Q') && player_lower_case) ||
               ((FEN_board[k][j] == 'r' || FEN_board[k][j] == 'q') && !player_lower_case))
            {
                //Check to the right of the king
                for(int i = 0; i < 8; ++i)
                {
                    attack_file = file_num + 1 + i;
                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(attack_file, rank))
                    {
                        break;
                    }
                }

                //Check to the left of the king
                for(int i = 0; i < 8; ++i)
                {
                    attack_file = file_num - 1 - i;
                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(attack_file, rank))
                    {

                        break;
                    }
                }

                //Check in front of the king
                attack_rank = rank;
                attack_file = file_num;
                for(int i = 0; i < 8; ++i)
                {
                    attack_rank += 1;
                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(file_num, attack_rank))
                    {
                        break;
                    }
                }

                //Check behind the king
                attack_rank = rank;
                for(int i = 0; i < 8; ++i)
                {
                    attack_rank -= 1;
                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(file_num, attack_rank))
                    {
                        break;
                    }
                }
            }

            //If the opponent piece is a bishop or queen
            //and it is in any of the squares in any diagonal direction
            //from the king without any other pieces between them, the king is in check
            attack_rank = rank;
            attack_file = file_num;
            if(((FEN_board[k][j] == 'B' || FEN_board[k][j] == 'Q') && player_lower_case) ||
               ((FEN_board[k][j] == 'b' || FEN_board[k][j] == 'q') && !player_lower_case))
            {
                //Check to the right and up
                for(int i = 0; i < 8; ++i)
                {
                    attack_file = file_num + 1 + i;
                    attack_rank += 1;
                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(attack_file, attack_rank))
                    {
                        break;
                    }
                }

                //Check to the left and up
                attack_rank = rank;
                for(int i = 0; i < 8; ++i)
                {
                    attack_file = file_num - 1 - i;
                    attack_rank += 1;
                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(attack_file, attack_rank))
                    {
                        break;
                    }
                }

                //Check to the right and down
                attack_rank = rank;
                for(int i = 0; i < 8; ++i)
                {
                    attack_file = file_num + 1 + i;
                    attack_rank -= 1;
                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(attack_file, attack_rank))
                    {
                        break;
                    }
                }

                //Check to the left and down
                attack_rank = rank;
                for(int i = 0; i < 8; ++i)
                {
                    attack_rank -= 1;
                    attack_file = file_num - 1 - i;

                    if((j == attack_file && k == attack_rank) ||
                       (j == attack_file && k == attack_rank))
                    {
                        return true;
                    }
                    else if(!empty_space(attack_file, attack_rank))
                    {
                        break;
                    }
                }
            }

            //If the king can be attacked by any opponent knight (from an L shape) then
            //the king is in check
            if((FEN_board[k][j] == 'N' && player_lower_case) ||
               (FEN_board[k][j] == 'n' && !player_lower_case))
            {
                //L shape to the left and up 2
                attack_file = file_num - 1;
                attack_rank = rank + player->rank_direction + player->rank_direction;
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }

                //L shape to the right and up 2
                attack_file = file_num + 1;
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }

                //L shape to the right and down 2
                attack_rank = rank - player->rank_direction - player->rank_direction;
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }

                //L shape to the left and down 2
                attack_file = file_num - 1;
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }

                //L shape to the left two and up
                attack_file = file_num - 2;
                attack_rank = rank + player->rank_direction;
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }
                
                attack_file = file_num + 2;
                //L shape to the right two and up
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }

                //L shape to the right 2 and down
                attack_rank = rank - player->rank_direction;
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }

                //L shape to the left 2 and down
                attack_file = file_num - 2;
                if((j == attack_file && k == attack_rank) ||
                   (j == attack_file && k == attack_rank))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

//Check if a kingside or queenside castling move is possible
void AI::check_for_castling(int rank, int file_num)
{
    for(int j = 0; j < player->pieces.size(); ++j)
    {
        Piece piece = player->pieces[j];

        //If the piece we are looking at is a rook and it is at the
        //same location as the king
        if(piece->type == "Rook" && piece->rank - 1 == rank)
        {
            int rook_file = piece->file[0] - 'a';

            //Check for kingside castling by determining if castling
            //was in the FEN string at the beginning of the turn
            if(((castling[0] == 'K' && !player_lower_case) ||
               (castling[2] == 'k' && player_lower_case)) &&
               rook_file > file_num)
            {
                bool kingside = true;
                int diff = abs(rook_file - file_num);
                //Determine if there are only empty spaces between
                //the king and the castle
                for(int i = 1; i < diff; ++i)
                {
                    if(!empty_space(file_num + i, rank))
                    {
                        kingside = false;
                    }
                }

                if(kingside == true)
                {
                    //Castling is possible, set up the move
                    set_up_move(file_num, file_num + 2, rank, rank, "");
                }
            }

            //Check for queenside castling by determining if castling
            //was in the FEN string at the beginning of the turn
            if(((castling[1] == 'Q' && !player_lower_case) ||
               (castling[3] == 'q' && player_lower_case)) &&
               rook_file < file_num)
            {
                bool queenside = true;
                int diff = abs(rook_file - file_num);
                //Determine if there are only empty spaces between
                //the king and the castle
                for(int i = 1; i < diff; ++i)
                {
                    if(!empty_space(file_num - i, rank))
                    {
                        queenside = false;
                    }
                }
                if(queenside == true)
                {
                    //Castling is possible, set up the move
                    set_up_move(file_num, file_num - 2, rank, rank, "");
                }
            }
        }
    }
}

//Is the given space of rank and file empty on the current board?
bool AI::empty_space(int file_num, int rank)
{
    //The space is off the board - return false - move not possible
    if(file_num < 0 || file_num > 7 || rank > 7 || rank < 0)
    {
        return false;
    }

    //If the array holds a '.' at the given location then the space is empty
    if(FEN_board[rank][file_num] == '.')
    {
        return true;
    }

    return false;
}

//Generate the FEN double array displaying the board
//by reading the current fen string
void AI::generate_FEN_array()
{
    //Initialize the array to 0s
    std::string fen = game->fen;
    for(int rank = 7; rank >= 0; --rank)
    {
        for(int i = 0; i < 8; ++i)
        {
            FEN_board[rank][i] = '0';
        }
    }

    int piece_num = 0;

    //Read the values between each forward slash and the next (or a space)
    //to determine the state of the board
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
            //If the given character is a digit, set up the empty spaces
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
            //Otherwise, record the piece character
            else
            {
                FEN_board[rank][column] = fen[i];
                ++column;
                ++piece_num;
            }
        }
        //Move to the next part
        fen = fen.substr(slash+1);
    }

    //Initialize the castling array to be empty
    for(int i = 0; i < 4; ++i)
    {
        castling[i] = '-';
    }

    //Initialize the en_passant variables to bad values
    en_passant_file = -1;
    en_passant_rank = -1;

    //Read what remains of the fen string
    int spaces_found = 0;
    while(spaces_found < 4)
    {
        int space = fen.find(' ');
        ++spaces_found;

        //Record the castlings that are possible
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

        //Record the space that an en passant can occur in
        if(spaces_found == 3)
        {
            if(fen[0] != '-')
            {
                en_passant_file = fen[0] - 'a';
                en_passant_rank = fen[1] - '0';
            }
        }
        //Move to the next part
        fen = fen.substr(space+1);
    }
}

//Determine if an opponent is located at the given location
bool AI::opponent_located(int file_num, int rank)
{
    //Return false if the values given are off the board
    if(file_num < 0 || file_num > 7 || rank > 7 || rank < 0)
    {
        return false;
    }

    //If the location has a character that is lower case (not '.') and the current
    //player is denoted by upper case characters, opponent is located there
    if(islower(FEN_board[rank][file_num]) && !player_lower_case &&
       FEN_board[rank][file_num] != '.')
    {
        return true;
    }

    //If the location has a character that is upper case (not '.') and the current
    //player is denoted by lower case characters, opponent is located there
    else if(!islower(FEN_board[rank][file_num]) && player_lower_case &&
            FEN_board[rank][file_num] != '.')
    {
        return true;
    }

    return false;
}

} // chess

} // cpp_client
