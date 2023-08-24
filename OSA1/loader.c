#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
void loader_cleanup() {
    close(fd);
}

void load_and_run_elf(char** exe) {
    fd = open(exe[1], O_RDONLY);

    if (fd == -1) {
        perror("Error opening ELF file");
        exit(1);
    }


    ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    if (read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        perror("Error reading ELF header");
        exit(1);
    }


    printf("Entry Point Address: %x\n", ehdr->e_entry);


    for (int i = 0; i < ehdr->e_phnum; i++) {
        phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr));
        lseek(fd, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);
        if (read(fd, phdr, sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)) {
            perror("Error reading program header");
            exit(1);
        }

        if (phdr->p_type == PT_LOAD) {
            void *mem = mmap(0, phdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (mem == MAP_FAILED) {
                perror("Error mapping memory");
                exit(1);
            }

            lseek(fd, phdr->p_offset, SEEK_SET);
            if (read(fd, mem, phdr->p_filesz) != phdr->p_filesz) {
                perror("Error reading segment content");
                exit(1);
            }


            int offset = (int)mem + (ehdr->e_entry - phdr->p_vaddr) - (int)phdr->p_vaddr;
            printf("Calculated Offset: %x\n", offset);


            int (*entrypoint)(int) = (int (*)(int))(offset);
            int result = entrypoint(0);  


            printf("User _start return value = %d\n", result);

            
            munmap(mem, phdr->p_memsz);
            free(phdr);
            break;
        }

        free(phdr);
    }
}
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF Executable>\n", argv[0]);
        exit(1);
    }

    load_and_run_elf(argv);

    loader_cleanup();
    return 0;
}

