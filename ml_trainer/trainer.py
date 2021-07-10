import generation
import tournament

# Description :
# The trainer controls the training and
# thereby connects the various parts.

# --------------
# Interface
# --------------


def train(instances=2, num_of_generations=1):
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


if __name__ == "__main__":
    train()
