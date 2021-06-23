import game_runner

class Tournament:
    def __init__(self, player_processes):
        self.player_processes = player_processes
    
    #--------------
    # Interface
    #--------------

    def run_tournament(self):
        """Runs a tournament between all players

        Returns:
        array: Contains the performance of each player. 

        """

        num_of_players = len(self.player_processes)
        wins_by_player = [0 * num_of_players]

        # let all players play against all others
        for i in range(num_of_players):
            for j in range(num_of_players):
                if i == j:
                    continue

                # init game
                w_player = self.player_processes[i] 
                b_player = self.player_processes[j]
                
                # run game
                game = game_runner.GameRunner(w_player, b_player)
                white_won = game.run()

                # store performance information
                winner_index = i if white_won else j
                wins_by_player[winner_index] += 1

        return wins_by_player