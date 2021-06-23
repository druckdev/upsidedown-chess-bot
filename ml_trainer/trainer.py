import subprocess, time, os, sys, errno

path_to_executable = "../build/bot"
cmd = [ path_to_executable ]

class Trainer:
    def __init__(self):
        self.current_move = 0
        self.time_left = 100.0
        self.fen_state = "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr"
        self.simplified_state = ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R', 
                                 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 '1', '1', '1', '1', '1', '1', '1', '1',
                                 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
                                 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' ]

    #--------------
    # Interface
    #--------------
    def train(self, instances=2):
        # setup first generation
        processes = []
        for i in range(instances):
            p = subprocess.Popen(cmd,
                                 stdin=subprocess.PIPE,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
            processes.append(p)

        # init game
        b_player = processes[0]
        w_player = processes[1] 
        white_turn = True

        # play game
        while True: 
            self.current_move += 1
            current_player = w_player if white_turn else b_player
            print("--------------------------------------------------------")
            print(("White :" if white_turn else "Black :"), current_player)

            # setup input for bot
            in_str = self.fen_state 
            in_str += ' w ' if white_turn else ' b ' 
            in_str += str(self.time_left) 
            in_str += ' ' + str(self.current_move)
            state_bytes = bytes(in_str, 'utf-8')
            print("trigger bot with ", in_str)

            # trigger bot
            try:
                current_player.stdin.write(state_bytes)
                current_player.stdin.flush()
            
            except IOError as e:
                # expected error when bot terminates because they lost
                if e.errno == errno.EPIPE:
                    break
                else:
                    print(e)
                    raise

            # get move current player wants to play and execute it
            for line in iter(current_player.stdout.readline, b''):
                self.do_move(line)
                print("new state ", self.fen_state)
                break
            
            white_turn = not white_turn

    #--------------
    # Helper
    #--------------

    # executes the move on the classes fen_state and simplified_state
    def do_move(self, move: str):
        # deconstruct move string
        move_split = str(move).split("'")[1] # casting because move contains ' as character
        move_split = move_split.split(",")
        from_pos, to_pos = int(move_split[0]), int(move_split[1])
        print("Move ", move, " or from ", from_pos, " to ", to_pos)

        # do move
        self.simplified_state[to_pos] = self.simplified_state[from_pos]
        self.simplified_state[from_pos] = '1'

        self.update_fen()

    # updates the classes fen string from it's simplified state
    def update_fen(self):
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
        
        self.fen_state = new_fen

if __name__ == "__main__":
    trainer = Trainer()
    trainer.train()