#include <stdio.h>
#include <signal.h>

#include "signal_handler.h"

void handle_signal(int signal_number)
{
    if (signal_number == SIGTERM)
    {
        printf("\n[Signal] SIGTERM received.\n");
        printf("[Signal] Process will terminate safely.\n");
    }
}

void setup_signals(void)
{
    signal(SIGTERM, handle_signal);
}
