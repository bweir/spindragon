#pragma once


enum Block {
    NO_BLOCK,
    CON_BLOCK,
    VAR_BLOCK,
    OBJ_BLOCK,
    PUB_BLOCK,
    PRI_BLOCK,
    DAT_BLOCK,
    ASM_BLOCK
};

enum Tokens {

    // blocks
    T_CON,
    T_VAR,
    T_OBJ,
    T_PUB,
    T_PRI,
    T_DAT,
    T_ASM,

    // comments
    T_DOC_APOSTROPHE,
    T_DOC_BRACKET_L,
    T_DOC_BRACKET_R,
    T_APOSTROPHE,
    T_BRACKET_L,
    T_BRACKET_R,

    T_QUOTE,
    T_HASH,
    T_DOT,
    T_ADDR,
    T_CARET,
    T_AND
};
