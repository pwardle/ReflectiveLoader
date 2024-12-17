This is a simple example of reflective payload. 

Notes: 
1. Its Mimimum Deployment is set to macOS 11 to ensure its build without LC_DYLD_CHAINED_FIXUPS (which the loader doesn't support).
2. It contains a constructor `__attribute__((constructor))` that will be automatically executed when the payload is reflectively loaded.
