#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "set-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "set-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "set-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(seta_rm)
make_helper_v(setae_rm)
make_helper_v(setb_rm)
make_helper_v(setbe_rm)
make_helper_v(setc_rm)
make_helper_v(sete_rm)
make_helper_v(setz_rm)
make_helper_v(setg_rm)
make_helper_v(setge_rm)
make_helper_v(setl_rm)
make_helper_v(setle_rm)
make_helper_v(setna_rm)
make_helper_v(setnae_rm)
make_helper_v(setnb_rm)
make_helper_v(setnbe_rm)
make_helper_v(setnc_rm)
make_helper_v(setne_rm)
make_helper_v(setng_rm)
make_helper_v(setnge_rm)
make_helper_v(setnl_rm)
make_helper_v(setnle_rm)
make_helper_v(setno_rm)
make_helper_v(setnp_rm)
make_helper_v(setns_rm)
make_helper_v(setnz_rm)
make_helper_v(seto_rm)
make_helper_v(setp_rm)
make_helper_v(setpe_rm)
make_helper_v(setpo_rm)
make_helper_v(sets_rm)


