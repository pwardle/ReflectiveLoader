This is a simple PoC that links against the reflective loader library (`libloader.a`) and reflectively loads payload from memory.

Notes: 
1. The library must be built with cmake 
2. The PoC expects a remote or local payload to execute (payload must not use LC_DYLD_CHAINED_FIXUPS).
