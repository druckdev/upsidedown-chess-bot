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
if __name__ == "__main__":
    trainer = Trainer()
    trainer.train()