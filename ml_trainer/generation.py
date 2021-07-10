import random

# Description :
# A generation is a set of configs as dictionaries, where
# the keys match those of the 'config_template' struct in
# 'param_config.h' .
# Each generation is based on the predecessing one,
# varying less with each iteration of the process.

config_path = "../src/param_config.c" # TODO : this also needs to go somewhere else

class Generation:
    def __init__(self, instances, variation=1, keep_from_gen=0.4):
        default_config = {
            "remaining_time_factor" : 0,
            "pyramid_gradient" : 0,
            "piece_values" : [],
        }

        # read the current values in param_config.c
        with open(config_path) as f:
            for line in f:
                if "struct config_template config" in line:
                    line_split = line.split('=')
                    line_reduced = line_split[1][:-5]
                    num_str = line_reduced.replace('{', '').replace('}', '')
                    num_list = num_str.split(',')

                    default_config["remaining_time_factor"] = float(num_list[0])
                    default_config["pyramid_gradient"] = float(num_list[1])
                    default_config["piece_values"] = [ int(v) for v in num_list[2:] ]

        # how much each generation should vary from the average of their predecessors
        # , in [0, 1]
        self.variation = variation

        # how many of each gen should be kept, in [0, 1]
        self.keep_from_gen = keep_from_gen

        # get first gen
        self.entity_configs = self.create_variations(default_config, instances)

    #--------------
    # Interface
    #--------------

    def evolve(self, performances):
        """Prepares next gen of processes.

        Parameters:
        performances (list): A list of same length as self.entity_configs
                              where the elements are the performance
                              of the corresponding config.

        """

        print("Evolve based on ", performances)

        # sort process indexes by performance
        indexes = range(len(performances))
        proc_i_by_fitness = sorted(indexes, key=lambda p: performances[p])

        # get strongest
        n = round(self.keep_from_gen * len(self.entity_configs)) # how many processes to keep
        strongest = proc_i_by_fitness[:n]

        # reproduce strongest, filling the next generation
        new_configs = [self.entity_configs[i] for i in strongest]
        offspring = self.get_offspring(new_configs, len(self.entity_configs) - len(strongest))
        new_configs += offspring

        self.entity_configs = new_configs


    def get_current_gen(self):
        '''Getter for configs'''
        return self.entity_configs

    #--------------
    # Helper
    #--------------

    def get_offspring(self, parents: list, needed: int) -> dict:
        """Computes new, varying configs based on given ones.

        Parameters:
        parents (list): List of parent configs.
        needed   (int): How many new configs are needed

        Returns:
        list: The new offspring configs.
        """

        offspring_configs = []

        avg_conf = {
            "remaining_time_factor": 0,
            "pyramid_gradient": 0,
            "piece_values": []
        }
        for config in parents:
            avg_conf["remaining_time_factor"] += config["remaining_time_factor"] / len(parents)
            avg_conf["pyramid_gradient"] += config["pyramid_gradient"] / len(parents)
            avg_conf["piece_values"] += [x / len(parents) for x in config["piece_values"]]

        offspring_configs = self.create_variations(avg_conf, needed)

        return offspring_configs


    def create_variations(self, config: dict, needed: int) -> list:
        """Create configs that vary from the one given.

        Parameters:
        confi (list): The config to vary from.

        Returns:
        list: The new varying configs.
        """
        varying_configs = []

        # calculate the upper and lower variation bounds
        bounds = lambda x: (x - (x * self.variation), x + (x * self.variation))

        p_time = config["remaining_time_factor"]
        p_gradient = config["pyramid_gradient"]
        p_piece_values = config["piece_values"]

        # generate new configs whilst varying based on bounds
        for i in range(needed):
            time_factor = random.uniform(bounds(p_time)[0], bounds(p_time)[1])
            gradient = random.uniform(bounds(p_gradient)[0], bounds(p_gradient)[1])

            piece_values = []
            for val in p_piece_values:
                val = round(random.uniform(bounds(val)[0], bounds(val)[1]))
                piece_values.append(val)

            new_config = {
                "remaining_time_factor" : time_factor,
                "pyramid_gradient" : gradient,
                "piece_values" : piece_values,
            }
            varying_configs.append(new_config)

        # reduce variation by 1/4 for next gen
        self.variation = self.variation * 0.75

        return varying_configs
