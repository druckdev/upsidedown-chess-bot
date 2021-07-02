#ifndef HELPER_H
#define HELPER_H

struct config_template {
    float remaining_time_factor;
    float pyramid_gradient;
    int piece_values[7];
};

extern struct config_template config;

#endif /* HELPER_H */