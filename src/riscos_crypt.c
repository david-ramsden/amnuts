/*
 * Unix DES crypt(3) replacement for RISC OS.
 *
 * Implements the traditional 2-character salt DES-based crypt() compatible
 * with standard Unix password files.  Password files can be moved between
 * Unix and RISC OS without conversion.
 *
 * Based on the public-domain DES algorithm as documented in FIPS PUB 46-3.
 * The Unix crypt modification uses the 12-bit salt to permute the E-box.
 *
 * Only compiled when __riscos is defined.
 */

#ifdef __riscos

#include <string.h>

/* ---- DES tables -------------------------------------------------- */

static const unsigned char IP[64] = {
    58,50,42,34,26,18,10, 2, 60,52,44,36,28,20,12, 4,
    62,54,46,38,30,22,14, 6, 64,56,48,40,32,24,16, 8,
    57,49,41,33,25,17, 9, 1, 59,51,43,35,27,19,11, 3,
    61,53,45,37,29,21,13, 5, 63,55,47,39,31,23,15, 7
};

static const unsigned char FP[64] = {
    40, 8,48,16,56,24,64,32, 39, 7,47,15,55,23,63,31,
    38, 6,46,14,54,22,62,30, 37, 5,45,13,53,21,61,29,
    36, 4,44,12,52,20,60,28, 35, 3,43,11,51,19,59,27,
    34, 2,42,10,50,18,58,26, 33, 1,41, 9,49,17,57,25
};

/* Permuted-choice 1 C and D halves */
static const unsigned char PC1_C[28] = {
    57,49,41,33,25,17, 9, 1,58,50,42,34,26,18,
    10, 2,59,51,43,35,27,19,11, 3,60,52,44,36
};
static const unsigned char PC1_D[28] = {
    63,55,47,39,31,23,15, 7,62,54,46,38,30,22,
    14, 6,61,53,45,37,29,21,13, 5,28,20,12, 4
};

/* Permuted-choice 2 */
static const unsigned char PC2[48] = {
    14,17,11,24, 1, 5, 3,28,15, 6,21,10,
    23,19,12, 4,26, 8,16, 7,27,20,13, 2,
    41,52,31,37,47,55,30,40,51,45,33,48,
    44,49,39,56,34,53,46,42,50,36,29,32
};

/* Number of left-rotations per round */
static const unsigned char LS[16] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

/* Expansion E (32->48) - modified by salt in crypt */
static const unsigned char E_initial[48] = {
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
     8, 9,10,11,12,13,12,13,14,15,16,17,
    16,17,18,19,20,21,20,21,22,23,24,25,
    24,25,26,27,28,29,28,29,30,31,32, 1
};

/* Permutation P */
static const unsigned char P_perm[32] = {
    16, 7,20,21,29,12,28,17, 1,15,23,26, 5,18,31,10,
     2, 8,24,14,32,27, 3, 9,19,13,30, 6,22,11, 4,25
};

/* S-boxes */
static const unsigned char S[8][64] = {
    { /* S1 */
        14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
         0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
         4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
        15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13
    },
    { /* S2 */
        15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
         3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
         0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
        13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9
    },
    { /* S3 */
        10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
        13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
        13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
         1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12
    },
    { /* S4 */
         7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
        13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
        10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
         3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14
    },
    { /* S5 */
         2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
        14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
         4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
        11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3
    },
    { /* S6 */
        12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
        10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
         9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
         4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13
    },
    { /* S7 */
         4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
        13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
         1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
         6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12
    },
    { /* S8 */
        13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
         1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
         7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
         2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11
    }
};

/* Traditional base64 alphabet for crypt output */
static const char b64[] =
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/* ---- bit manipulation helpers ------------------------------------ */

/* Get bit n (1-based) from buf[] */
#define GETBIT(buf, n) (((buf)[((n)-1)>>3] >> (7 - (((n)-1)&7))) & 1)

/* Set bit n (1-based) in buf[] */
#define SETBIT(buf, n, v) \
    do { \
        if (v) (buf)[((n)-1)>>3] |=  (1 << (7 - (((n)-1)&7))); \
        else   (buf)[((n)-1)>>3] &= ~(1 << (7 - (((n)-1)&7))); \
    } while (0)

/* ---- internal state ---------------------------------------------- */

typedef unsigned char block64[8];   /* 64-bit block as 8 bytes */
typedef unsigned char block48[6];   /* 48-bit block as 6 bytes */
typedef unsigned char block28[4];   /* 28-bit block (only 28 bits used) */
typedef unsigned char block32[4];   /* 32-bit block */

/* ---- core DES functions ------------------------------------------ */

static void
permute(const block64 in, const unsigned char *perm, int out_bits, unsigned char *out)
{
    int i;
    int out_bytes = (out_bits + 7) / 8;
    for (i = 0; i < out_bytes; i++) out[i] = 0;
    for (i = 0; i < out_bits; i++) {
        int src = perm[i];
        int bit = GETBIT(in, src);
        if (bit) out[i >> 3] |= (1 << (7 - (i & 7)));
    }
}

static void
expand48(const block32 in, const unsigned char *E_box, block48 out)
{
    int i;
    for (i = 0; i < 6; i++) out[i] = 0;
    for (i = 0; i < 48; i++) {
        int src = E_box[i];
        int bit = GETBIT(in, src);
        if (bit) out[i >> 3] |= (1 << (7 - (i & 7)));
    }
}

/* Generate 16 subkeys from a 64-bit key, storing as 48-bit values */
static void
key_schedule(const block64 key, block48 subkeys[16])
{
    unsigned char C[4], D[4];
    int i, j;

    /* PC1 */
    for (i = 0; i < 4; i++) C[i] = D[i] = 0;
    for (i = 0; i < 28; i++) {
        if (GETBIT(key, PC1_C[i])) C[i>>3] |= (1 << (7-(i&7)));
    }
    for (i = 0; i < 28; i++) {
        if (GETBIT(key, PC1_D[i])) D[i>>3] |= (1 << (7-(i&7)));
    }

    for (j = 0; j < 16; j++) {
        int k;
        /* Rotate C and D left by LS[j] */
        for (k = 0; k < LS[j]; k++) {
            int c0 = (C[0] >> 7) & 1;
            int d0 = (D[0] >> 7) & 1;
            /* Rotate C (28 bits) */
            C[0] = (unsigned char)((C[0] << 1) | ((C[1] >> 7) & 1));
            C[1] = (unsigned char)((C[1] << 1) | ((C[2] >> 7) & 1));
            C[2] = (unsigned char)((C[2] << 1) | ((C[3] >> 7) & 1));
            /* Bit 27 lives at C[3] bit 4; the wrap-around must land there. */
            C[3] = (unsigned char)((C[3] << 1) | (c0 << 4));
            C[3] &= 0xF0;
            /* Rotate D (28 bits) */
            D[0] = (unsigned char)((D[0] << 1) | ((D[1] >> 7) & 1));
            D[1] = (unsigned char)((D[1] << 1) | ((D[2] >> 7) & 1));
            D[2] = (unsigned char)((D[2] << 1) | ((D[3] >> 7) & 1));
            D[3] = (unsigned char)((D[3] << 1) | (d0 << 4));
            D[3] &= 0xF0;
        }
        /* PC2: combine C||D (56 bits) and select 48 */
        {
            unsigned char CD[7];
            for (k = 0; k < 4; k++) CD[k] = 0;
            CD[0] = C[0]; CD[1] = C[1]; CD[2] = C[2]; CD[3] = C[3];
            /* D shifted down 4 bits to pack into CD[3..6] */
            CD[3] |= (D[0] >> 4) & 0x0F;
            CD[4] = (unsigned char)((D[0] << 4) | ((D[1] >> 4) & 0x0F));
            CD[5] = (unsigned char)((D[1] << 4) | ((D[2] >> 4) & 0x0F));
            CD[6] = (unsigned char)((D[2] << 4) | ((D[3] >> 4) & 0x0F));

            for (k = 0; k < 6; k++) subkeys[j][k] = 0;
            for (k = 0; k < 48; k++) {
                int src = PC2[k];
                int bit = (CD[(src-1)>>3] >> (7-((src-1)&7))) & 1;
                if (bit) subkeys[j][k>>3] |= (1 << (7-(k&7)));
            }
        }
    }
}

/* One DES round: R -> new R, L XOR f(R,K) -> new L */
static void
des_round(block32 L, block32 R, const block48 K, const unsigned char *E_box,
          block32 new_L, block32 new_R)
{
    block48 ER;
    unsigned char f[4];
    int i;

    /* Expand R with the (possibly salt-modified) E_box */
    expand48(R, E_box, ER);

    /* XOR with subkey */
    for (i = 0; i < 6; i++) ER[i] ^= K[i];

    /* S-box substitution */
    for (i = 0; i < 4; i++) f[i] = 0;
    for (i = 0; i < 8; i++) {
        int row, col, val;
        int base = i * 6;
        /* bits base, base+1..base+5 from ER */
        int b0 = (ER[base>>3] >> (7-(base&7))) & 1;
        int b1 = (ER[(base+1)>>3] >> (7-((base+1)&7))) & 1;
        int b2 = (ER[(base+2)>>3] >> (7-((base+2)&7))) & 1;
        int b3 = (ER[(base+3)>>3] >> (7-((base+3)&7))) & 1;
        int b4 = (ER[(base+4)>>3] >> (7-((base+4)&7))) & 1;
        int b5 = (ER[(base+5)>>3] >> (7-((base+5)&7))) & 1;
        row = (b0 << 1) | b5;
        col = (b1 << 3) | (b2 << 2) | (b3 << 1) | b4;
        val = S[i][row * 16 + col];
        /* Pack 4-bit val into f starting at bit i*4 */
        {
            int out_bit = i * 4;
            if (val & 8) f[out_bit>>3] |= (1 << (7-(out_bit&7)));
            out_bit++;
            if (val & 4) f[out_bit>>3] |= (1 << (7-(out_bit&7)));
            out_bit++;
            if (val & 2) f[out_bit>>3] |= (1 << (7-(out_bit&7)));
            out_bit++;
            if (val & 1) f[out_bit>>3] |= (1 << (7-(out_bit&7)));
        }
    }

    /* P permutation */
    {
        unsigned char pf[4];
        for (i = 0; i < 4; i++) pf[i] = 0;
        for (i = 0; i < 32; i++) {
            int src = P_perm[i];
            int bit = (f[(src-1)>>3] >> (7-((src-1)&7))) & 1;
            if (bit) pf[i>>3] |= (1 << (7-(i&7)));
        }
        /* new_R = L XOR f(R,K) */
        for (i = 0; i < 4; i++) new_R[i] = (unsigned char)(L[i] ^ pf[i]);
    }

    /* new_L = R */
    for (i = 0; i < 4; i++) new_L[i] = R[i];
}

/* Apply full DES (25 iterations as per Unix crypt) to a 64-bit block */
static void
des_crypt_block(block64 in, block48 subkeys[16], const unsigned char *E_box,
                block64 out)
{
    block64 tmp;
    block32 L, R, nL, nR;
    int i, round;

    /* Initial permutation */
    permute(in, IP, 64, tmp);

    /* Split into L and R halves */
    for (i = 0; i < 4; i++) {
        L[i] = tmp[i];
        R[i] = tmp[i + 4];
    }

    /* 25 applications of DES, swapping L/R between each one.
     * Each DES application ends with the Feistel swap (built into how
     * DES feeds into FP).  Without the inter-iteration swap the rounds
     * run in the wrong Feistel order and produce a wrong cipher. */
    for (i = 0; i < 25; i++) {
        unsigned char tL0, tL1, tL2, tL3;
        for (round = 0; round < 16; round++) {
            des_round(L, R, subkeys[round], E_box, nL, nR);
            L[0]=nL[0]; L[1]=nL[1]; L[2]=nL[2]; L[3]=nL[3];
            R[0]=nR[0]; R[1]=nR[1]; R[2]=nR[2]; R[3]=nR[3];
        }
        tL0=L[0]; tL1=L[1]; tL2=L[2]; tL3=L[3];
        L[0]=R[0]; L[1]=R[1]; L[2]=R[2]; L[3]=R[3];
        R[0]=tL0;  R[1]=tL1;  R[2]=tL2;  R[3]=tL3;
    }

    /* After 25 swaps: L=R25, R=L25.  FP needs R25||L25 = L||R. */
    for (i = 0; i < 4; i++) {
        tmp[i]   = L[i];
        tmp[i+4] = R[i];
    }

    /* Final permutation */
    permute(tmp, FP, 64, out);
}

/* ---- Public crypt() replacement ---------------------------------- */

char *
crypt(const char *pw, const char *salt)
{
    static char result[14];
    unsigned char key[8];
    block64 block, cipher;
    block48 subkeys[16];
    unsigned char E_box[48];
    int i, s0, s1, ibit;
    const char *p;

    /* Build 56-bit key from password (8 chars, bit 0 of each byte = 0) */
    for (i = 0; i < 8; i++) key[i] = 0;
    for (i = 0, p = pw; i < 8 && *p; i++, p++) {
        key[i] = (unsigned char)((*p & 0x7F) << 1);
    }

    /* Decode salt characters (2 chars -> 12 bits) */
    s0 = s1 = 0;
    for (i = 0; i < 64; i++) {
        if (b64[i] == salt[0]) s0 = i;
        if (b64[i] == salt[1]) s1 = i;
    }

    /* Apply salt to modify the E-box expansion.
     * For each bit position 0..23 in E_box, if the corresponding
     * salt bit is set, swap E_box[i] and E_box[i+24]. */
    for (i = 0; i < 48; i++) E_box[i] = E_initial[i];

    for (ibit = 0; ibit < 12; ibit++) {
        int saltbit = (ibit < 6) ? ((s0 >> ibit) & 1)
                                 : ((s1 >> (ibit-6)) & 1);
        if (saltbit) {
            unsigned char tmp2 = E_box[ibit];
            E_box[ibit] = E_box[ibit + 24];
            E_box[ibit + 24] = tmp2;
        }
    }

    /* Generate key schedule */
    key_schedule(key, subkeys);

    /* Encrypt 64 zero bits 25 times */
    for (i = 0; i < 8; i++) block[i] = 0;
    des_crypt_block(block, subkeys, E_box, cipher);

    /* Encode result: salt (2 chars) + 11 base64 chars */
    result[0] = salt[0];
    result[1] = salt[1];

    /* Pack 64 bits into 11 base64 characters (6 bits each) */
    {
        unsigned long acc = 0;
        int bits = 0;
        int pos = 2;
        for (i = 0; i < 8 && pos < 13; i++) {
            acc = (acc << 8) | cipher[i];
            bits += 8;
            while (bits >= 6 && pos < 13) {
                bits -= 6;
                result[pos++] = b64[(acc >> bits) & 0x3F];
            }
        }
        /* 64 mod 6 = 4 bits remain; shift left 2 to place them MSB-first. */
        if (pos < 13) result[pos++] = b64[(acc << 2) & 0x3F];
        result[13] = '\0';
    }

    return result;
}

#else /* !__riscos */

/* This file is RISC OS only; give non-RISC OS builds a declaration so the
 * translation unit is not empty (ISO C forbids that; -Wpedantic warns). */
typedef int riscos_crypt_not_used;

#endif /* __riscos */
