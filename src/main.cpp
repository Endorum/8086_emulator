#include <stdio.h>
#include <fstream>

#include "BUS.hpp"
#include "CPU.hpp"
#include "RAM.hpp"
#include "ROM.hpp"
#include "tests/decoder.hpp"



void runInstr(const std::array<uint8_t, 16>& bytes) {
    Instruction instr = decode(bytes.data(), 0);
    std::string actual = strInstructionDebug(instr);
    std::cout << actual << std::endl;
}

void loadBinary(const std::string& filename, std::vector<u8>& data) {
    std::cout << "loading file '" << filename << "'...";

    std::ifstream f(filename, std::ios::binary | std::ios::ate);

    if (!f) {
        throw std::runtime_error( "Could not open file '" + filename + "'!" );
    }

    data.resize( static_cast<size_t>(f.tellg()) );

    f.seekg(0);
    f.read( reinterpret_cast<char*>(data.data()), data.size() );

    std::cout << "Done!" << std::endl;
}

int main() {

    // decoder_run_tests();
    CPU cpu;
    BUS bus;
    RAM ram;
    ROM rom;

    bus.add_device(&ram);
    bus.add_device(&rom);
    cpu.attach_bus(&bus);

    std::vector<u8> data;
    loadBinary("rom.bin", data);
    for ( int i = 0; i < data.size(); i++ ) {
        rom.store(i, data.at(i));
    }


    cpu.reset();

    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    

    return 0;
}

