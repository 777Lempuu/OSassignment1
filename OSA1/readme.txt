1.Done without bonus
2.The compilation was fine 
3.Error occured where segmentation fault is the error
4.tried changing loader.c with some mmap commands
5.error of mmap showed up
6.tried fixing and found out the _start was the problem and so added print statements to confirm that the right entry address and offset was being chosen
7.Still segmentation fault but this time there is proof that the right address is being reached.
Now for the loader.c 
1.The program starts by opening the ELF executable specified as a command line argument.
"fd = open(exe[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening ELF file");
        exit(1);"

2.It reads the ELF header to determine the entry point address of the executable.
" ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));"

3.It iterates through the program headers of the ELF file, looking for loadable segments.
"for (int i = 0; i < ehdr->e_phnum; i++) {
        phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr));
        lseek(fd, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);"

4.If it finds a loadable segment, it maps memory, reads the segment content from      the file into memory, and calculates an offset to the entry point within the memory-mapped segment.
" if (phdr->p_type == PT_LOAD) {
            void *mem = mmap(0, phdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
           "

5.It casts this offset to a function pointer and executes the entry point function.
"           int offset = (int)mem + (ehdr->e_entry - phdr->p_vaddr) - (int)phdr->p_vaddr;
            printf("Calculated Offset: %x\n", offset);
            int (*entrypoint)(int) = (int (*)(int))(offset);
            int result = entrypoint(0);
            printf("User _start return value = %d\n", result);"
in code of loader.c error checks have been implemented 
GITHUB:"https://github.com/777Lempuu/OSA-1-/tree/main/OSA1"


