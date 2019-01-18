# Roveboard generics

 General list of functions that each board in the RoveBoard network support.
 * Each board may have more functions than this, but these are the standard across all of them for roveware usage.
 * Functions listed here are just the generic declarations and not for specific use outside of universal, multi-board roveware files such as RoveComm that just need to know that the functions exist. Never try to include these files in main.cpp, for instance.
 * The specific implemenations depend on which board you're going to use, and are contained in their own folders delegated to their own processor. Those are the versions to pay attention to for building main.cpp; be sure to read their headers for any hardware-specific notes.