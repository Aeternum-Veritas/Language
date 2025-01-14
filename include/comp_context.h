#ifndef _COMP_CONTEXT_
#define _COMP_CONTEXT_

// represents a single compilation context
#include "compiler.h"
#include "utils.h"
#include "defs.h"
#include "report.h"
#include "vec.h"
#include "umap.h"
#include "uset.h"
#include "stack.h"
#include "stream.h"
#include "slice.h"
#include "queue.h"
#include "file_context.h"
#include "enums.h"
#include "keys.h"
#include "global_hdlr.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct comp_context comp_context;

struct comp_context
{
    file_context *root;    // the root file context(input file)
    uset *_all_used_files; // key = file path

    // /**
    //  * While parsing, we parse one file at a time meaning even if we encounter an include statement, we won't go parse it first.
    //  * This allows for us to save some time in preparing the new context(we will create a new context but won't switch to it).
    //  * But this won't work while doing semantic analysis. Hence, we keep a stack of all file contexts for quick switching.
    //  */
    // stack *_compilation_stack;
};

static comp_context *cont = NULL;

comp_context *create_compilation_context();

bool add_base_file_context(file_context *base);

bool add_file_context(slice *file_path, file_context *parent);

bool file_already_exists(slice *file_path);

bool compile();

bool run_compiler();

void destroy_compilation_context();

void handle_invalid_state();

void handle_start_compilation_state();

void handle_parse_state();

#endif