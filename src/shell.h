#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

/** Resets the line editor and prints the banner and prompt. */
void shell_init(void);

/** Feeds one received byte: edits the line, echoes it, and runs the command on Enter. */
void shell_input(uint8_t byte);

#endif
