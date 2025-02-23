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
        lt_bad_str
    };
}

#endif
