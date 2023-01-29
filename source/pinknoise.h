#include <stdint.h>
#include <string.h>

typedef struct middle_square_weyl
{
    uint64_t s;
    uint64_t x;
    uint64_t w;
} middle_square_weyl;

int32_t pink_next_int(middle_square_weyl* rand);

typedef struct pink_noise_gen
{
    middle_square_weyl rand;
    float out;
    float octave_hold_vals[9];
    uint8_t counter;
} pink;

float pink_noise_next(pink *osc);
int32_t pink_next_int(middle_square_weyl* rand);
void init_pink_noise_gen(pink *osc);
