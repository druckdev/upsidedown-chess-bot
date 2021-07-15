import errno
import time

# Description :
# GameRunner runs a game between two given processes.


class GameRunner:
    def __init__(self, w_player, b_player):
        self.w_player = w_player
        self.b_player = b_player

        # TODO : all the following needs to be wrapped or parsed
        self.time_left_w = 45.0 # game time in seconds
        self.time_left_b = 45.0 # game time in seconds

        self.current_move = 0
        self.max_moves = 60

        self.fen_state = "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr"
        self.simplified_state = ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
                                 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
                                 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r']

    # --------------
    # Interface
    # --------------

    def run(self):
        """Executes a game between two players

        Returns:
        bool: Whether it was a draw.
        bool: Whether white won that game.

        """

        white_won = False
        white_turn = True

        # play game
        while self.current_move < self.max_moves:
            self.current_move += 1
            print("Move ", self.current_move, " of ", self.max_moves, end="\r")

            # get variables for current player
            current_player = self.w_player if white_turn else self.b_player
            player_char = ' w ' if white_turn else ' b '
            time_left = self.time_left_w if white_turn else self.time_left_b

            # player lost on time (happened in last turn)
            if time_left <= 0:
                self.handle_game_end(white_turn, True)
                break

            # setup input for bot
            in_str = self.fen_state
            in_str += player_char
            in_str += str(self.current_move)
            in_str += ' ' + str(time_left)
            state_bytes = bytes(in_str, 'utf-8')

            # trigger bot (also start the timer)
            move_start_time = time.time()
            try:
                current_player.stdin.write(state_bytes)
                current_player.stdin.flush()

            except IOError as e:
                # expected error when bot terminates because they lost
                if e.errno == errno.EPIPE:
                    white_won = self.handle_game_end(white_turn, False)
                    break
                else:
                    print(e)
                    raise

            # get move current player wants to play and execute it
            for line in iter(current_player.stdout.readline, b''):

                # bot is done calculating the move, so compute the used time
                # for the current player
                move_end_time = time.time()
                used_time = move_end_time - move_start_time
                if white_turn:
                    self.time_left_w -= used_time
                else:
                    self.time_left_b -= used_time

                self.do_move(line)
                break

            white_turn = not white_turn

        self.print_board()
        print("white_won ", white_won, " draw ",
              self.current_move >= self.max_moves)

        return (self.current_move >= self.max_moves), white_won

    # --------------
    # Helper
    # --------------

    def handle_game_end(self, white_turn: bool, lost_on_time: bool) -> bool:
        """Get who won the game and as a sideeffect terminate the remaining process.

        Parameters:
        white_turn (bool):  Whether the move on which the end was detected was 
                            whites to play.
        lost_on_time (bool): Whether the game was lost on time.

        Returns:
        bool: Whether white won.

        """

        if lost_on_time :
            self.b_player.kill()
            self.w_player.kill()
        elif white_turn:
            self.b_player.kill()
        else:
            self.w_player.kill()

        return False if white_turn else True

    def do_move(self, move: str):
        """Executes a given move on the internal board representations

        Parameters:
        move (str): The move as it's read from stdout.

        """

        # deconstruct move string
        # casting because move contains ' as character
        move_split = str(move).split("'")[1]
        move_split = move_split.split(",")
        from_pos, to_pos = int(move_split[0]), int(move_split[1])

        # catch promotion
        moving_piece = self.simplified_state[from_pos]
        if (to_pos < 8 or to_pos > 56) and (moving_piece == 'p' or moving_piece == 'P'):
            moving_piece = move_split[2][0]

        # do move
        self.simplified_state[to_pos] = moving_piece
        self.simplified_state[from_pos] = '1'

        self.update_fen()

    # updates the classes fen string from it's simplified state
    def update_fen(self):
        """Updates the internal fen-string based on the simplified state"""

        new_fen = ""
        cnt = 0
        for i in range(len(self.simplified_state)):
            c = self.simplified_state[i]
            if c == '1':
                cnt += 1
            else:
                unoccupied = str(cnt) if cnt != 0 else ''
                cnt = 0
                new_fen = new_fen + unoccupied + c

            if i % 8 == 7 and i != 63:
                unoccupied = str(cnt) if cnt != 0 else ''
                cnt = 0
                new_fen = new_fen + unoccupied + '/'

        if cnt != 0:
            new_fen = new_fen + str(cnt)

        self.fen_state = new_fen

    def print_board(self):
        """Prints the internal 'simplified_state' but human-readable."""

        print(self.fen_state)

        tmp = ""
        for i in range(len(self.simplified_state)):
            if i % 8 == 0:
                print(tmp)
                tmp = ""

            c = ' ' if self.simplified_state[i] == '1' else self.simplified_state[i]
            tmp += '[ ' + c + ' ]'

        print(tmp)
