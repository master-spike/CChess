#include <stdint.h>
#include "bitboard.h"

#define CHECK_EXPECTED_CHAR(z)      \
    if (rank == p / 8)              \
        b.pieces[z] |= (1ULL << i); \
    else                            \
        unexpected = 1;             \
    p--;                            \
    break;
#define CHECK_EXPECTED_CRIGHT(z) \
    if (!(b.c_rights & z))       \
        b.c_rights += z;         \
    else                         \
        invalid = 1;             \
    break;

struct BitBoard parseFEN(char *str)
{

    int p = 63;
    int rank = 7;
    int i = 0;
    struct BitBoard b;
    while (str[i] != '\0' && str[i] == ' ')
        i++;
    if (str[i] == '\0')
    {
        b.pieces[10] = ~0ULL;
        return b;
    }
    while (p >= 0)
    {
        if (str[i] == '\0')
            break;
        int unexpected = 0;
        switch (str[i])
        {
        case '/':
            if (p == rank * 8 - 1)
                rank--;
            else
                unexpected = 1;
            break;
        case 'P':
            CHECK_EXPECTED_CHAR(0)
        case 'p':
            CHECK_EXPECTED_CHAR(1)
        case 'N':
            CHECK_EXPECTED_CHAR(2)
        case 'n':
            CHECK_EXPECTED_CHAR(3)
        case 'B':
            CHECK_EXPECTED_CHAR(4)
        case 'b':
            CHECK_EXPECTED_CHAR(5)
        case 'R':
            CHECK_EXPECTED_CHAR(6)
        case 'r':
            CHECK_EXPECTED_CHAR(7)
        case 'Q':
            CHECK_EXPECTED_CHAR(8)
        case 'q':
            CHECK_EXPECTED_CHAR(9)
        case 'K':
            CHECK_EXPECTED_CHAR(10)
        case 'k':
            CHECK_EXPECTED_CHAR(11)
        default:
            if ('1' <= str[i] < '1' + (p % 8))
                p -= str[i] - '0';
            else
                unexpected = 1;
            break;
        }
        i++;
        if (unexpected)
        {
            b.pieces[10] = ~0ULL; // to represent an error
            return b;
        }
    }

    if (str[i] != ' ')
    {
        b.pieces[10] = ~0ULL;
        return b;
    }
    while (str[i] == ' ')
        i++;

    if (str[i] == 'b' || str[i] == 'B')
    {
        b.ply_count = 1;
    }
    else if (str[i] == 'w' || str[i] == 'W')
    {
        b.ply_count = 0;
    }
    else
    {
        b.pieces[10] = ~0ULL;
        return b;
    }

    i++;

    if (str[i] != ' ')
    {
        b.pieces[10] = ~0ULL;
        return b;
    }

    while (str[i] == ' ')
        i++;

    b.c_rights = 0;

    if (str[i] == '\0')
    {
        b.pieces[10] = ~0ULL;
        return b;
    }

    if (str[i] == '-')
        i++;
    else
    {
        int invalid = 0;
        while (str[i] != ' ')
        {
            switch (str[i])
            {
            case 'K':
                CHECK_EXPECTED_CRIGHT(1);
            case 'Q':
                CHECK_EXPECTED_CRIGHT(2);
            case 'k':
                CHECK_EXPECTED_CRIGHT(4);
            case 'q':
                CHECK_EXPECTED_CRIGHT(8);
            default:
                invalid = 1;
                break;
            }
            if (invalid)
            {
                b.pieces[10] = ~0ULL; // to represent an error
                return b;
            }
            i++;
        }
    }
}