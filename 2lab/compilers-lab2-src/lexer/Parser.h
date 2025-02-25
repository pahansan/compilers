#ifndef PARSER_H
#define PARSER_H

/* Token types */
namespace tok
{
    enum type
    {
        /* keywords */
        kw_class = 1,
        kw_else,
        kw_false,
        kw_fi,
        kw_if,
        kw_in,
        kw_inherits,
        kw_isvoid,
        kw_let,
        kw_loop,
        kw_pool,
        kw_then,
        kw_while,
        kw_case,
        kw_esac,
        kw_new,
        kw_of,
        kw_not,
        kw_true,
        /* identifiers */
        id_type,
        id_object,
        /* literals */
        lt_int,
        lt_str,
        /* operators */
        op_dot,
        op_at,
        op_tilda,
        op_asterisk,
        op_slash,
        op_arrow,
        op_plus,
        op_minus,
        op_less_eq,
        op_less,
        op_eq,
        /* special symbols */
        sp_colon,
        sp_semi_colon,
        sp_big_arrow,
        sp_comma,
        /* brackets */
        br_r_open,
        br_r_close,
        br_c_open,
        br_c_close
    };
}

#endif
