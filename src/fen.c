//
// Created by erena on 13.09.2024.
//

#include "fen.h"

// parse FEN string
void parseFEN(char *fen, board* position) {
    // reset board position (bitboards)
    memset(position->bitboards, 0ULL, sizeof(position->bitboards));
    // reset board occupancies (bitboards)
    memset(position->occupancies, 0ULL, sizeof(position->occupancies));
    // reset mailbox
    memset(position->mailbox, NO_PIECE, sizeof(position->mailbox));

    // reset game state variables
    position->side = 0;
    position->enpassant = no_sq;
    position->castle = 0;

    // reset repetition index
    position->repetitionIndex = 0;

    // reset repetition table
    memset(position->repetitionTable, 0ULL, sizeof(position->repetitionTable));

    // parse board
    for (int square = 0; square < 64 && *fen; fen++) {
        switch (*fen) {
            case 'P': case 'N': case 'B': case 'R': case 'Q': case 'K':
            case 'p': case 'n': case 'b': case 'r': case 'q': case 'k': {
                int piece = charPieces[(unsigned char)*fen];
                setBit(position->bitboards[piece], square);
                position->mailbox[square] = piece;
                square++;
                break;
            }
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': {
                square += *fen - '0';
                break;
            }
            case '/': {
                break;
            }
            default: {
                // TODO: Error detection
                break;
            }
        }
    }

    fen++;

    position->side = (*fen == 'w') ? white : black;
    fen += 2;

    while (*fen != ' ') {
        switch (*fen) {
            case 'K':
                position->castle |= wk;
                break;
            case 'Q':
                position->castle |= wq;
                break;
            case 'k':
                position->castle |= bk;
                break;
            case 'q':
                position->castle |= bq;
                break;
            case '-':
                break;
        }
        fen++;
    }

    fen++;

    if (*fen != '-') {
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        position->enpassant = rank * 8 + file;
    } else {
        position->enpassant = no_sq;
    }

    // go to parsing half move counter (increment pointer to FEN string)
    fen++;

    // parse half move counter to init fifty move counter
    position->fifty = atoi(fen);

    for (int piece = P; piece <= K; piece++) {
        position->occupancies[white] |= position->bitboards[piece];
    }
    for (int piece = p; piece <= k; piece++) {
        position->occupancies[black] |= position->bitboards[piece];
    }
    position->occupancies[both] |= position->occupancies[white];
    position->occupancies[both] |= position->occupancies[black];

    // init hash key
    position->hashKey = generateHashKey(position);
    position->pawnKey = generatePawnKey(position);
    position->minorKey = generateMinorKey(position);
    position->majorKey = generateMajorKey(position);
    position->whiteNonPawnKey = generate_white_np_hash_key(position);
    position->blackNonPawnKey = generate_black_np_hash_key(position);
    position->krpKey = generate_krp_key(position);
}

