#include "handler.h"

#include <stdlib.h>
#include <argp.h>

const char *argp_program_version = "sbp 0.1";
const char *argp_program_bug_address = "<sterz@mathematik.uni-marburg.de>";

static char doc[] = "SBP -- Userland part of the Seminar Broadcast Protocol";

static char args_doc[] = "INTERFACE";

extern struct neighbors nbs;

static struct argp_option options[] = {
    {"value", 'v', "val", 0, "Value to be sent", 0 },
    { 0 }
};

struct arguments {
    uint32_t value;
    char *interface;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'v':
            printf("Before %s\n", arg);
            arguments->value = (uint32_t) strtol(arg, (char **)NULL, 10);
            printf("After\n");
            break;

        case ARGP_KEY_ARG:
            if (state->arg_num >= 1) argp_usage (state);

            arguments->interface = arg;

            break;

        case ARGP_KEY_END:
            if (state->arg_num < 1) argp_usage (state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };


int main(int argc, char **argv) {
    struct arguments arguments = {
        .value = 0,
    };

    argp_parse(&argp, argc, argv, 0, 0, &arguments);


    if (handle(arguments.interface, arguments.value) != 0) {
        return -1;
    }

    return 0;
}
