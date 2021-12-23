#include <math.h>

#include "generator.h"
#include "mt19937.h"
#include "../errcode.h"

int generator_generate_next(struct Generator *gen, double *out) {
    if (!gen || !out) {
        return ERR_INPUT_NULL;
    }
    *out = -log(mt19937_gen(gen->gen) / (double)0xffffffff) / gen->rate;
    return ERR_OK;
}

int generator_generate_amount(struct Generator *gen, uint32_t *out) {
    if (!gen || !out) {
        return ERR_INPUT_NULL;
    }
    for (uint32_t i = 0,
         *ii = out,
         *iii = gen->upper_limit;
         i < 3;
         ++i,
         ++ii,
         ++iii) {
        *ii = (uint32_t)((mt19937_gen(gen->gen) /
                          (double)0xffffffff) *
                        (*iii - 1) +
                        1.5);
    }
    return ERR_OK;
}
