import random

# Description :
# A generation is a set of configs as dictionaries, where 
# the keys match those of the 'config_template' struct in
# 'param_config.h' .

class Generation:
    def __init__(self, instances, keep_from_gen=0.4):
        # setup first generation
        # TODO : read the default config from the current param_config.h 
        #        and shift the values for every except one config
        default_config = {
            "remaining_time_factor" : 3,
            "pyramid_gradient" : 0.01,
            "piece_values" : [ 0, 100, 400, 400, 500, 900, 1000000 ],
        }
        self.entity_configs = [default_config] * instances
        self.keep_from_gen = keep_from_gen

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
        print("Configs before : ", self.entity_configs)

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

        print("Configs after : ", self.entity_configs)
        
    

    def get_current_gen(self):
        '''Getter for configs'''
        return self.entity_configs
    
    #--------------
    # Helper
    #--------------

    def get_offspring(self, parents: list, needed: int) -> dict:
        """Computes new, slightly randomized configs based on given ones.

        Parameters:
        parents (list): list of parent configs.
        needed   (int): how many new configs are needed 

        Returns:
        list: The new offspring configs.

        """
        offspring_configs = []

        # the best and worst player (in the upper x percent)
        # define the value range for children
        # TODO : don't ignore the others
        print("parents ", parents)
        up_b_time = parents[0]["remaining_time_factor"]
        up_b_gradient = parents[0]["pyramid_gradient"]
        up_b_pieces = parents[0]["piece_values"]

        l_b_time = parents[-1]["remaining_time_factor"]
        l_b_gradient = parents[-1]["pyramid_gradient"]
        l_b_pieces = parents[-1]["piece_values"]

        # generate new configs based on ranges
        for i in range(needed):
            time_factor = l_b_time if l_b_time == up_b_time else random.uniform(l_b_time, up_b_time)
            gradient = l_b_gradient if l_b_gradient == up_b_gradient else random.uniform(l_b_gradient, up_b_gradient)
            
            piece_values = []
            for l, u in zip(l_b_pieces, up_b_pieces):
                l, u = min(l, u), max(l, u)
                val = l if l == u else random.randrange(l, u)
                piece_values.append(val)

            new_config = {
                "remaining_time_factor" : time_factor,
                "pyramid_gradient" : gradient,
                "piece_values" : piece_values,
            }
            offspring_configs.append(new_config)

        return offspring_configs
