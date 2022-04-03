#include "third_party/elfio/include/elfio/elfio.hpp"
#include "third_party/elfio/include/elfio/elfio_dump.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <errno.h>
#include <iostream>
#include <ostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: ./<exec> <elf_file>" << std::endl;
        exit(0);
    }

    // Create elfio reader
    ELFIO::elfio reader;

    // Load ELF data
    if (!reader.load(argv[1])) {
        perror("Can't find or process ELF file");
        exit(-1);
    }

    // check ELF propeties
    const auto machine = reader.get_machine();
    if (machine != ELFIO::EM_RISCV) {
        std::cerr << "Wrong Elf Machine:\t";
        std::cerr << machine << "\n";
        exit(-1);
    }

    const auto elfClass = reader.get_class();
    if (elfClass != ELFIO::ELFCLASS32) {
        std::cerr << "Wrong Elf Class:\t";
        std::cerr << elfClass << "\n";
        exit(-1);
    }

    const auto entry = reader.get_entry();
    std::cout << std::hex << entry << std::dec << std::endl;

    // Print ELF file sections info
    ELFIO::Elf_Half sec_num = reader.sections.size();
    for (auto i = 0; i < sec_num; ++i) {
        const ELFIO::section *psec = reader.sections[i];

        if (psec->get_name() == ".text") {
            const auto align = psec->get_addr_align();
            const auto secAddr = psec->get_address();

            // Access section's data
            const char *pdata = reader.sections[i]->get_data();
            assert(pdata != nullptr);

            for (ELFIO::Elf_Xword byte = 0;
                 byte < std::min(psec->get_size(), ELFIO::MAX_DATA_ENTRIES);
                 ++byte) {
                if (byte % align == 0) {
                    printf("addr: %016lx\t", byte);
                    ELFIO::Elf_Word instr =
                        *((ELFIO::Elf_Word *)pdata + (byte / 4));
                    printf("%08x\n", instr);
                }
            }
        }
    }

    // Print ELF file segments info
    ELFIO::Elf_Half seg_num = reader.segments.size();
    std::cout << "Number of segments: " << seg_num << std::endl;
    for (int i = 0; i < seg_num; ++i) {
        const ELFIO::segment *pseg = reader.segments[i];

        std::cout << "#\tflags\tvirt\tsize\tmemsize" << std::endl;

        std::cout << " [" << i << "] 0x" << std::hex << pseg->get_flags()
                  << "\t0x" << pseg->get_virtual_address() << "\t0x"
                  << pseg->get_file_size() << "\t0x" << pseg->get_memory_size()
                  << std::endl;
        // Access segments's data
        std::cout << "alignn: " << pseg->get_align() << std::endl;

        const char *pdata = reader.segments[i]->get_data();

        for (ELFIO::Elf_Xword byte = 0;
             byte < std::max(pseg->get_file_size(), ELFIO::MAX_DATA_ENTRIES);
             ++byte) {
            if (byte % 4 == 0) {
                printf("addr: %016lx\t", byte);
                ELFIO::Elf_Word instr =
                    *((ELFIO::Elf_Word *)pdata + (byte / 4));
                printf("%08x\n", instr);
            }
        }
    }
}