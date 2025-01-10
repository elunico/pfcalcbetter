#include "utils.h"

pfnum_t safe_op(pfnum_t lhs, pfnum_t rhs,
                        pfnum_t (*operation)(pfnum_t, pfnum_t)) {
    if (rhs == 0) {
#ifdef PFCALC_PREVENT_ZERO_DIV
        rhs = 1;
#else
        fail("Division by 0");
#endif
    }
    return operation(lhs, rhs);
}


pfnum_t divide(pfnum_t lhs, pfnum_t rhs) {
    return lhs / rhs;
}

#if defined(PF_NUM_LONG)
pfnum_t modulo(pfnum_t lhs, pfnum_t rhs) {
    return lhs % rhs;
}
#endif