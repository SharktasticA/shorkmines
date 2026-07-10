/*
    ######################################################
    ##                  SHORK UTILITY                   ##
    ######################################################
    ## General, utility functions for SHORK Utilities & ##
    ## SHORK ENTERTAINMENT                              ##
    ######################################################
    ## Licence: MIT                                     ##
    ######################################################
    ## Kali (links.sharktastica.co.uk)                  ##
    ######################################################
*/



#ifndef GENERAL
#define GENERAL

#include <dirent.h>
#include <stdio.h>



int formatNewLines(char *, int, char *, int);
struct winsize getTerminalSize(void);
int isProgramInstalled(char*, int);
void splitText(char*, char*[], int);
char *strdup99(char*);

#endif
