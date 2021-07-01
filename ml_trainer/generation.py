# Description :
# A generation is a set of configs as dictionaries, where 
# the keys match those of the 'config_template' struct in
# 'param_config.h' .

class Generation:
    def __init__(self, instances, keep_from_gen=0.4):
        # setup first generation
        # TODO : read the default config from the current param_config.h
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
        
        # sort process indexes by performance
        indexes = range(len(performances))
        proc_i_by_fitness = sorted(indexes, key=lambda p: performances[p])

        # get strongest
        n = int(self.keep_from_gen * len(self.entity_configs)) # how many processes to keep
        strongest = proc_i_by_fitness[:n]

        # reproduce strongest, filling the next generation
        new_configs = strongest
        for i in range(len(self.entity_configs) - len(strongest)):
            new_config = self.get_offspring(strongest)
            new_configs.append(new_config)
        
        self.entity_configs = new_configs
    

    def get_current_gen(self):
        '''Getter for configs'''
        return self.entity_configs
    
    #--------------
    # Helper
    #--------------

    def get_offspring(self, parents: list) -> dict:
        """Computes a new, slightly randomized config based on given ones.

        Parameters:
        parents (list): list of parent configs.

        Returns:
        dict: The new offspring config.

        """

        # TODO : compute based on parents
        default_config = {
            "remaining_time_factor" : 3,
            "pyramid_gradient" : 0.01,
            "piece_values" : [ 0, 100, 400, 400, 500, 900, 1000000 ],
        }

        return default_config
