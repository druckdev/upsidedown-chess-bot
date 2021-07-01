import game_runner
import subprocess

# Description :
# A tournament uses a generation (list of bot fitting configs)
# to create bot processes which play against each other, to
# determine which configs are superior.

# TODO : this needs to be wrapped or parsed somehow
path_to_executable = "../build/bot"
parameters = " w 100 50"
cmd = [ path_to_executable + parameters ]

class Tournament:
    def __init__(self, player_configs):
        self.player_configs = player_configs
    
    #--------------
    # Interface
    #--------------

    def run_tournament(self):
        """Runs a tournament between all players

        Returns:
        array: Contains the performance of each player. 

        """

        num_of_players = len(self.player_configs)
        wins_by_player = [0] * num_of_players

        # let all players play against all others
        for i in range(num_of_players):
            for j in range(num_of_players):
                if i == j:
                    continue
                print(i, "as w vs", j, "as b")

                # init game
                w_player = self.start_process(self.player_configs[i])
                b_player = self.start_process(self.player_configs[j])
                
                # run game
                game = game_runner.GameRunner(w_player, b_player)
                draw, white_won = game.run()

                # store performance information
                
                if draw:
                    # a draw is not as bad as loosing but not as good as winning
                    wins_by_player[i] += 0.5 
                    wins_by_player[j] += 0.5
                else:
                    winner_index = i if white_won else j
                    wins_by_player[winner_index] += 1

        return wins_by_player

    #--------------
    # HELPER
    #--------------

    def start_process(self, config):
        """Starts a process based on a config.

        Parameters:
        config (dict): The config.
        
        Returns:
        process: The process.

        """
        # TODO : rewrite param_config.h , compile
        p = subprocess.Popen(cmd,
                             shell=True,
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        return p