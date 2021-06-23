import subprocess

path_to_executable = "../build/bot"
cmd = [ path_to_executable ]

class Generation:
    def __init__(self, instances, keep_from_gen=0.4):
        # setup first generation
        self.processes = []
        self.keep_from_gen = keep_from_gen
        for i in range(instances):
            p = subprocess.Popen(cmd,
                                 stdin=subprocess.PIPE,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
            self.processes.append(p)

    #--------------
    # Interface
    #--------------
    
    def evolve(self, performances):
        """prepares next gen of processes

        Parameters:
        performances (array): array of same length as self.processes
                              where the elements are the performance
                              of the corresponding process

        """
        
        print("Evolve based on ", performances)
        
        # sort process indexes by performance
        indexes = range(len(performances))
        proc_i_by_fitness = sorted(indexes, key=lambda p: performances[p])

        # get strongest
        n = int(self.keep_from_gen * len(self.processes)) # how many processes to keep
        strongest = proc_i_by_fitness[:n]

        # reproduce strongest, initalizing the next generation
        new_processes = strongest
        for i in range(len(self.processes) - len(strongest)):
            p = subprocess.Popen(cmd,
                                 stdin=subprocess.PIPE,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
            new_processes.append(p)
        
        self.processes = new_processes
    

    def get_current_gen(self):
        '''Getter for processes'''
        return self.processes
        