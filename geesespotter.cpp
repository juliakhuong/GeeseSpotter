#include "geesespotter_lib.h"
#include <iostream>

char *createBoard(std::size_t xdim, std::size_t ydim){
    //The pointer, board, points to a char array with the length [xdim...]. new returns the address
    //of the dynamically allocated variable.
    char* board = new char[xdim*ydim];
    for(std::size_t i = 0; i < xdim*ydim; ++i){
        board[i] = 0;
    }
    
    return board;
}

void cleanBoard(char *board){
    delete[] board;
}

void hideBoard(char *board, std::size_t xdim, std::size_t ydim){
    for(std::size_t i = 0; i < xdim*ydim; ++i){
     board[i] = hiddenBit() ^ board[i];
    }
}

void printBoard(char *board, std::size_t xdim, std::size_t ydim){
    for(std::size_t i = 0; i < xdim*ydim; ++i){
        //if marked bit & value = marked bit, then it's a marked value
        /* Explanation: Let the value = 8. Assume the value is marked. 
        As such, the marked value is 0x38.

            0010 0000 --> marked bit
        &          --> bitwise 'and' operator
            0011 1000 --> marked value
            ___________
            0010 0000 --> the resulting value of this operation. Note that it equals the marked bit.
        */
        if( (markedBit() & board[i]) == markedBit() ){
            std::cout << 'M';
        }

        //if hidden bit & value = hidden bit, then it's a hidden value
        else if( (hiddenBit() & board[i]) == hiddenBit() ){
            std::cout << '*';
        }

        //if hidden bit & value == 0, then it's a revealed value
        else if( (hiddenBit() & board[i]) == 0x00){
            std::cout << (board[i] & valueMask());
        }

        if( (i != 0) && ((i+1)%xdim == 0) ){
            std::cout << std::endl;
        }
    }
    
    
}

int mark(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc){
    std::size_t i = yloc*xdim + xloc;
    
    //if the value is already revealed
    if( (hiddenBit() & board[i]) == 0x00){
        return 2;
    }
    
    //if the value is already marked
    else if( (markedBit() & board[i]) == markedBit() ){
        board[i] = board[i] ^ markedBit();
    }

    //if the value is hidden
    else if( (hiddenBit() & board[i]) == hiddenBit() ){
        board[i] = board[i] | markedBit();
    }

    return 0;
}

void computeNeighbors(char *board, std::size_t xdim, std::size_t ydim){

    for(std::size_t i = 0; i < xdim*ydim; ++i){
        int xloc = i % xdim;
        int yloc = i / xdim;
        int counter = 0; //keeps count of the number of geese

        if(board[i] & valueMask() == 0x09){ //if value is 9 then move onto next
            continue; 
        }
        
        for(int j = xloc - 1; j <= xloc + 1; ++j){ //where j is the xloc and k is yloc
            for(int k = yloc - 1; k <= yloc + 1; ++k){
                if(j == xloc && k == yloc){
                    continue;
                }

                if( (j >= 0) && (j < xdim) && (k >= 0) && (k < ydim) ){ //if both j and k are inside bounds
                    int temp = k * xdim + j; //temp is the index at coords (j,k)
                     if( ( board[temp] & valueMask() ) == 0x09){
                        counter++;
                    }
                }
            }
        }

        board[i] = board[i] | counter;
    }
}

int reveal(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc){
    int index = yloc * xdim + xloc;
    //if field is revealed
    if( (hiddenBit() & board[index]) == 0x00){
        return 2;
    }
    
    //if field is marked
    if( (markedBit() & board[index]) == markedBit() ){
        return 1;
    }

    //reveal the value
    board[index] = board[index] & valueMask();

    //if the revealed value is a goose
    if(board[index] == 0x09){
        return 9;
    }

    //if field is equal to zero
    if( ( board[index] & valueMask() ) == 0){
        for(int j = xloc - 1; j <= xloc + 1; ++j){ //where j is the xloc and k is yloc
            for(int k = yloc - 1; k <= yloc + 1; ++k){

                if(j == xloc && k == yloc){
                    continue;
                }

                if( (j >= 0) && (j < xdim) && (k >= 0) && (k < ydim) ){

                    int temp = k * xdim + j; //temp is the index at coords (j,k)
                board[temp] = board[temp] & valueMask();
                }   
            }
        }
    }

    return 0;
}

bool isGameWon(char *board, std::size_t xdim, std::size_t ydim){
    for(int i = 0; i < xdim*ydim; ++i){
        //if its a geese
        if( (board[i] & valueMask() ) == 0x09){
            continue;
        }

        //if it is not revealed
        if( (board[i] & 0x30) != 0){
            return false;
        }
    }
    return true;
}