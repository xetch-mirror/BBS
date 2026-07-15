// stubs.c
// Look at the compiler errors to see what names the headers are looking for
int _isctype(int c, int type) {
    // implement the bare-minimum logic your header expects
    return (c >= '0' && c <= '9'); 
}

void __main() {
    // MinGW/Win32 targets often look for an internal __main setup routine.
    // leave this empty, DO NOT DO ANYTHING
}
