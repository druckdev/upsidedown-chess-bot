import subprocess

path_to_executable = "../build/bot"
cmd = [ path_to_executable ]

class Generation:
    def __init__(self, instances):
        # setup first generation
        self.processes = []
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
        
        # remove weakest and reproduce strongest (fitness = last performance)
        print("Evolve based on ", performances)
        
        pass
    
    def get_current_gen(self):
        '''Getter for processes'''
        return self.processes