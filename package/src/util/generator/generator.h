#ifndef GENERATOR_H__
#define GENERATOR_H__

#include <stdint.h>

struct Generator {
    struct MT19937 *gen;
    double rate;
    uint32_t upper_limit[3];
};

int generator_generate_next(struct Generator *gen, double *out);
int generator_generate_amount(struct Generator *gen, uint32_t *out);

#endif
