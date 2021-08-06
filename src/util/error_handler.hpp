#ifndef ERROR_HDL
#define ERROR_HDL

/*
 * Error message definitions
 */
#define BAD_FILENAME "[!] Error parsing file name!\n\
[!] Supported file format: umps[digits].txt\n"
#define BAD_NUM_ARGS "[!] Usage: ./<program-name> <path to instance data> <q1> <q2> <max iter steps>\n"


/*
 * Function definitions
 */
void fatal_error(std::string);
void liberror();

#endif
