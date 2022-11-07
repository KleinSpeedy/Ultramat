#ifndef __HELPER_H__
#define __HELPER_H__

/* helper tools */

// check if Widget created successfully, exit if not
#define CHECK_WIDGET(x,name) \
    if(!x){ fprintf(stderr, "Couldn't create %s!\n", name); exit(1);}

// check if File could be opened, exit if not
#define CHECK_FILE(x, name) \
    if(!x){ fprintf(stderr, "Couldnt open File %s!\n",name); exit(1);}

// check if allocation failed
#define CHECK_ALLOC(x) \
    if(!x){ fprintf(stderr, "Allocation error\n"); exit(1);}

#endif //__HELPER_H__