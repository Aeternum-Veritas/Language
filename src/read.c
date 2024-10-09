#include "read.h"

// An undesirable return from any of the following indicates a fatal error and
// we terminate immediately

bool create_file_context(file_context *fc)
{
    // start with no children of course
    if (!is_file_valid(fc->entry.fname))
    {
        fprintf(stderr, "ERROR: The file '%s' is not a valid, supported file.\n", fc->entry.fname);
        return false;
    }
    fc->entry.f = fopen(fc->entry.fname, "r");
    if (!fc->entry.f)
    {
        pr_err(fc);
        return false;
    }
    if (gen_file_context_stream(fc) == false)
    {
        fclose(fc->entry.f);
        return false;
    }
    fclose(fc->entry.f);
    return true;
}

bool is_file_valid(char *fname)
{
    size_t len = strlen(fname);
    if (len < 2)
        return false;
    if (!(*(fname + len) == 'l' && (*fname + (len - 1)) == '.'))
        return false;
    return true;
}

void pr_err(file_context *fc)
{
    switch (errno)
    {
    case EISDIR:
        fprintf(stderr, "ERROR: The file '%s' is a directory.\n", fc->entry.fname);
        break;
    default:
        // most likely failed to open.
        fprintf(stderr, "ERROR: Failed to open file '%s'.\n", fc->entry.fname);
    }
}

bool gen_file_context_stream(file_context *fc)
{
    size_t flen = 0;
    fseek(fc->entry.f, SEEK_SET, SEEK_END);
    flen = ftell(fc->entry.f);
    rewind(fc->entry.f);
    fc->entry.stream = (char *)malloc(flen + 1);
    if (fc->entry.stream == NULL)
        return false;
    fread(fc->entry.stream, 1, flen, fc->entry.f); // should not fail
    fc->entry.stream_len = flen;
    fc->entry.stream[flen - 1] = 0; // terminate
    return true;
}

bool gen_file_context_tree(file_context *entry, uset *inc_list)
{
    // first set-up entry
    if (uset_contains(inc_list, entry->entry.fname))
        return true;
    // make the first pass and generate the tree
    if (!create_file_context(entry))
    {
        // failed to create the context
        // in this case, termination with error message is a must
        // but every such failure is an automatic "internal error"
        // we print it and exit
        return false; // immediate after-termination
    }
    // add to the uset
    if (!uset_insert(inc_list, entry->entry.fname))
        return false;
    register char *iter = entry->entry.stream;
    register size_t cstart = 0, lstart = 1, ostart = 0;
    while (*iter != 0)
    {
        if (*iter == '\n')
        {
            iter++;
            lstart++;
        }
        else if (*iter == '/' && *(iter + 1) == '/')
            while (*iter != '\n' && *iter != '\0')
                iter++;
        else if (*iter == '@')
        {
            size_t temp = lstart;
            while (*iter != '@' && *iter != 0)
            {
                if (*iter == '\n')
                    lstart++;
                iter++;
            }
            if (*iter == 0)
            {
                // where is the '@'
                fprintf(stderr, "ERROR: Multi-line comment was not terminated. Stray '@' in file '%s', line %lu.\n", entry->entry.fname, temp);
                return false;
            }
        }
        else if (*iter == '#')
        {
            size_t temp = lstart;
            while (*iter != '#' && *iter != 0)
            {
                if (*iter == '\n')
                    lstart++;
                iter++;
            }
            if (*iter == 0)
            {
                // where is the '#'?
                fprintf(stderr, "ERROR: Block was never ended. Stray '#' in file '%s', line %lu.\n", entry->entry.fname, temp);
                return false;
            }
        }
        else if (*iter == '$')
        {
            // this is where we deal with the include
            size_t temp = lstart;
            iter++;
            while (IsSpace(*iter) && *iter != 0)
            {
                if (*iter == '\n')
                    lstart++;
                iter++;
            }
            if (iter == 0 || !IsAlpha(*iter))
            {
                fprintf(stderr, "ERROR: Expected 'include', 'assembly' or '..(in future)..' after '$' in file '%s', line %lu.\n", entry->entry.fname, temp);
                return false;
            }
            slice s;
            s._s = iter;
            while (IsAlpha(*iter) && *iter != 0)
            {
                iter++;
            }
            s._e = iter;
            if (strncmp(s._s, "assembly", s._e - s._s) == 0)
            {
                // an assembly(ignore right now but we should remember this somehow)
                /// TODO: Finish me
                
            }
        }
    }
    return true;
}