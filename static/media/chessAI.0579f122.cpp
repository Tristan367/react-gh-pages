#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <chrono>

using namespace std::chrono;

class Position {
public:
    char board[64];
    short lastMove;
    float alpha, beta;
    bool kingCapture;
    Position() {
        for (short i = 0; i < 64; i++) {
            board[i] = '0';
        }
        lastMove = -1;
        alpha = -1000;
        beta = 1000;
        kingCapture = false;
    }
};

void printBoard(Position position) {
    for (short i = 7; i >= 0; i--) {
        for (short j = 0; j < 8; j++) {
            std::cout << position.board[i * 8 + j] << ' ';
        }
        std::cout << '\n';
    }
}
void copyPosition(Position& dst, Position src) {
    for (short i = 0; i < 64; i++) {
        dst.board[i] = src.board[i];
    }
}

inline bool isSquareInCheckByBlack(Position position, int square) {

    // checking diagonal checks, bishops, queens, pawns, and enemy kings
    short move;
    for (short j = 1; j < 8; j++) {
        move = square + (9 * j);
        if (move < 64 && move % 8 != 0) { // forward right
            if (j == 1) {
                if (position.board[move] == 'p') {
                    return true;
                }
                else if (position.board[move] == 'k' || position.board[move] == 'c') // a black king or a moved black king
                {
                    return true;
                }
            }
            if (position.board[move] == 'b' || position.board[move] == 'q') {
               
                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - (7 * j);
        if (move >= 0 && move % 8 != 0) { // backward right
            if (j == 1) {
                if (position.board[move] == 'k' || position.board[move] == 'c') // a black king or a moved black king 
                {
                    return true;
                }
            }
            if (position.board[move] == 'b' || position.board[move] == 'q') {

                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square + (7 * j);
        if (move < 64 && move % 8 != 7) { // forward left
            if (j == 1) {
                if (position.board[move] == 'p') {
                    return true;
                }
                else if (position.board[move] == 'k' || position.board[move] == 'c') // a black king or a moved black king
                {
                    return true;
                }
            }
            if (position.board[move] == 'b' || position.board[move] == 'q') {

                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - (9 * j);
        if (move >= 0 && move % 8 != 0) { // backward left
            if (j == 1) {
                if (position.board[move] == 'k' || position.board[move] == 'c') // a black king or a moved black king 
                {
                    return true;
                }
            }
            if (position.board[move] == 'b' || position.board[move] == 'q') {

                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }

    // checking orthagonal checks, rooks, queens, and enemy kings
    for (short j = 1; j < 8; j++) {
        move = square + (8 * j);
        if (move < 64) { // forward
            if (j == 1) {
                if (position.board[move] == 'k' || position.board[move] == 'c') {
                    return true;
                }
            }
            if (position.board[move] == 'r' || position.board[move] == 'h' || position.board[move] == 'q') {
                
                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - (8 * j);
        if (move >= 0) { // backward
            if (j == 1) {
                if (position.board[move] == 'k' || position.board[move] == 'c') {
                    return true;
                }
            }
            if (position.board[move] == 'r' || position.board[move] == 'h' || position.board[move] == 'q') {

                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square + j;
        if (move < 64 && move % 8 != 0) { // right
            if (j == 1) {
                if (position.board[move] == 'k' || position.board[move] == 'c') {
                    return true;
                }
            }
            if (position.board[move] == 'r' || position.board[move] == 'h' || position.board[move] == 'q') {

                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - j;
        if (move >= 0 && move % 8 != 7) { // left
            if (j == 1) {
                if (position.board[move] == 'k' || position.board[move] == 'c') {
                    return true;
                }
            }
            if (position.board[move] == 'r' || position.board[move] == 'h' || position.board[move] == 'q') {

                return true;
            }
            else if (position.board[move] < 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }

    // checking nights
    move = square + 10;
    if (move < 64 && move % 8 != 0 && move % 8 != 1) { // if it is within the bounds of the board
        if (position.board[move] == 'n') { 
            return true;
        }
    }
    move = square + 17;
    if (move < 64 && move % 8 != 0) {
        if (position.board[move] == 'n') {
            return true;
        }
    }
    move = square + 15;
    if (move < 64 && move % 8 != 7) {
        if (position.board[move] == 'n') {
            return true;
        }
    }
    move = square + 6;
    if (move < 64 && move % 8 != 7 && move % 8 != 6) {
        if (position.board[move] == 'n') {
            return true;
        }
    }
    move = square - 15;
    if (move >= 0 && move % 8 != 0) {
        if (position.board[move] == 'n') {
            return true;
        }
    }
    move = square - 6;
    if (move >= 0 && move % 8 != 0 && move % 8 != 1) {
        if (position.board[move] == 'n') {
            return true;
        }
    }
    move = square - 17;
    if (move >= 0 && move % 8 != 7) {
        if (position.board[move] == 'n') {
            return true;
        }
    }
    move = square - 10;
    if (move >= 0 && move % 8 != 7 && move % 8 != 6) {
        if (position.board[move] == 'n') {
            return true;
        }
    }

    return false;
}
inline bool isSquareInCheckByWhite(Position position, int square) {

    // checking diagonal checks, bishops, queens, pawns, and kings
    short move;
    for (short j = 1; j < 8; j++) {
        move = square + (9 * j);
        if (move < 64 && move % 8 != 0) { // forward right
            if (j == 1) {
                if (position.board[move] == 'K' || position.board[move] == 'C') // a black king or a moved black king
                {
                    return true;
                }
            }
            if (position.board[move] == 'B' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - (7 * j);
        if (move >= 0 && move % 8 != 0) { // backward right
            if (j == 1) {
                if (position.board[move] == 'P') {
                    return true;
                }
                else if (position.board[move] == 'K' || position.board[move] == 'C') // a black king or a moved black king 
                {
                    return true;
                }
            }
            if (position.board[move] == 'B' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square + (7 * j);
        if (move < 64 && move % 8 != 7) { // forward left
            if (j == 1) {
                if (position.board[move] == 'K' || position.board[move] == 'C') // a black king or a moved black king
                {
                    return true;
                }
            }
            if (position.board[move] == 'B' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - (9 * j);
        if (move >= 0 && move % 8 != 0) { // backward left
            if (j == 1) {
                if (position.board[move] == 'P') {
                    return true;
                }
                else if (position.board[move] == 'K' || position.board[move] == 'C') // a black king or a moved black king 
                {
                    return true;
                }
            }
            if (position.board[move] == 'B' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }

    // checking orthagonal checks, rooks, queens, and kings
    for (short j = 1; j < 8; j++) {
        move = square + (8 * j);
        if (move < 64) { // forward
            if (j == 1) {
                if (position.board[move] == 'K' || position.board[move] == 'C') {
                    return true;
                }
            }
            if (position.board[move] == 'R' || position.board[move] == 'H' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - (8 * j);
        if (move >= 0) { // backward
            if (j == 1) {
                if (position.board[move] == 'K' || position.board[move] == 'C') {
                    return true;
                }
            }
            if (position.board[move] == 'R' || position.board[move] == 'H' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square + j;
        if (move < 64 && move % 8 != 0) { // right
            if (j == 1) {
                if (position.board[move] == 'K' || position.board[move] == 'C') {
                    return true;
                }
            }
            if (position.board[move] == 'R' || position.board[move] == 'H' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short j = 1; j < 8; j++) {
        move = square - j;
        if (move >= 0 && move % 8 != 7) { // left
            if (j == 1) {
                if (position.board[move] == 'K' || position.board[move] == 'C') {
                    return true;
                }
            }
            if (position.board[move] == 'R' || position.board[move] == 'H' || position.board[move] == 'Q') {

                return true;
            }
            else if (position.board[move] > 96)
            {
                break; // stop looking in this direction when we see a freindly piece
            }
        }
        else
        {
            break; // reaching end of board
        }
    }

    // checking nights
    move = square + 10;
    if (move < 64 && move % 8 != 0 && move % 8 != 1) { // if it is within the bounds of the board
        if (position.board[move] == 'N') {
            return true;
        }
    }
    move = square + 17;
    if (move < 64 && move % 8 != 0) {
        if (position.board[move] == 'N') {
            return true;
        }
    }
    move = square + 15;
    if (move < 64 && move % 8 != 7) {
        if (position.board[move] == 'N') {
            return true;
        }
    }
    move = square + 6;
    if (move < 64 && move % 8 != 7 && move % 8 != 6) {
        if (position.board[move] == 'N') {
            return true;
        }
    }
    move = square - 15;
    if (move >= 0 && move % 8 != 0) {
        if (position.board[move] == 'N') {
            return true;
        }
    }
    move = square - 6;
    if (move >= 0 && move % 8 != 0 && move % 8 != 1) {
        if (position.board[move] == 'N') {
            return true;
        }
    }
    move = square - 17;
    if (move >= 0 && move % 8 != 7) {
        if (position.board[move] == 'N') {
            return true;
        }
    }
    move = square - 10;
    if (move >= 0 && move % 8 != 7 && move % 8 != 6) {
        if (position.board[move] == 'N') {
            return true;
        }
    }

    return false;
}
inline bool pieceWouldBeCaptured(Position position, short square) {
    short whiteProtectors = 0; 
    short blackProtectors = 0;
    // check diagonals
    short move;
    for (short i = 1; i < 8; i++) {
        move = square + (9 * i);
        if (move < 64 && move % 8 != 0) { // forward right
            switch (position.board[move])
            {
            case '0':
                break;
            case 'p':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'b':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'B':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
            
            
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short i = 1; i < 8; i++) {
        move = square - (7 * i);
        if (move >= 0 && move % 8 != 0) { // backward right
            switch (position.board[move])
            {
            case '0':
                break;
            case 'P':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'b':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'B':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short i = 1; i < 8; i++) {
        move = square + (7 * i);
        if (move < 64 && move % 8 != 7) { // forward left
            switch (position.board[move])
            {
            case '0':
                break;
            case 'p':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7;
                break;
            }
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'b':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'B':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short i = 1; i < 8; i++) {
        move = square - (9 * i);
        if (move >= 0 && move % 8 != 0) { // backward left
            switch (position.board[move])
            {
            case '0':
                break;
            case 'P':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7;
                break;
            }
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'b':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'B':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    // orthogonals
    for (short i = 1; i < 8; i++) {
        move = square + (8 * i);
        if (move < 64) { // forward
            switch (position.board[move])
            {
            case '0':
                break;
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'r':
            {
                blackProtectors++;
                break;
            }
            case 'h':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'R':
            {
                whiteProtectors++;
                break;
            }
            case 'H':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short i = 1; i < 8; i++) {
        move = square - (8 * i);
        if (move >= 0) { // backward
            switch (position.board[move])
            {
            case '0':
                break;
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'r':
            {
                blackProtectors++;
                break;
            }
            case 'h':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'R':
            {
                whiteProtectors++;
                break;
            }
            case 'H':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short i = 1; i < 8; i++) {
        move = square + i;
        if (move < 64 && move % 8 != 0) { // right
            switch (position.board[move])
            {
            case '0':
                break;
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'r':
            {
                blackProtectors++;
                break;
            }
            case 'h':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'R':
            {
                whiteProtectors++;
                break;
            }
            case 'H':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    for (short i = 1; i < 8; i++) {
        move = square - i;
        if (move >= 0 && move % 8 != 7) { // left
            switch (position.board[move])
            {
            case '0':
                break;
            case 'c':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'C':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'k':
            {
                if (i == 1) {
                    blackProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'K':
            {
                if (i == 1) {
                    whiteProtectors++;
                }
                i = 7; // break the loop
                break;
            }
            case 'r':
            {
                blackProtectors++;
                break;
            }
            case 'h':
            {
                blackProtectors++;
                break;
            }
            case 'q':
            {
                blackProtectors++;
                break;
            }
            case 'R':
            {
                whiteProtectors++;
                break;
            }
            case 'H':
            {
                whiteProtectors++;
                break;
            }
            case 'Q':
            {
                whiteProtectors++;
                break;
            }

            default:
            {
                i = 7; // breaking from loop
                break;
            }
            }
        }
        else
        {
            break; // reaching end of board
        }
    }
    //knights
    if (move < 64 && move % 8 != 0 && move % 8 != 1) { // if it is within the bounds of the board
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }
    move = square + 17;
    if (move < 64 && move % 8 != 0) {
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }
    move = square + 15;
    if (move < 64 && move % 8 != 7) {
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }
    move = square + 6;
    if (move < 64 && move % 8 != 7 && move % 8 != 6) {
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }
    move = square - 15;
    if (move >= 0 && move % 8 != 0) {
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }
    move = square - 6;
    if (move >= 0 && move % 8 != 0 && move % 8 != 1) {
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }
    move = square - 17;
    if (move >= 0 && move % 8 != 7) {
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }
    move = square - 10;
    if (move >= 0 && move % 8 != 7 && move % 8 != 6) {
        if (position.board[move] == 'n') {
            blackProtectors++;
        }
        if (position.board[move] == 'N') {
            whiteProtectors++;
        }
    }

    //std::cout << "white protectors: " << whiteProtectors << ", black protectors: " << blackProtectors << '\n';

    if (position.board[square] > 96) {
        if (whiteProtectors > blackProtectors) {
            return true;
        }
    }
    if (position.board[square] < 96 && position.board[square] != '0') {
        if (blackProtectors > whiteProtectors) {
            return true;
        }
    }
    return false;
}
inline float heuristicValue(Position position) {
    short moveCounter = 0;
    short moveCounterBlack = 0;
    short objectiveValue = 0;
    for (short square = 0; square < 64; square++) {
        switch (position.board[square])
        {
        case '0': // empty
        {
            break;
        }
        case 'P': // white pawn
        {
            objectiveValue += 1;

            bool hasLeftNeighbor = false;
            bool hasRightNeighbor = false;
            if (square % 8 != 7) {
                if (position.board[square + 1] == 'P' || position.board[square + 1] == 'E') {
                    hasRightNeighbor = true;
                }
                if (square + 9 < 64) {
                    if (position.board[square + 9] == 'P' || position.board[square + 9] == 'E') {
                        hasRightNeighbor = true;
                    }
                }
                if (square - 7 >= 0) {
                    if (position.board[square - 7] == 'P' || position.board[square - 7] == 'E') {
                        hasRightNeighbor = true;
                    }
                }
            }
            if (square % 8 != 0) {
                if (position.board[square - 1] == 'P' || position.board[square - 1] == 'E') {
                    hasLeftNeighbor = true;
                }
                if (square + 7 < 64) {
                    if (position.board[square + 7] == 'P' || position.board[square + 7] == 'E') {
                        hasLeftNeighbor = true;
                    }
                }
                if (square - 9 >= 0) {
                    if (position.board[square - 9] == 'P' || position.board[square - 9] == 'E') {
                        hasLeftNeighbor = true;
                    }
                }
            }
            moveCounter += hasLeftNeighbor + hasRightNeighbor;

            break;
        }

        case 'E': // white double moved pawn
        {
            objectiveValue += 1;
            bool hasLeftNeighbor = false;
            bool hasRightNeighbor = false;
            if (square % 8 != 7) {
                if (position.board[square + 1] == 'P' || position.board[square + 1] == 'E') {
                    hasRightNeighbor = true;
                }
                if (square + 9 < 64) {
                    if (position.board[square + 9] == 'P' || position.board[square + 9] == 'E') {
                        hasRightNeighbor = true;
                    }
                }
                if (square - 7 >= 0) {
                    if (position.board[square - 7] == 'P' || position.board[square - 7] == 'E') {
                        hasRightNeighbor = true;
                    }
                }
            }
            if (square % 8 != 0) {
                if (position.board[square - 1] == 'P' || position.board[square - 1] == 'E') {
                    hasLeftNeighbor = true;
                }
                if (square + 7 < 64) {
                    if (position.board[square + 7] == 'P' || position.board[square + 7] == 'E') {
                        hasLeftNeighbor = true;
                    }
                }
                if (square - 9 >= 0) {
                    if (position.board[square - 9] == 'P' || position.board[square - 9] == 'E') {
                        hasLeftNeighbor = true;
                    }
                }
            }
            moveCounter += hasLeftNeighbor + hasRightNeighbor;
            break;
        }

        case 'N': // white knight
        {
            objectiveValue += 3;
            short move = square + 10;
            if (move < 64 && move % 8 != 0 && move % 8 != 1) { // if it is within the bounds of the board
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    moveCounter++;
                }
            }
            move = square + 17;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 15;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 6;
            if (move < 64 && move % 8 != 7 && move % 8 != 6) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 15;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 6;
            if (move >= 0 && move % 8 != 0 && move % 8 != 1) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 17;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 10;
            if (move >= 0 && move % 8 != 7 && move % 8 != 6) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            break;
        }

        case 'B': // white bishop
        {
            objectiveValue += 3;
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (9 * i);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (7 * i);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + (7 * i);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (9 * i);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;

        }

        case 'R': // white rook
        {
            objectiveValue += 5;
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (8 * i);
                if (move < 64) { // forward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (8 * i);
                if (move >= 0) { // backward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + i;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - i;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'H': // white moved rook
        {
            objectiveValue += 5;
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (8 * i);
                if (move < 64) { // forward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (8 * i);
                if (move >= 0) { // backward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + i;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - i;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'Q': // white queen
        {
            objectiveValue += 9;
            /*
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (9 * i);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (7 * i);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + (7 * i);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (9 * i);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + (8 * i);
                if (move < 64) { // forward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (8 * i);
                if (move >= 0) { // backward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + i;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - i;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        moveCounter++;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            */
            break;
        }

        case 'K': // white king
        {
            objectiveValue += 100;
            /*
            short move = square + 8;
            if (move < 64) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 8;
            if (move >= 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            */
            break;
        }

        case 'C': // white moved king
        {
            objectiveValue += 100;
            /*
            short move = square + 8;
            if (move < 64) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 8;
            if (move >= 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            move = square - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    moveCounter++;
                }
            }
            */
            break;
        }

        case 'p': // black pawn
        {
            objectiveValue -= 1;
            bool hasLeftNeighbor = false;
            bool hasRightNeighbor = false;
            if (square % 8 != 7) {
                if (position.board[square + 1] == 'p' || position.board[square + 1] == 'e') {
                    hasRightNeighbor = true;
                }
                if (square + 9 < 64) {
                    if (position.board[square + 9] == 'p' || position.board[square + 9] == 'e') {
                        hasRightNeighbor = true;
                    }
                }
                if (square - 7 >= 0) {
                    if (position.board[square - 7] == 'p' || position.board[square - 7] == 'e') {
                        hasRightNeighbor = true;
                    }
                }
            }
            if (square % 8 != 0) {
                if (position.board[square - 1] == 'p' || position.board[square - 1] == 'e') {
                    hasLeftNeighbor = true;
                }
                if (square + 7 < 64) {
                    if (position.board[square + 7] == 'p' || position.board[square + 7] == 'e') {
                        hasLeftNeighbor = true;
                    }
                }
                if (square - 9 >= 0) {
                    if (position.board[square - 9] == 'p' || position.board[square - 9] == 'e') {
                        hasLeftNeighbor = true;
                    }
                }
            }
            moveCounterBlack += hasLeftNeighbor + hasRightNeighbor;
            break;
        }

        case 'e': // black double moved pawn
        {
            objectiveValue -= 1;
            bool hasLeftNeighbor = false;
            bool hasRightNeighbor = false;
            if (square % 8 != 7) {
                if (position.board[square + 1] == 'p' || position.board[square + 1] == 'e') {
                    hasRightNeighbor = true;
                }
                if (square + 9 < 64) {
                    if (position.board[square + 9] == 'p' || position.board[square + 9] == 'e') {
                        hasRightNeighbor = true;
                    }
                }
                if (square - 7 >= 0) {
                    if (position.board[square - 7] == 'p' || position.board[square - 7] == 'e') {
                        hasRightNeighbor = true;
                    }
                }
            }
            if (square % 8 != 0) {
                if (position.board[square - 1] == 'p' || position.board[square - 1] == 'e') {
                    hasLeftNeighbor = true;
                }
                if (square + 7 < 64) {
                    if (position.board[square + 7] == 'p' || position.board[square + 7] == 'e') {
                        hasLeftNeighbor = true;
                    }
                }
                if (square - 9 >= 0) {
                    if (position.board[square - 9] == 'p' || position.board[square - 9] == 'e') {
                        hasLeftNeighbor = true;
                    }
                }
            }
            moveCounterBlack += hasLeftNeighbor + hasRightNeighbor;
            break;
        }

        case 'n': // black knight
        {
            objectiveValue -= 3;
            short move = square + 10;
            if (move < 64 && move % 8 != 0 && move % 8 != 1) { // if it is within the bounds of the board
                if (position.board[move] < 96) { // if it is either empty or has an enemy piece in it
                    moveCounterBlack++;
                }
            }
            move = square + 17;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 15;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 6;
            if (move < 64 && move % 8 != 7 && move % 8 != 6) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 15;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 6;
            if (move >= 0 && move % 8 != 0 && move % 8 != 1) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 17;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 10;
            if (move >= 0 && move % 8 != 7 && move % 8 != 6) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            break;
        }

        case 'b': // black bishop
        {
            objectiveValue -= 3;
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (9 * i);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (7 * i);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + (7 * i);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (9 * i);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;

        }

        case 'r': // black rook
        {
            objectiveValue -= 5;
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (8 * i);
                if (move < 64) { // forward
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (8 * i);
                if (move >= 0) { // backward
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + i;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - i;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'h': // black moved rook
        {
            objectiveValue -= 5;
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (8 * i);
                if (move < 64) { // forward
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (8 * i);
                if (move >= 0) { // backward
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + i;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - i;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'q': // black queen
        {
            objectiveValue -= 9;
            /*
            short move;
            for (short i = 1; i < 7; i++) {
                move = square + (9 * i);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (7 * i);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + (7 * i);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (9 * i);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + (8 * i);
                if (move < 64) { // forward
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - (8 * i);
                if (move >= 0) { // backward
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square + i;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short i = 1; i < 7; i++) {
                move = square - i;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] < 96) {
                        moveCounterBlack++;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            */
            break;
        }

        case 'k': // black king
        {
            objectiveValue -= 100;
            /*
            short move = square + 8;
            if (move < 64) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 8;
            if (move >= 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            */
            break;
        }

        case 'c': // black moved king
        {
            objectiveValue -= 100;
            /*
            short move = square + 8;
            if (move < 64) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 8;
            if (move >= 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            move = square - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    moveCounterBlack++;
                }
            }
            */
            break;
        }

        default:
            break;
        }
    }
    float value = (float)objectiveValue + (((float)moveCounter) / 100.0f) - (((float)moveCounterBlack) / 100.0f);
    return value;
}
inline float getIntHeuristicValue(Position position) {
    float value = 0.0f;
    for (short i = 0; i < 64; i++) {
        switch (position.board[i])
        {
        case '0':
        {
            break;
        }
        case 'P':
        {
            value += 1.0f;
            break;
        }
        case 'E':
        {
            value += 1.0f;
            break;
        }
        case 'N':
        {
            value += 3.0f;
            break;
        }
        case 'B':
        {
            value += 3.0f;
            break;
        }
        case 'R':
        {
            value += 5.0f;
            break;
        }
        case 'H':
        {
            value += 5.0f;
            break;
        }
        case 'Q':
        {
            value += 9.0f;
            break;
        }
        case 'K':
        {
            value += 100.0f;
            break;
        }
        case 'C':
        {
            value += 100.0f;
            break;
        }

        case 'p':
        {
            value -= 1.0f;
            break;
        }
        case 'e':
        {
            value -= 1.0f;
            break;
        }
        case 'n':
        {
            value -= 3.0f;
            break;
        }
        case 'b':
        {
            value -= 3.0f;
            break;
        }
        case 'r':
        {
            value -= 5.0f;
            break;
        }
        case 'h':
        {
            value -= 5.0f;
            break;
        }
        case 'q':
        {
            value -= 9.0f;
            break;
        }
        case 'k':
        {
            value -= 100.0f;
            break;
        }
        case 'c':
        {
            value -= 100.0f;
            break;
        }

        default:
            break;
        }
    }


    return value;
}
float miniMax(Position positions[], short depth, bool white, short& bestMoveFrom, short& bestMoveTo);
inline bool createNewPositionAndCallMiniMax(Position positions[], short positionIndex, short fromSqr, short toSqr, short depth, short staticDepth, short &bestMoveFrom, short &bestMoveTo, bool white) {
    
    // copying board to child and some other things
    for (short i = 0; i < 64; i++) {
        if (positions[positionIndex].board[i] == 'e') { // resetting en passant pawns
            positions[positionIndex + 1].board[i] = 'p';
        }
        else if (positions[positionIndex].board[i] == 'E') {
            positions[positionIndex + 1].board[i] = 'P';
        }
        else
        {
            positions[positionIndex + 1].board[i] = positions[positionIndex].board[i];
        }
    }

    if (positions[positionIndex].board[toSqr] == 'K' || positions[positionIndex].board[toSqr] == 'C' || positions[positionIndex].board[toSqr] == 'k' || positions[positionIndex].board[toSqr] == 'c') { // if a king has been captured
        positions[positionIndex + 1].kingCapture = true;
    }

    positions[positionIndex + 1].lastMove = toSqr; // storing the last move
    positions[positionIndex + 1].board[toSqr] = positions[positionIndex + 1].board[fromSqr]; // moving the piece
    positions[positionIndex + 1].board[fromSqr] = '0'; // making the square it came from empty
    positions[positionIndex + 1].alpha = positions[positionIndex].alpha;
    positions[positionIndex + 1].beta = positions[positionIndex].beta;


    if (white) {
        float val = miniMax(positions, depth - 1, false, bestMoveFrom, bestMoveTo);
        if (val > positions[positionIndex].alpha) {

            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].alpha = val;
            if (val > positions[positionIndex].beta) { // prune
                return true;
            }
        }
    }
    else
    {
        float val = miniMax(positions, depth - 1, true, bestMoveFrom, bestMoveTo);

        if (val < positions[positionIndex].beta) {
            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].beta = val;
            if (positions[positionIndex].alpha > val) { // prune
                return true;
            }
        }
    }

    return false; // if we didn't return true earlier and shouldn't prune this branch
}
inline bool createNewPosChangePieceAndCallMiniMax(Position positions[], short positionIndex, short fromSqr, short toSqr, short depth, short staticDepth, short& bestMoveFrom, short& bestMoveTo, bool white, char pieceChangeTo) {
    // copying board to child and some other things
    for (short i = 0; i < 64; i++) {
        if (positions[positionIndex].board[i] == 'e') { // resetting en passant pawns
            positions[positionIndex + 1].board[i] = 'p';
        }
        else if (positions[positionIndex].board[i] == 'E') {
            positions[positionIndex + 1].board[i] = 'P';
        }
        else
        {
            positions[positionIndex + 1].board[i] = positions[positionIndex].board[i];
        }
    }

    if (positions[positionIndex].board[toSqr] == 'K' || positions[positionIndex].board[toSqr] == 'C' || positions[positionIndex].board[toSqr] == 'k' || positions[positionIndex].board[toSqr] == 'c') { // if a king has been captured
        positions[positionIndex + 1].kingCapture = true;
    }

    positions[positionIndex + 1].lastMove = toSqr; // storing the last move
    positions[positionIndex + 1].board[fromSqr] = pieceChangeTo; // changing the piece
    positions[positionIndex + 1].board[toSqr] = positions[positionIndex + 1].board[fromSqr]; // moving the piece
    positions[positionIndex + 1].board[fromSqr] = '0'; // making the square it came from empty
    positions[positionIndex + 1].alpha = positions[positionIndex].alpha;
    positions[positionIndex + 1].beta = positions[positionIndex].beta;

    if (white) {
        float val = miniMax(positions, depth - 1, false, bestMoveFrom, bestMoveTo);
        if (val > positions[positionIndex].alpha) {
            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].alpha = val;
            if (val > positions[positionIndex].beta) { // prune
                return true;
            }
        }
    }
    else
    {
        float val = miniMax(positions, depth - 1, true, bestMoveFrom, bestMoveTo);
        if (val < positions[positionIndex].beta) {
            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].beta = val;
            if (positions[positionIndex].alpha > val) { // prune
                return true;
            }
        }
    }
    return false; // if we didn't return true earlier and shouldn't prune this branch
}
inline bool createNewPosRemovePieceAndCallMiniMax(Position positions[], short positionIndex, short fromSqr, short toSqr, short depth, short staticDepth, short& bestMoveFrom, short& bestMoveTo, bool white, short removeIndex) {
    // copying board to child and some other things
    for (short i = 0; i < 64; i++) {
        if (positions[positionIndex].board[i] == 'e') { // resetting en passant pawns
            positions[positionIndex + 1].board[i] = 'p';
        }
        else if (positions[positionIndex].board[i] == 'E') {
            positions[positionIndex + 1].board[i] = 'P';
        }
        else
        {
            positions[positionIndex + 1].board[i] = positions[positionIndex].board[i];
        }
    }

    if (positions[positionIndex].board[toSqr] == 'K' || positions[positionIndex].board[toSqr] == 'C' || positions[positionIndex].board[toSqr] == 'k' || positions[positionIndex].board[toSqr] == 'c') { // if a king has been captured
        positions[positionIndex + 1].kingCapture = true;
    }

    positions[positionIndex + 1].lastMove = toSqr; // storing the last move
    positions[positionIndex + 1].board[toSqr] = positions[positionIndex + 1].board[fromSqr]; // moving the piece
    positions[positionIndex + 1].board[fromSqr] = '0'; // making the square it came from empty
    positions[positionIndex + 1].board[removeIndex] = '0'; // removing other piece
    positions[positionIndex + 1].alpha = positions[positionIndex].alpha;
    positions[positionIndex + 1].beta = positions[positionIndex].beta;

    if (white) {
        float val = miniMax(positions, depth - 1, false, bestMoveFrom, bestMoveTo);
        if (val > positions[positionIndex].alpha) {
            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].alpha = val;
            if (val > positions[positionIndex].beta) { // prune
                return true;
            }
        }
    }
    else
    {
        float val = miniMax(positions, depth - 1, true, bestMoveFrom, bestMoveTo);
        if (val < positions[positionIndex].beta) {
            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].beta = val;
            if (positions[positionIndex].alpha > val) { // prune
                return true;
            }
        }
    }
    return false; // if we didn't return true earlier and shouldn't prune this branch
}
inline bool createNewPosChangeMoveTwoPiecesAndCallMiniMax(Position positions[], short positionIndex, short fromSqr, short toSqr, short depth, short staticDepth, short& bestMoveFrom, short& bestMoveTo, bool white, char pieceChangeTo, short otherPieceTo, short otherPieceFrom, char pieceChangeTo2) {
    // copying board to child and some other things
    for (short i = 0; i < 64; i++) {
        if (positions[positionIndex].board[i] == 'e') { // resetting en passant pawns
            positions[positionIndex + 1].board[i] = 'p';
        }
        else if (positions[positionIndex].board[i] == 'E') {
            positions[positionIndex + 1].board[i] = 'P';
        }
        else
        {
            positions[positionIndex + 1].board[i] = positions[positionIndex].board[i];
        }
    }

    if (positions[positionIndex].board[toSqr] == 'K' || positions[positionIndex].board[toSqr] == 'C' || positions[positionIndex].board[toSqr] == 'k' || positions[positionIndex].board[toSqr] == 'c') { // if a king has been captured
        positions[positionIndex + 1].kingCapture = true;
    }

    positions[positionIndex + 1].lastMove = toSqr; // storing the last move
    positions[positionIndex + 1].board[fromSqr] = pieceChangeTo; // changing the piece
    positions[positionIndex + 1].board[otherPieceFrom] = pieceChangeTo2; // changing the other piece
    positions[positionIndex + 1].board[toSqr] = positions[positionIndex + 1].board[fromSqr]; // moving the piece
    positions[positionIndex + 1].board[otherPieceTo] = positions[positionIndex + 1].board[otherPieceFrom]; // moving the other piece
    positions[positionIndex + 1].board[fromSqr] = '0'; // making the square it came from empty
    positions[positionIndex + 1].board[otherPieceFrom] = '0'; // making the square the other came from empty
    positions[positionIndex + 1].alpha = positions[positionIndex].alpha;
    positions[positionIndex + 1].beta = positions[positionIndex].beta;

    if (white) {
        float val = miniMax(positions, depth - 1, false, bestMoveFrom, bestMoveTo);
        if (val > positions[positionIndex].alpha) {
            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].alpha = val;
            if (val > positions[positionIndex].beta) { // prune
                return true;
            }
        }
    }
    else
    {
        float val = miniMax(positions, depth - 1, true, bestMoveFrom, bestMoveTo);
        if (val < positions[positionIndex].beta) {
            if (depth == staticDepth) { // record best move if we are the origin
                bestMoveFrom = fromSqr;
                bestMoveTo = toSqr;
            }
            positions[positionIndex].beta = val;
            if (positions[positionIndex].alpha > val) { // prune
                return true;
            }
        }
    }
    return false; // if we didn't return true earlier and shouldn't prune this branch
}
float miniMax(Position positions[], short depth, bool white, short &bestMoveFrom, short &bestMoveTo) {
    static short staticDepth = depth;
    short depthIndex = staticDepth - depth;

    if (depth > 0 && !positions[depthIndex].kingCapture) {
        if (white) {
            for (short square = 0; square < 64; square++) { // for every square of the position
                short positionIndex = depthIndex;
                switch (positions[positionIndex].board[square])
                {
                case '0':
                    break;

                case 'P': // white pawn
                {
                    short move = square + 8;
                    if (move < 64) {
                        if (positions[positionIndex].board[move] == '0') { // forward one
                            if (move > 55) { // promotion
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'Q')) {
                                    goto PRUNED;
                                }
                            }
                            else
                            {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                            }


                            if (square < 16) { // double first move, changing to be recognized in en passant
                                move += 8; 
                                if (positions[positionIndex].board[move] == '0') {
                                    if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'E')) {
                                        goto PRUNED;
                                    }
                                }
                            }
                            
                        }
                        if (square % 8 != 0) { // diagonal left
                            move = square + 7;
                            if (positions[positionIndex].board[move] > 96) { // is a black piece in that square
                                if (move > 55) { // promotion
                                    if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'Q')) {
                                        goto PRUNED;
                                    }
                                }
                                else
                                {
                                    if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                        goto PRUNED;
                                    }
                                }
                            }

                            if (positions[positionIndex].board[square - 1] == 'e') { // capturing en passant
                                if (createNewPosRemovePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, square - 1)) {
                                    goto PRUNED;
                                }
                            }
                        }
                        if (square % 8 != 7) { // diagonal right
                            move = square + 9;
                            if (positions[positionIndex].board[move] > 96) { // is a black piece in that square
                                if (move > 55) { // promotion
                                    if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'Q')) {
                                        goto PRUNED;
                                    }
                                }
                                else
                                {
                                    if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                        goto PRUNED;
                                    }
                                }
                            }

                            if (positions[positionIndex].board[square + 1] == 'e') { // capturing en passant
                                if (createNewPosRemovePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, square + 1)) {
                                    goto PRUNED;
                                }
                            }
                        }
                    }
                    break;
                }

                case 'N': // white knight
                {
                    short move = square + 10;
                    if (move < 64 && move % 8 != 0 && move % 8 != 1) { // if it is within the bounds of the board
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') { // if it is either empty or has an enemy piece in it
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    move = square + 17;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    move = square + 15;
                    if (move < 64 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    move = square + 6;
                    if (move < 64 && move % 8 != 7 && move % 8 != 6) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    move = square - 15;
                    if (move >= 0 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    move = square - 6;
                    if (move >= 0 && move % 8 != 0 && move % 8 != 1) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    move = square - 17;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    move = square - 10;
                    if (move >= 0 && move % 8 != 7 && move % 8 != 6) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                square = 64;
                                break;
                            }
                        }
                    }
                    break;
                }

                case 'B': // white bishop
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (9 * j);
                        if (move < 64 && move % 8 != 0) { // forward right
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (7 * j);
                        if (move >= 0 && move % 8 != 0) { // backward right
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + (7 * j);
                        if (move < 64 && move % 8 != 7) { // forward left
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (9 * j);
                        if (move >= 0 && move % 8 != 0) { // backward left
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }

                    break;

                }

                case 'R': // white rook
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (8 * j);
                        if (move < 64) { // forward
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'H')) { // changing it to a moved rook
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (8 * j);
                        if (move >= 0) { // backward
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'H')) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + j;
                        if (move < 64 && move % 8 != 0) { // right
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'H')) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - j;
                        if (move >= 0 && move % 8 != 7) { // left
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'H')) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    break;
                }

                case 'H': // moved white rook
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (8 * j);
                        if (move < 64) { // forward
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (8 * j);
                        if (move >= 0) { // backward
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + j;
                        if (move < 64 && move % 8 != 0) { // right
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - j;
                        if (move >= 0 && move % 8 != 7) { // left
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    break;
                }

                case 'Q': // white queen
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (9 * j);
                        if (move < 64 && move % 8 != 0) { // forward right
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (7 * j);
                        if (move >= 0 && move % 8 != 0) { // backward right
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + (7 * j);
                        if (move < 64 && move % 8 != 7) { // forward left
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (9 * j);
                        if (move >= 0 && move % 8 != 0) { // backward left
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + (8 * j);
                        if (move < 64) { // forward
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (8 * j);
                        if (move >= 0) { // backward
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + j;
                        if (move < 64 && move % 8 != 0) { // right
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - j;
                        if (move >= 0 && move % 8 != 7) { // left
                            if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] > 96) {
                                    break; // obstructed by an enemy piece, stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    break;
                }

                case 'K': // white king
                {
                    short move = square + 8;
                    if (move < 64) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 9;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 7;
                    if (move < 64 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 9;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 8;
                    if (move >= 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 7;
                    if (move >= 0 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 1;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 1;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C')) {
                                goto PRUNED;
                            }
                        }
                    }
                    // check for castling, checking that we, and the spaces in between us and the rook are not in check, and if the rook hasn't moved yet
                    if (positions[positionIndex].board[7] == 'R') { // castling king-side
                        if (positions[positionIndex].board[6] == '0' && positions[positionIndex].board[5] == '0') {
                            if (!isSquareInCheckByBlack(positions[positionIndex], 4)) {
                                if (!isSquareInCheckByBlack(positions[positionIndex], 5)) {
                                    if (!isSquareInCheckByBlack(positions[positionIndex], 6)) {
                                        if (createNewPosChangeMoveTwoPiecesAndCallMiniMax(positions, positionIndex, square, 6, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C', 5, 7, 'H')) {
                                            goto PRUNED;
                                        }
                                    }
                                }
                            }
                        }

                    }
                    if (positions[positionIndex].board[0] == 'R') { // castling queen-side
                        if (positions[positionIndex].board[1] == '0' && positions[positionIndex].board[2] == '0' && positions[positionIndex].board[3] == '0') {
                            if (!isSquareInCheckByBlack(positions[positionIndex], 4)) {
                                if (!isSquareInCheckByBlack(positions[positionIndex], 3)) {
                                    if (!isSquareInCheckByBlack(positions[positionIndex], 2)) {
                                        if (!isSquareInCheckByBlack(positions[positionIndex], 1)) {
                                            if (createNewPosChangeMoveTwoPiecesAndCallMiniMax(positions, positionIndex, square, 2, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'C', 0, 3, 'H')) {
                                                goto PRUNED;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                }

                case 'C': // moved white king
                {
                    short move = square + 8;
                    if (move < 64) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 9;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 7;
                    if (move < 64 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 9;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 8;
                    if (move >= 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 7;
                    if (move >= 0 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 1;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 1;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] > 96 || positions[positionIndex].board[move] == '0') {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, square, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    break;
                }

                default:
                    break;
                }
            }
        }
        else
        {
            for (short i = 0; i < 64; i++) { // for every square of the position
                short square = i;
                short positionIndex = depthIndex;
                switch (positions[positionIndex].board[square])
                {
                case '0':
                    break;

                case 'p': // black pawn
                {
                    short move = square - 8;
                    if (move >= 0) {
                        if (positions[positionIndex].board[move] == '0') { // forward one
                            if (move < 8) { // promotion
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'q')) {
                                    goto PRUNED;
                                }
                            }
                            else
                            {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                            }

                            if (square > 47) { // double first move
                                move -= 8;
                                if (positions[positionIndex].board[move] == '0') {
                                    if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'e')) {
                                        goto PRUNED;
                                    }
                                }
                            }
                        }
                        if (square % 8 != 0) { // down left
                            move = square - 9;
                            if (positions[positionIndex].board[move] < 96 && positions[positionIndex].board[move] > 64) { // a white piece in that square
                                if (move < 8) { // promotion
                                    if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'q')) {
                                        goto PRUNED;
                                    }
                                }
                                else
                                {
                                    if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                        goto PRUNED;
                                    }
                                }
                            }
                            if (positions[positionIndex].board[square - 1] == 'E') { // capturing en passant
                                if (createNewPosRemovePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, square - 1)) {
                                    goto PRUNED;
                                }
                            }
                        }
                        if (square % 8 != 7) { // down right
                            move = square - 7;
                            if (positions[positionIndex].board[move] < 96 && positions[positionIndex].board[move] > 64) { // a white piece in that square
                                if (move < 8) { // promotion
                                    if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'q')) {
                                        goto PRUNED;
                                    }
                                }
                                else
                                {
                                    if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                        goto PRUNED;
                                    }
                                }
                            }
                            if (positions[positionIndex].board[square + 1] == 'E') { // capturing en passant
                                if (createNewPosRemovePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, square + 1)) {
                                    goto PRUNED;
                                }
                            }
                        }
                    }
                    break;
                }

                case 'n': // black knight
                {
                    short move = square + 10;
                    if (move < 64 && move % 8 != 0) { // if it is within the bounds of the board
                        if (positions[positionIndex].board[move] < 96) { // either empty or an enemy piece is there
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 17;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 15;
                    if (move < 64 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 6;
                    if (move < 64 && move % 8 != 7 && square % 8 > 1) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 15;
                    if (move >= 0 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 6;
                    if (move >= 0 && move % 8 != 0 && square % 8 < 6) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 17;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 10;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    break;
                }

                case 'b': // black bishop
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (9 * j);
                        if (move < 64 && move % 8 != 0) { // forward right
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (7 * j);
                        if (move >= 0 && move % 8 != 0) { // backward right
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + (7 * j);
                        if (move < 64 && move % 8 != 7) { // forward left
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (9 * j);
                        if (move >= 0 && move % 8 != 0) { // backward left
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    break;
                }

                case 'r': // black rook
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (8 * j);
                        if (move < 64) { // forward
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'h')) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (8 * j);
                        if (move >= 0) { // backward
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'h')) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + j;
                        if (move < 64 && move % 8 != 0) { // right
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'h')) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - j;
                        if (move >= 0 && move % 8 != 7) { // left
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'h')) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    break;
                }

                case 'h': // moved black rook
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (8 * j);
                        if (move < 64) { // forward
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (8 * j);
                        if (move >= 0) { // backward
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + j;
                        if (move < 64 && move % 8 != 0) { // right
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - j;
                        if (move >= 0 && move % 8 != 7) { // left
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    break;
                }

                case 'q': // black queen
                {
                    short move;
                    for (short j = 1; j < 8; j++) {
                        move = square + (9 * j);
                        if (move < 64 && move % 8 != 0) { // forward right
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (7 * j);
                        if (move >= 0 && move % 8 != 0) { // backward right
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + (7 * j);
                        if (move < 64 && move % 8 != 7) { // forward left
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (9 * j);
                        if (move >= 0 && move % 8 != 0) { // backward left
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + (8 * j);
                        if (move < 64) { // forward
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - (8 * j);
                        if (move >= 0) { // backward
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square + j;
                        if (move < 64 && move % 8 != 0) { // right
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    for (short j = 1; j < 8; j++) {
                        move = square - j;
                        if (move >= 0 && move % 8 != 7) { // left
                            if (positions[positionIndex].board[move] < 96) {
                                if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                    goto PRUNED;
                                }
                                if (positions[positionIndex].board[move] != '0') {
                                    break; // obstructed by an enemy piece,stop looking in this direction
                                }
                            }
                            else
                            {
                                break; // stop looking in this direction when we see a freindly piece
                            }
                        }
                        else
                        {
                            break; // reaching end of board
                        }
                    }
                    break;
                }

                case 'k': // black king
                {
                    short move = square + 8;
                    if (move < 64) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 9;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 7;
                    if (move < 64 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 9;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 8;
                    if (move >= 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 7;
                    if (move >= 0 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 1;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 1;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPosChangePieceAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c')) {
                                goto PRUNED;
                            }
                        }
                    }
                    // check for castling, checking that we, and the spaces in between us and the rook are not in check, and if the rook hasn't moved yet
                    if (positions[positionIndex].board[63] == 'r') { // castling king-side
                        if (positions[positionIndex].board[62] == '0' && positions[positionIndex].board[61] == '0') {
                            if (!isSquareInCheckByWhite(positions[positionIndex], 60)) {
                                if (!isSquareInCheckByWhite(positions[positionIndex], 61)) {
                                    if (!isSquareInCheckByWhite(positions[positionIndex], 62)) {
                                        if (createNewPosChangeMoveTwoPiecesAndCallMiniMax(positions, positionIndex, i, 62, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c', 61, 63, 'h')) {
                                            goto PRUNED;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (positions[positionIndex].board[56] == 'r') { // castling queen-side
                        if (positions[positionIndex].board[57] == '0' && positions[positionIndex].board[58] == '0' && positions[positionIndex].board[59] == '0') {
                            if (!isSquareInCheckByWhite(positions[positionIndex], 60)) {
                                if (!isSquareInCheckByWhite(positions[positionIndex], 59)) {
                                    if (!isSquareInCheckByWhite(positions[positionIndex], 58)) {
                                        if (!isSquareInCheckByWhite(positions[positionIndex], 57)) {
                                            if (createNewPosChangeMoveTwoPiecesAndCallMiniMax(positions, positionIndex, i, 58, depth, staticDepth, bestMoveFrom, bestMoveTo, white, 'c', 56, 59, 'h')) {
                                                goto PRUNED;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                }

                case 'c': // moved black king
                {
                    short move = square + 8;
                    if (move < 64) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 9;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 7;
                    if (move < 64 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 9;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 8;
                    if (move >= 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 7;
                    if (move >= 0 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square + 1;
                    if (move < 64 && move % 8 != 0) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    move = square - 1;
                    if (move >= 0 && move % 8 != 7) {
                        if (positions[positionIndex].board[move] < 96) {
                            if (createNewPositionAndCallMiniMax(positions, positionIndex, i, move, depth, staticDepth, bestMoveFrom, bestMoveTo, white)) {
                                goto PRUNED;
                            }
                        }
                    }
                    break;
                }

                default:
                    break;
                }
            }
        }
    }
    else // deepest layer
    {
        char lastMoveID = positions[depthIndex - 1].board[positions[depthIndex].lastMove];
        if (lastMoveID != '0') { // we captured a piece on the deepest layer
            if (!positions[depthIndex].kingCapture){ // not doing this to a king
                if (pieceWouldBeCaptured(positions[depthIndex], positions[depthIndex].lastMove)) { // if it would be captured back
                    positions[depthIndex].board[positions[depthIndex].lastMove] = '0'; // removing the piece to score the position as if it were captured back
                }
            }
            else
            {
                positions[depthIndex].kingCapture = false; // resetting
            }
        }
        //float value = getIntHeuristicValue(positions[depthIndex]); // assigning value of the position
        float value = heuristicValue(positions[depthIndex]); // assigning value of the position

        if (white) { // alpha beta assignement at deepest layer
            positions[depthIndex].alpha = value;
        }
        else
        {
            positions[depthIndex].beta = value;
        }

        /*
        if (positions[1].board[16] == 'p') {
            if (positions[2].board[16] == 'P') {
                if (positions[3].board[16] == 'h') {

                    printBoard(positions[depthIndex]);

                    std::cout << "wouldPieceBeCaptured: " << pieceWouldBeCaptured(positions[depthIndex], 16) << '\n';

                }
            }
        }*/


    }
    
PRUNED:

    if (white) { // returning the correct value to the parent
        return positions[depthIndex].alpha; 
    }
    else
    {
        return positions[depthIndex].beta;
    }
}

void findBestMove(Position positions[], short depth, bool white) {
    short bestMoveFrom, bestMoveTo;
    positions[0].alpha = -1000; // just making sure alpha and beta are reset
    positions[0].beta = 1000;

    miniMax(positions, depth, white, bestMoveFrom, bestMoveTo); // recursive function

    for (short i = 0; i < 64; i++) {
        if (white) {
            if (positions[0].board[i] == 'e') { // resetting en passant pawns from last move
                positions[0].board[i] = 'p';
            }
        }
        else
        {
            if (positions[0].board[i] == 'E') {
                positions[0].board[i] = 'P';
            }
        }
    }


    // applying the best move to the origin board after miniMax is complete, applying possible piece changes from special moves
    if (positions[0].board[bestMoveFrom] == 'P') {
        if (bestMoveTo > 55) {
            positions[0].board[bestMoveFrom] = 'Q'; // promoting to queen
        }
        short mDif = bestMoveTo - bestMoveFrom; // for en passant
        if (mDif != 8) {
            if (mDif == 16) {
                positions[0].board[bestMoveFrom] = 'E'; // changing to an en passant pawn
            }
            else
            {
                if (positions[0].board[bestMoveTo] == '0') { // meaning we captured en passant
                    positions[0].board[bestMoveTo - 8] = '0';
                }
            }
        }
    }
    if (positions[0].board[bestMoveFrom] == 'p') {
        if (bestMoveTo < 8) {
            positions[0].board[bestMoveFrom] = 'q'; // promoting to queen
        }
        short mDif = bestMoveFrom - bestMoveTo; // for en passant
        if (mDif != 8) {
            if (mDif == 16) {
                positions[0].board[bestMoveFrom] = 'e'; // changing to an en passant pawn
            }
            else
            {
                if (positions[0].board[bestMoveTo] == '0') { // meaning we captured en passant
                    positions[0].board[bestMoveTo + 8] = '0';
                }
            }
        }
    }

    if (positions[0].board[bestMoveFrom] == 'K') {
        if (bestMoveFrom + 2 == bestMoveTo) { // castling king side
            positions[0].board[7] = '0';
            positions[0].board[5] = 'H'; // moving the rook
        }
        if (bestMoveFrom - 2 == bestMoveTo) { // castling queen side
            positions[0].board[0] = '0';
            positions[0].board[3] = 'H';
        }
        positions[0].board[bestMoveFrom] = 'C';
    }
    if (positions[0].board[bestMoveFrom] == 'R') {
        positions[0].board[bestMoveFrom] = 'H'; // changing rook to a moved rook
    }
    if (positions[0].board[bestMoveFrom] == 'k') {
        if (bestMoveFrom + 2 == bestMoveTo) { 
            positions[0].board[63] = '0';
            positions[0].board[61] = 'h';
        }
        if (bestMoveFrom - 2 == bestMoveTo) { 
            positions[0].board[56] = '0';
            positions[0].board[59] = 'h';
        }
        positions[0].board[bestMoveFrom] = 'c';
    }
    if (positions[0].board[bestMoveFrom] == 'r') {
        positions[0].board[bestMoveFrom] = 'h';
    }

    positions[0].board[bestMoveTo] = positions[0].board[bestMoveFrom]; // applying the move
    positions[0].board[bestMoveFrom] = '0';

    // check for game over


    return;
}

void arrangeStartingBoard(Position &position) {
    for (short i = 16; i < 48; i++) {
        position.board[i] = '0';
    }
    for (short i = 8; i < 16; i++) {
        position.board[i] = 'P';
    }
    for (short i = 48; i < 56; i++) {
        position.board[i] = 'p';
    }
    position.board[0] = 'R';
    position.board[1] = 'N';
    position.board[2] = 'B';
    position.board[3] = 'Q';
    position.board[4] = 'K';
    position.board[5] = 'B';
    position.board[6] = 'N';
    position.board[7] = 'R';

    position.board[56] = 'r';
    position.board[57] = 'n';
    position.board[58] = 'b';
    position.board[59] = 'q';
    position.board[60] = 'k';
    position.board[61] = 'b';
    position.board[62] = 'n';
    position.board[63] = 'r';
}
void savePosition(Position position, std::string path) {
    std::ofstream myfile;
    myfile.open(path);

    for (short i = 0; i < 64; i++) {
        myfile << position.board[i];
        if (i % 8 == 7) {
            myfile << '\n';
        }
    }
    myfile.close();
}
void loadPosition(Position &dst, std::string path) {
    std::string line;
    std::string lines;
    std::ifstream myfile(path);
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
                lines += line;
        }
        myfile.close();
    }
    else std::cout << "Unable to open file\n";
    for (short i = 0; i < 72; i++) { // 72 to include the line endings
        dst.board[i] = lines[i];
    }
}

void debuggingPosition1(Position& position) {

    for (int i = 0; i < 64; i++) {
        position.board[i] = '0';
    }

    position.board[56] = 'r';
    //position.board[63] = 'r';
    position.board[0] = 'R';
    //position.board[7] = 'R';


    for (int i = 0; i < 8; i++) {
        position.board[i + 8] = 'P';
        position.board[i + 48] = 'p';
    }
    position.board[10] = '0';
    position.board[11] = '0';
    position.board[50] = '0';
    position.board[51] = '0';

    position.board[4] = 'K';
    position.board[59] = 'c';

}

void printFancyBoard(Position position, bool whitePerspective) {
    
    /*

  ( )
  < >
 /___\

  { \
 {   \
  < >
 /___\

  / \
  < >
  | |
 /___\

 _ _ _
 |   |
 |   |
 |___|

 \^|^/
  < >
  | |
 /___\

   |
___|___
  < >
  | |
 /___\


  */
    char whiteChar = 219;
    char wPawn[] = {
       ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ', ' ',' ', ' ', '(', 219, ')', ' ', ' ',  ' ', ' ',
       ' ', ' ',' ', ' ', '<', 219, '>', ' ', ' ',  ' ', ' ',
       ' ', ' ',' ', '/', 219, 219, 219, '\\', ' ', ' ', ' ',
       ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',

    };
    char bPawn[] = {
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', '(', '$', ')', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', '<', '$', '>', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', '/', '_', '_', '_', '\\', ' ', ' ', ' ',
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',

    };
    char wKnight[] = {
        ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
        ' ',' ',' ', ' ', '{', 219, '\\', ' ', ' ', ' ', ' ',
        ' ',' ',' ', '{', 219, 219, 219, '\\', ' ', ' ', ' ',
        ' ',' ',' ', ' ', '<', 219, '>', ' ', ' ',  ' ', ' ',
        ' ',' ',' ', '/', 219, 219, 219, '\\', ' ', ' ', ' ',
        ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',

    };
    char bKnight[] = {
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', '{', '$', '\\', ' ', ' ', ' ', ' ',
       ' ',' ',' ', '{', '$', '$', '$', '\\', ' ', ' ', ' ',
       ' ',' ',' ', ' ', '<', '$', '>', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', '/', '_', '_', '_', '\\', ' ', ' ', ' ',
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',

    };
    char wBishop[] = {
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', ' ', '/', 219, '\\', ' ', ' ', ' ', ' ',
      ' ',' ',' ', ' ', '<', 219, '>', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', ' ', '|', 219, '|', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', '/', 219, 219, 219, '\\', ' ', ' ', ' ',
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };
    char bBishop[] = {
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', '/', '$', '\\', ' ', ' ', ' ', ' ',
       ' ',' ',' ', ' ', '<', '$', '>', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', '|', '$', '|', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', '/', '_', '_', '_', '\\', ' ', ' ', ' ',
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };
    char bRook[] = {
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', '=', '_', '=', '_', '=', ' ',  ' ', ' ',
      ' ',' ',' ', '|', '$', '$', '$', '|', ' ',  ' ', ' ',
      ' ',' ',' ', '|', '$', '$', '$', '|', ' ',  ' ', ' ',
      ' ',' ',' ', '|', '_', '_', '_', '|', ' ',  ' ', ' ',
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };
    char wRook[] = {
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', '=', '_', '=', '_', '=', ' ',  ' ', ' ',
      ' ',' ',' ', '|', 219, 219, 219, '|', ' ',  ' ', ' ',
      ' ',' ',' ', '|', 219, 219, 219, '|', ' ',  ' ', ' ',
      ' ',' ',' ', '|', 219, 219, 219, '|', ' ',  ' ', ' ',
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };
    char bQueen[] = {
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', '\\', '^', '|', '^', '/', ' ', ' ', ' ',
       ' ',' ',' ', ' ', '<', '$', '>', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', '|', '$', '|', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', '/', '_', '_', '_', '\\', ' ', ' ', ' ',
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };
    char wQueen[] = {
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', '\\', '^', '|', '^', '/', ' ', ' ', ' ',
       ' ',' ',' ', ' ', '<', 219, '>', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', ' ', '|', 219, '|', ' ', ' ',  ' ', ' ',
       ' ',' ',' ', '/', 219, 219, 219, '\\', ' ', ' ', ' ',
       ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };
    char bKing[] = {
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', '_', '_', '|', '_', '_', ' ',  ' ', ' ',
      ' ',' ',' ', ' ', '<', '$', '>', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', ' ', '|', '$', '|', ' ', ' ',  ' ', ' ',
      ' ',' ',' ', '/', '_', '_', '_', '\\', ' ', ' ', ' ',
      ' ',' ',' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };
    char wKing[] = {
     ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
     ' ',' ', ' ', '_', '_', '|', '_', '_', ' ',  ' ', ' ',
     ' ',' ', ' ', ' ', '<', 219, '>', ' ', ' ',  ' ', ' ',
     ' ',' ', ' ', ' ', '|', 219, '|', ' ', ' ',  ' ', ' ',
     ' ',' ', ' ', '/', 219, 219, 219, '\\', ' ', ' ', ' ',
     ' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ', ' ',
    };

    const int squareSize = 11;
    const int boardRowSize = squareSize * 8;
    const int bufferSize = squareSize * 6 * 64;
    char boardBuffer[bufferSize];
    int counter = 0;
    char rowIDbuffer[8];

    char whiteSquareChar = '.';
    char blackSquareChar = ' ';

    for (short i = 7; i >= 0; i--){
        int newI = ((7 - i) * (1 - whitePerspective)) + (i * whitePerspective);
        for (int j = 0; j < 8; j++) {
            int newJ = ((7 - j) * (1 - whitePerspective)) + (j * whitePerspective);
            rowIDbuffer[j] = position.board[newI * 8 + newJ];
        }

        for (int l = 0; l < 6; l++) { // for each row row
            for (int j = 0; j < 8; j++) { // for each column
                for (int k = 0; k < 11; k++) { // for each column column

                    switch (rowIDbuffer[j])
                    {

                    case 'P':
                    {
                        if (wPawn[k + (l * squareSize)] == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = wPawn[k + (l * squareSize)];
                            counter++;
                        }
                        break;
                    }
                    case 'E':
                    {
                        if (wPawn[k + (l * squareSize)] == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = wPawn[k + (l * squareSize)];
                            counter++;
                        }
                        break;
                    }
                    case 'p':
                    {
                        char c = bPawn[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }
                            
                        }
                        break;
                    }
                    case 'e':
                    {
                        char c = bPawn[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }

                    case 'N':
                    {
                        char c = wKnight[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = c;
                            counter++;
                        }
                        break;
                    }
                    case 'n':
                    {
                        char c = bKnight[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }

                    case 'B':
                    {
                        char c = wBishop[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = c;
                            counter++;
                        }
                        break;
                    }
                    case 'b':
                    {
                        char c = bBishop[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }

                    case 'R':
                    {
                        char c = wRook[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = c;
                            counter++;
                        }
                        break;
                    }
                    case 'H':
                    {
                        char c = wRook[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = c;
                            counter++;
                        }
                        break;
                    }
                    case 'r':
                    {
                        char c = bRook[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }
                    case 'h':
                    {
                        char c = bRook[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }

                    case 'Q':
                    {
                        char c = wQueen[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = c;
                            counter++;
                        }
                        break;
                    }
                    case 'q':
                    {
                        char c = bQueen[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }

                    case 'K':
                    {
                        char c = wKing[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = c;
                            counter++;
                        }
                        break;
                    }
                    case 'C':
                    {
                        char c = wKing[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            boardBuffer[counter] = c;
                            counter++;
                        }
                        break;
                    }
                    case 'k':
                    {
                        char c = bKing[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }
                    case 'c':
                    {
                        char c = bKing[k + (l * squareSize)];
                        if (c == ' ') {
                            if (j % 2 == (i % 2)) {
                                boardBuffer[counter] = blackSquareChar;
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = whiteSquareChar;
                                counter++;
                            }
                        }
                        else
                        {
                            if (c == '$') {
                                boardBuffer[counter] = ' ';
                                counter++;
                            }
                            else
                            {
                                boardBuffer[counter] = c;
                                counter++;
                            }

                        }
                        break;
                    }

                    default:
                        
                        if (j % 2 == (i % 2)) {
                            boardBuffer[counter] = blackSquareChar;
                            counter++;
                        }
                        else
                        {
                            boardBuffer[counter] = whiteSquareChar;
                            counter++;
                        }

                        break;
                    }
                }
            }
        }
    }

    //std::cout << '\n';
    for (int i = 0; i < bufferSize; i++) {
        std::cout << boardBuffer[i];
        if (i % boardRowSize == boardRowSize - 1) {
            std::cout << '\n';
        }
    }

}
int notationToInt(std::string square, int indexMod) {

    int sLength = square.length();

    if (sLength >= indexMod - 1) {

        std::string str = "";
        if (square[indexMod] >= 97 && square[indexMod] <= 104) { // the first char

            if (square[1 + indexMod] >= 49 && square[1 + indexMod] <= 56) { // the second char

                int x = square[indexMod] - 97; // 0 - 7
                int y = square[1 + indexMod] - 49; // 0 - 7

                //debugging
                //std::cout << "indexMod + 1: " << (1 + (int)indexMod) << ", square: " << square << '\n';
                //std::wcout << "x and y: " << x << ", " << y << '\n';

                return (y * 8) + x;
            }
            else
            {
                //std::cout << "y out of bounds of the board\n";
                return -1; // error
            }
        }
        else
        {
            //std::cout << "x out of bounds of the board\n";
            return -1; // error
        }
    }
    else
    {
        //std::cout << "string too small " << square.length() << ", " << indexMod - 1 << '\n';
        return -1; // error
    }
}
bool moveIsLegal(Position position, int from, int to) {

    char legalMoves[23];
    char legalMovesCounter = 0;
    for (char i = 0; i < 23; i++) {
        legalMoves[i] = -1;
    }

        switch (position.board[from])
        {
        case '0':
            break;

        case 'P': // white pawn
        {
            short move = from + 8;
            if (move < 64) {
                if (position.board[move] == '0') { // forward one
                    legalMoves[legalMovesCounter++] = move;

                    if (from < 16) { // double first move, changing to be recognized in en passant
                        move += 8;
                        if (position.board[move] == '0') {
                            legalMoves[legalMovesCounter++] = move;
                        }
                    }
                }
                if (from % 8 != 0) { // diagonal left
                    move = from + 7;
                    if (position.board[move] > 96) { // is a black piece in that square
                        legalMoves[legalMovesCounter++] = move;
                    }

                    if (position.board[from - 1] == 'e') { // capturing en passant
                        legalMoves[legalMovesCounter++] = move;
                    }
                }
                if (from % 8 != 7) { // diagonal right
                    move = from + 9;
                    if (position.board[move] > 96) { // is a black piece in that square
                        legalMoves[legalMovesCounter++] = move;
                    }

                    if (position.board[from + 1] == 'e') { // capturing en passant
                        legalMoves[legalMovesCounter++] = move;
                    }
                }
            }
            break;
        }

        case 'N': // white knight
        {
            short move = from + 10;
            if (move < 64 && move % 8 != 0 && move % 8 != 1) { // if it is within the bounds of the board
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 17;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 15;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 6;
            if (move < 64 && move % 8 != 7 && move % 8 != 6) {
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 15;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 6;
            if (move >= 0 && move % 8 != 0 && move % 8 != 1) {
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 17;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 10;
            if (move >= 0 && move % 8 != 7 && move % 8 != 6) {
                if (position.board[move] > 96 || position.board[move] == '0') { // if it is either empty or has an enemy piece in it
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            break;
        }

        case 'B': // white bishop
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (9 * j);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (7 * j);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + (7 * j);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (9 * j);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }

            break;

        }

        case 'R': // white rook
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (8 * j);
                if (move < 64) { // forward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (8 * j);
                if (move >= 0) { // backward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + j;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - j;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'H': // moved white rook
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (8 * j);
                if (move < 64) { // forward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (8 * j);
                if (move >= 0) { // backward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + j;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - j;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'Q': // white queen
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (9 * j);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (7 * j);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + (7 * j);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (9 * j);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + (8 * j);
                if (move < 64) { // forward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (8 * j);
                if (move >= 0) { // backward
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + j;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - j;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] > 96 || position.board[move] == '0') {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] > 96) {
                            break; // obstructed by an enemy piece, stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'K': // white king
        {
            short move = from + 8;
            if (move < 64) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 8;
            if (move >= 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            // check for castling, checking that we, and the spaces in between us and the rook are not in check, and if the rook hasn't moved yet
            if (position.board[7] == 'R') { // castling king-side
                if (position.board[6] == '0' && position.board[5] == '0') {
                    if (!isSquareInCheckByBlack(position, 4)) {
                        if (!isSquareInCheckByBlack(position, 5)) {
                            if (!isSquareInCheckByBlack(position, 6)) {
                                legalMoves[legalMovesCounter++] = 6;
                            }
                        }
                    }
                }

            }
            if (position.board[0] == 'R') { // castling queen-side
                if (position.board[1] == '0' && position.board[2] == '0' && position.board[3] == '0') {
                    if (!isSquareInCheckByBlack(position, 4)) {
                        if (!isSquareInCheckByBlack(position, 3)) {
                            if (!isSquareInCheckByBlack(position, 2)) {
                                if (!isSquareInCheckByBlack(position, 1)) {
                                    legalMoves[legalMovesCounter++] = 2;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        case 'C': // moved white king
        {
            short move = from + 8;
            if (move < 64) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 8;
            if (move >= 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] > 96 || position.board[move] == '0') {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            break;
        }

        case 'p': // black pawn
        {
            short move = from - 8;
            if (move >= 0) {
                if (position.board[move] == '0') { // forward one
                    legalMoves[legalMovesCounter++] = move;
                    if (from > 47) { // double first move
                        move -= 8;
                        if (position.board[move] == '0') {
                            legalMoves[legalMovesCounter++] = move;
                        }
                    }
                }
                if (from % 8 != 0) { // down left
                    move = from - 9;
                    if (position.board[move] < 96 && position.board[move] > 64) { // a white piece in that square
                        legalMoves[legalMovesCounter++] = move;
                    }
                    if (position.board[from - 1] == 'E') { // capturing en passant
                        legalMoves[legalMovesCounter++] = move;
                    }
                }
                if (from % 8 != 7) { // down right
                    move = from - 7;
                    if (position.board[move] < 96 && position.board[move] > 64) { // a white piece in that square
                        legalMoves[legalMovesCounter++] = move;
                    }
                    if (position.board[from + 1] == 'E') { // capturing en passant
                        legalMoves[legalMovesCounter++] = move;
                    }
                }
            }
            break;
        }

        case 'n': // black knight
        {
            short move = from + 10;
            if (move < 64 && move % 8 != 0) { // if it is within the bounds of the board
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 17;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 15;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 6;
            if (move < 64 && move % 8 != 7 && from % 8 > 1) {
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 15;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 6;
            if (move >= 0 && move % 8 != 0 && from % 8 < 6) {
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 17;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 10;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) { // either empty or an enemy piece is there
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            break;
        }

        case 'b': // black bishop
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (9 * j);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (7 * j);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + (7 * j);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (9 * j);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'r': // black rook
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (8 * j);
                if (move < 64) { // forward
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (8 * j);
                if (move >= 0) { // backward
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + j;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - j;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'h': // moved black rook
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (8 * j);
                if (move < 64) { // forward
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (8 * j);
                if (move >= 0) { // backward
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + j;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - j;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'q': // black queen
        {
            short move;
            for (short j = 1; j < 8; j++) {
                move = from + (9 * j);
                if (move < 64 && move % 8 != 0) { // forward right
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (7 * j);
                if (move >= 0 && move % 8 != 0) { // backward right
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + (7 * j);
                if (move < 64 && move % 8 != 7) { // forward left
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (9 * j);
                if (move >= 0 && move % 8 != 0) { // backward left
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + (8 * j);
                if (move < 64) { // forward
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - (8 * j);
                if (move >= 0) { // backward
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from + j;
                if (move < 64 && move % 8 != 0) { // right
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            for (short j = 1; j < 8; j++) {
                move = from - j;
                if (move >= 0 && move % 8 != 7) { // left
                    if (position.board[move] < 96) {
                        legalMoves[legalMovesCounter++] = move;
                        if (position.board[move] != '0') {
                            break; // obstructed by an enemy piece,stop looking in this direction
                        }
                    }
                    else
                    {
                        break; // stop looking in this direction when we see a freindly piece
                    }
                }
                else
                {
                    break; // reaching end of board
                }
            }
            break;
        }

        case 'k': // black king
        {
            short move = from + 8;
            if (move < 64) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 8;
            if (move >= 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            // check for castling, checking that we, and the spaces in between us and the rook are not in check, and if the rook hasn't moved yet
            if (position.board[63] == 'r') { // castling king-side
                if (position.board[62] == '0' && position.board[61] == '0') {
                    if (!isSquareInCheckByWhite(position, 60)) {
                        if (!isSquareInCheckByWhite(position, 61)) {
                            if (!isSquareInCheckByWhite(position, 62)) {
                                legalMoves[legalMovesCounter++] = 62;
                            }
                        }
                    }
                }
            }
            if (position.board[56] == 'r') { // castling queen-side
                if (position.board[57] == '0' && position.board[58] == '0' && position.board[59] == '0') {
                    if (!isSquareInCheckByWhite(position, 60)) {
                        if (!isSquareInCheckByWhite(position, 59)) {
                            if (!isSquareInCheckByWhite(position, 58)) {
                                if (!isSquareInCheckByWhite(position, 57)) {
                                    legalMoves[legalMovesCounter++] = 58;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        case 'c': // moved black king
        {
            short move = from + 8;
            if (move < 64) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 9;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 7;
            if (move < 64 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 9;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 8;
            if (move >= 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 7;
            if (move >= 0 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from + 1;
            if (move < 64 && move % 8 != 0) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            move = from - 1;
            if (move >= 0 && move % 8 != 7) {
                if (position.board[move] < 96) {
                    legalMoves[legalMovesCounter++] = move;
                }
            }
            break;
        }

        default:
            break;
        }
    
    for (char i = 0; i < legalMovesCounter; i++) {
        if (legalMoves[i] == to) {
            return true;
        }
    }
    return false;
}

void moveForPlayer(Position &position, bool white, int from, int to) {

    for (short i = 0; i < 64; i++) {
        if (white) {
            if (position.board[i] == 'e') { // resetting en passant pawns from last move
                position.board[i] = 'p';
            }
        }
        else
        {
            if (position.board[i] == 'E') {
                position.board[i] = 'P';
            }
        }
    }


    // applying the best move to the origin board after miniMax is complete, applying possible piece changes from special moves
    if (position.board[from] == 'P') {
        if (to > 55) {
            position.board[from] = 'Q'; // promoting to queen
        }
        short mDif = to - from; // for en passant
        if (mDif != 8) {
            if (mDif == 16) {
                position.board[from] = 'E'; // changing to an en passant pawn
            }
            else
            {
                if (position.board[to] == '0') { // meaning we captured en passant
                    position.board[to - 8] = '0';
                }
            }
        }
    }
    if (position.board[from] == 'p') {
        if (to < 8) {
            position.board[from] = 'q'; // promoting to queen
        }
        short mDif = from - to; // for en passant
        if (mDif != 8) {
            if (mDif == 16) {
                position.board[from] = 'e'; // changing to an en passant pawn
            }
            else
            {
                if (position.board[to] == '0') { // meaning we captured en passant
                    position.board[to + 8] = '0';
                }
            }
        }
    }

    if (position.board[from] == 'K') {
        if (from + 2 == to) { // castling king side
            position.board[7] = '0';
            position.board[5] = 'H'; // moving the rook
        }
        if (from - 2 == to) { // castling queen side
            position.board[0] = '0';
            position.board[3] = 'H';
        }
        position.board[from] = 'C';
    }
    if (position.board[from] == 'R') {
        position.board[from] = 'H'; // changing rook to a moved rook
    }
    if (position.board[from] == 'k') {
        if (from + 2 == to) {
            position.board[63] = '0';
            position.board[61] = 'h';
        }
        if (from - 2 == to) {
            position.board[56] = '0';
            position.board[59] = 'h';
        }
        position.board[from] = 'c';
    }
    if (position.board[from] == 'r') {
        position.board[from] = 'h';
    }

    position.board[to] = position.board[from]; // applying the move
    position.board[from] = '0';
}

void DisplayHelpMenu() {

    



}

inline COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
    {
        return cbsi.dwCursorPosition;
    }
    else
    {
        // The function failed. Call GetLastError() for details.
        COORD invalid = { 0, 0 };

        std::cout << "WTF";
        while (true);


        return invalid;
    }
}

void clearScreen(HANDLE handle) {
    int consoleStartPositionY = GetConsoleCursorPosition(handle).Y;
    COORD dCoord;
    dCoord.X = 0; 
    dCoord.Y = 8 * 6;
    SetConsoleCursorPosition(handle, dCoord);

    for (int r = 48; r < consoleStartPositionY; r++) {
        for (int c = 0; c < 300; c++) {
            std::cout << ' ';
        }
        std::cout << '\n';
    }
    dCoord.Y = 0;
    SetConsoleCursorPosition(handle, dCoord);
}

int main()
{
    std::string input;

    bool playerIsWhite = true;
    int difficultyLevel = 4;
    //std::cout << "Welcome! Enter ? at anytime to see the help menu";
    //std::cout << '\n';
    std::cout << "Choose a color, enter W or B: ";
    std::cin >> input;
    if (input[0] != 'W' && input[0] != 'w') {
        playerIsWhite = false;
        std::cout << " You chose black\n";
    }
    else
    {
        std::cout << " You chose white\n";
    }

    std::cout << "Select difficulty level, 1-5: ";
    std::cin >> input;
    difficultyLevel = input[0] - 48;
    if (difficultyLevel > 5) {
        difficultyLevel = 5;
    }
    if (difficultyLevel < 1) {
        difficultyLevel = 1;
    }
    std::cout << " You chose level " << difficultyLevel << '\n';

    HANDLE hStdout;
    COORD destCoord;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    destCoord.X = 0;
    destCoord.Y = 0;
    SetConsoleCursorPosition(hStdout, destCoord);


    const short depth = difficultyLevel;
    Position positions[6]; // buffer for position data, the first element is the current position, just making 6 being the maximum depth the player can choose

    arrangeStartingBoard(positions[0]);
    //loadPosition(positions[0], "debug17.txt"); // changes alpha to 0 for some reason
    //debuggingPosition1(positions[0]);

    Position lastPosition = Position();
    copyPosition(lastPosition, positions[0]);


    if (!playerIsWhite) {
        findBestMove(positions, depth, true);
    }

    std::cout << "Example move: type \"e2,e4\" and hit enter. Type \"back\" and hit enter to takeback your move.\n";
    printFancyBoard(positions[0], playerIsWhite);

    while (true)
    {
    TRYAGAIN:;
        std::cin >> input;
        if (input[0] == 'S') {
            std::cout << "enter file name: \n";
            std::cin >> input;
            savePosition(lastPosition, input);
        }
        else if (input[0] == 'L') {
            std::cout << "enter file name: \n";
            std::cin >> input;
            loadPosition(positions[0], input);
        }
        else if (input == "back") {
            copyPosition(positions[0], lastPosition);

            clearScreen(hStdout);

            std::cout << "Example move: type \"e2,e4\" and hit enter. Type \"back\" and hit enter to takeback your move.\n";
            printFancyBoard(positions[0], playerIsWhite);
            std::cout << "           " << '\n';
            std::cout << "Heuristic value: " << heuristicValue(positions[0]) << '\n';
            std::cout << '\n';
            destCoord.X = 0;
            destCoord.Y = 8 * 6 + 4;
            std::cout << "         ";
            destCoord.X = 0;
            SetConsoleCursorPosition(hStdout, destCoord);
            std::cin >> input;
        }
        else if (notationToInt(input, 0) != -1 && notationToInt(input, 3) != -1) {

            if (moveIsLegal(positions[0], notationToInt(input, 0), notationToInt(input, 3))) {
                moveForPlayer(positions[0], playerIsWhite, notationToInt(input, 0), notationToInt(input, 3));
            }
            else
            {
                std::cout << "illegal move, try again\n";
                goto TRYAGAIN;
            }
        }
        else
        {
            std::cout << "invalid input, try again\n";
            goto TRYAGAIN;
        }

        copyPosition(lastPosition, positions[0]);


        auto start = high_resolution_clock::now();

        findBestMove(positions, depth, !playerIsWhite);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        std::cout << '\n';
        std::cout << '\n';

        //position cursor at start of window
        
        clearScreen(hStdout);

        std::cout << "Example move: type \"e2,e4\" and hit enter. Type \"back\" and hit enter to takeback your move.\n";
        printFancyBoard(positions[0], playerIsWhite);
        std::cout << "           " << '\n';
        //printBoard(positions[0]);
        std::cout << "Heuristic value: " << heuristicValue(positions[0]) << '\n';
        std::cout << duration.count() / 1000.0 << " milliseconds" << '\n';
        
        destCoord.X = 0;
        destCoord.Y = 8 * 6 + 4;
        std::cout << "         ";
        destCoord.X = 0;
        SetConsoleCursorPosition(hStdout, destCoord);
        
    }

}

// lower case = black, uppercase = white, empty is '0'


// TO-DO LIST
//figure out a way to stop black's blunder in debug12.txt, it thinks it would win white's bishop if it dared taking the pawn, but doesn't see that the bishop can both trade away and threaten the king (or queen) at the same time

// check and checkmate - partly implemented, needs testing
// castling - implemented, needs testing



