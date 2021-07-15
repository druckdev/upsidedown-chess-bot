import game_runner
import subprocess
from threading import Thread
from threading import Lock
import time
import shlex

# Description :
# A tournament uses a generation (list of bot fitting configs)
# to create bot processes which play against each other, to
# determine which configs are superior.

# TODO : this needs to be wrapped or parsed somehow
path_to_executable = "../build/bot"
config_path = "../src/param_config.c"


class Tournament:
    def __init__(self, player_configs):
        self.player_configs = player_configs
        self.num_of_players = len(self.player_configs)
        self.wins_by_player = [0] * self.num_of_players
        self.running_games = []
    
    #--------------
    # Interface
    # --------------

    def run_tournament(self):
        """Runs a tournament between all players

        Returns:
        array: Contains the performance of each player.

        """

        # let all players play against all others
        for i in range(self.num_of_players):
            for j in range(self.num_of_players):
                if i == j:
                    continue
                print(i, "as w vs", j, "as b")

                # init game
                w_player = self.start_process(self.player_configs[i], 'w')
                b_player = self.start_process(self.player_configs[j], 'b')

                # run game
                game = game_runner.GameRunner(w_player, b_player)
                
                win_list_lock = Lock()
                t = Thread(target = self.game_run_thread, args =(game, win_list_lock, i, j))
                t.start()
                self.running_games.append(t)
        
        # wait till all games are finished
        all_finished = False
        while(not all_finished):
            all_finished = True
            for t in self.running_games:
                if t.is_alive():
                    all_finished = False
            
            time.sleep(1)

        return self.wins_by_player

    # --------------
    # HELPER
    # --------------

    def game_run_thread(self, game, win_list_lock, w_index: int, b_index: int):
        """Runs the game in a new thread

        Parameters:
        game (game_runner): The gamer_runner instance that is to be run.
        wins_by_player(list): The list that holds the points scored by each player.
        win_list_lock(lock): A lock for wins_by_player.

        """
        draw, white_won = game.run()

        # store performance information
        win_list_lock.acquire()
        
        if draw:
            # a draw is not as bad as loosing but not as good as winning
            self.wins_by_player[w_index] += 0.5 
            self.wins_by_player[b_index] += 0.5
        else:
            winner_index = w_index if white_won else b_index
            self.wins_by_player[winner_index] += 1

        win_list_lock.release()

    def start_process(self, config, player_token):
        """Starts a process based on a config.

        Parameters:
        config (dict): The config.

        Returns:
        process: The process.

        """
        # read param_config.c and generate the new
        # one by substituting the config instance
        new_config = []
        with open(config_path, 'r') as f:
            for line in f:
                if "struct config_template config" in line:

                    # generate the new line
                    new_line = "struct config_template config="

                    time_factor = config["remaining_time_factor"]
                    new_line += '{' + str(time_factor) + ','

                    gradient = config["pyramid_gradient"]
                    new_line += str(gradient) + ",{"

                    for v in config["piece_values"]:
                        new_line += str(v) + ','

                    new_line = new_line[:-1]  # remove trailing ','
                    new_line += "}};\n"

                    new_config.append(new_line)
                else:
                    new_config.append(line)

        # write the new config
        with open(config_path, 'w') as f:
            f.writelines(new_config)

        # recompile bot.c
        p = subprocess.Popen(shlex.split("make -C ../build"),
                             shell=False,
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        p.kill()

        # TODO : these parameters must be linked with those in game_runner
        parameters = ' ' + player_token + " 60.0 60"
        cmd = path_to_executable + parameters

        # start new bot
        p = subprocess.Popen(shlex.split(cmd),
                             shell=False,
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        return p
