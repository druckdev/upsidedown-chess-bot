import generation
import tournament

# Description :
# The trainer controls the training and
# thereby connects the various parts.

# --------------
# Interface
# --------------


def train(instances=20, num_of_generations=100):
    """Main function for the ML driven training

    Parameters:
    instances (int): How many instances one Generation should consist of
    num_of_generations (int): How deep the generation tree should go

    """

    # setup player configs
    gen = generation.Generation(instances)

    for i in range(num_of_generations):
        print("Processing generation ", i)

        # run the tournament on current generation
        curr_gen = gen.get_current_gen()
        contest = tournament.Tournament(curr_gen)
        res = contest.run_tournament()

        # evolve the current generation based on tournament results
        gen.evolve(res)

def run_single_game():
    """More of a helper function to let just two specific entities compete."""

    old_config = {}
    old_config["remaining_time_factor"] = 3
    old_config["pyramid_gradient"] = 0.01
    old_config["piece_values"] = [0,100,400,400,500,900,1000000]

    new_config = {}
    new_config["remaining_time_factor"] = 2.820726304362136
    new_config["pyramid_gradient"] = 0.011585641926341213
    new_config["piece_values"] = [0, 19, 25, 23, 120, 132, 14458]

    gen = [old_config, new_config]
    contest = tournament.Tournament(gen)
    res = contest.run_tournament()

    print("Points old config : ", res[0])
    print("Points old config : ", res[1])

if __name__ == "__main__":
    pass
