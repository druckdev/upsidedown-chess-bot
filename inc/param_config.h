#ifndef HELPER_H
#define HELPER_H

struct config_template {
    float remaining_time_factor;
    float pyramid_gradient;
    int piece_values[7];
};

// the following is read and written by the ml trainer
struct config_template config={3,0.01,{0,100,400,400,500,900,1000000}};

#endif /* HELPER_H */