#include <infrastructure.h>

/* Helpers for instruction counting code generation.  */

static TCGArg *icount_arg;
static int stopflag_label;

static inline void gen_block_header(void)
{
    TCGv_i32 flag;
    stopflag_label = gen_new_label();
    flag = tcg_temp_local_new_i32();
    tcg_gen_ld_i32(flag, cpu_env, offsetof(CPUState, exit_request));
    tcg_gen_brcondi_i32(TCG_COND_NE, flag, 0, stopflag_label);
    tcg_temp_free_i32(flag);

#if defined(TARGET_SPARC) || defined(TARGET_ARM) || defined(TARGET_PPC)
    if(tlib_is_instruction_count_enabled())
    {
        icount_arg = gen_opparam_ptr + 1;
        TCGv_i32 instruction_count = tcg_const_i32(88888); // bogus value
        gen_helper_update_insn_count(instruction_count);
        tcg_temp_free_i32(instruction_count);
    }

#endif
}

static void gen_block_footer(TranslationBlock *tb, int num_insns)
{
    gen_set_label(stopflag_label);
    tcg_gen_exit_tb((long)tb + 2);
    if(tlib_is_instruction_count_enabled())
    {
        *icount_arg = num_insns;
    }
}