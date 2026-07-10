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



#include "general.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>



/**
 * Adds new lines to a given string based on the requested line width.
 * @param input Input string
 * @param width Characters per line
 * @param indent Indent to include after newly inserted new line
 * @param trim Flags that any trailing newlines should be removed
 * @return Number of lines in the string
 */
int formatNewLines(char *input, int width, char *indent, int trim)
{
    if (!input || width < 1) return 0;

    // Initialse variables that help us track progress
    size_t inputStrLen = strlen(input);
    size_t indentLen = indent ? strlen(indent) : 0;
    int lines = 1;
    int lastSpace = -1;
    int widthCount = 1;

    // Iterate through the input string to find line breaks or places to add new ones
    for (int i = 0; i < inputStrLen; i++)
    {
        if (input[i] == '\033')
        {
            while (i < inputStrLen && input[i] != 'm') i++;
            if (i >= inputStrLen) break;
            continue; 
        }
        
        // Track where the last space was in case so we can go back for a future word wrap
        if (input[i] == ' ') lastSpace = i;
        // Reset tracking and take into account if we find an existing new line
        else if (input[i] == '\n')
        {
            lines++;
            widthCount = 0;
            continue;
        }

        // Begin word wrapping once the line width is saturated
        if (widthCount == width)
        {
            if (lastSpace != -1)
            {
                input[lastSpace] = '\n';
                lines++;

                if (indent && indentLen > 0)
                {
                    memmove(input + lastSpace + 1 + indentLen, input + lastSpace + 1, inputStrLen - lastSpace);
                    memcpy(input + lastSpace + 1, indent, indentLen);
                    inputStrLen += indentLen;
                    if (lastSpace <= i) i += indentLen;
                }
            }
            widthCount = i - lastSpace;
        }

        widthCount++;
    }

    // If desired, strip possible trailing new line
    if (trim)
    {
        int end = strlen(input) - 1;
        while (end >= 0 && input[end] == '\n')
        {
            input[end] = '\0';
            end--;
            lines--;
        }
    }

    return lines;
}

/**
 * @return winsize struct containing the current terminal size in columns and rows
 */
struct winsize getTerminalSize(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        ws.ws_col = 80;
        ws.ws_row = 24;
    }
    return ws;
}

/**
 * @param prog Program's executable name or full path
 * @param isExec Flags if the function should also check if a found program has
 *               execute permissions
 * @returns 1 if program is installed; 0 if not
 */
int isProgramInstalled(char *prog, int isExec)
{
    int mode = isExec ? X_OK : F_OK;

    // If prog contains '/' treat it as a full path
    if (strchr(prog, '/') != NULL)
        return (access(prog, mode) == 0);

    char *path = getenv("PATH");
    if (!path)
    {
        char cmd[64];
        snprintf(cmd, 64, "%s --version > /dev/null 2>&1", prog);
        return (system(cmd) == 0);
    }

    char *paths = strdup99(path);
    char *dir = strtok(paths, ":");
    while (dir)
    {
        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, prog);
        if (access(fullPath, mode) == 0)
        {
            free(paths);
            return 1;
        }
        dir = strtok(NULL, ":");
    }
    free(paths);

    // Also try /usr/libexec
    char libexecPath[PATH_MAX];
    snprintf(libexecPath, PATH_MAX, "/usr/libexec/%s", prog);
    if (access(libexecPath, mode) == 0) return 1;

    return 0;
}

/**
 * A C99-only implementation of POSIX's strdup. Creates and returns a heap
 * allocated copy of the input string.
 * @param input Input string to duplicate
 * @return Copy of the input string; NULL if input was also NULL or if error
 */
char *strdup99(char* input)
{
    if (!input)
        return NULL;

    size_t len = strlen(input) + 1;
    char *copy = malloc(len);
    if (copy)
        memcpy(copy, input, len);

    return copy;
}
