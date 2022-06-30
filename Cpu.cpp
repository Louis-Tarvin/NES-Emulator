#include "Bus.h"
#include <iostream>
#include <sstream>
#include <bitset>

Cpu::Cpu()
{
    // Initialise giant lookup table mapping opcodes to instructions
    lookup = {
        {0x00, {&Cpu::brk, AddressingMode::Implied, 7}},
        {0x01, {&Cpu::ora, AddressingMode::IndirectX, 6}},
        {0x05, {&Cpu::ora, AddressingMode::ZeroPage, 3}},
        {0x06, {&Cpu::asl, AddressingMode::ZeroPage, 5}},
        {0x08, {&Cpu::php, AddressingMode::Implied, 3}},
        {0x09, {&Cpu::ora, AddressingMode::Immediate, 2}},
        {0x0A, {&Cpu::asl, AddressingMode::Accumulator, 2}},
        {0x0D, {&Cpu::ora, AddressingMode::Absolute, 4}},
        {0x0E, {&Cpu::asl, AddressingMode::Absolute, 6}},
        {0x10, {&Cpu::bpl, AddressingMode::Implied, 2}},
        {0x11, {&Cpu::ora, AddressingMode::IndirectY, 5}},
        {0x15, {&Cpu::ora, AddressingMode::ZeroPageX, 4}},
        {0x16, {&Cpu::asl, AddressingMode::ZeroPageX, 6}},
        {0x18, {&Cpu::clc, AddressingMode::Implied, 2}},
        {0x19, {&Cpu::ora, AddressingMode::AbsoluteY, 4}},
        {0x1D, {&Cpu::ora, AddressingMode::AbsoluteX, 4}},
        {0x1E, {&Cpu::asl, AddressingMode::AbsoluteX, 7}},
        {0x20, {&Cpu::jsr, AddressingMode::Implied, 6}},
        {0x21, {&Cpu::and_, AddressingMode::IndirectX, 6}},
        {0x24, {&Cpu::bit, AddressingMode::ZeroPage, 3}},
        {0x25, {&Cpu::and_, AddressingMode::ZeroPage, 3}},
        {0x26, {&Cpu::rol, AddressingMode::ZeroPage, 5}},
        {0x28, {&Cpu::plp, AddressingMode::Implied, 4}},
        {0x29, {&Cpu::and_, AddressingMode::Immediate, 2}},
        {0x2A, {&Cpu::rol, AddressingMode::Accumulator, 2}},
        {0x2C, {&Cpu::bit, AddressingMode::Absolute, 4}},
        {0x2D, {&Cpu::and_, AddressingMode::Absolute, 4}},
        {0x2E, {&Cpu::rol, AddressingMode::Absolute, 6}},
        {0x30, {&Cpu::bmi, AddressingMode::Implied, 2}},
        {0x31, {&Cpu::and_, AddressingMode::IndirectY, 5}},
        {0x35, {&Cpu::and_, AddressingMode::ZeroPageX, 4}},
        {0x36, {&Cpu::rol, AddressingMode::ZeroPageX, 6}},
        {0x38, {&Cpu::sec, AddressingMode::Implied, 2}},
        {0x39, {&Cpu::and_, AddressingMode::AbsoluteY, 4}},
        {0x3D, {&Cpu::and_, AddressingMode::AbsoluteX, 4}},
        {0x3E, {&Cpu::rol, AddressingMode::AbsoluteX, 7}},
        {0x40, {&Cpu::rti, AddressingMode::Implied, 6}},
        {0x41, {&Cpu::eor, AddressingMode::IndirectX, 6}},
        {0x45, {&Cpu::eor, AddressingMode::ZeroPage, 3}},
        {0x46, {&Cpu::lsr, AddressingMode::ZeroPage, 5}},
        {0x48, {&Cpu::pha, AddressingMode::Implied, 3}},
        {0x49, {&Cpu::eor, AddressingMode::Immediate, 2}},
        {0x4A, {&Cpu::lsr, AddressingMode::Accumulator, 2}},
        {0x4C, {&Cpu::jmp, AddressingMode::Absolute, 3}},
        {0x4D, {&Cpu::eor, AddressingMode::Absolute, 4}},
        {0x4E, {&Cpu::lsr, AddressingMode::Absolute, 6}},
        {0x50, {&Cpu::bvc, AddressingMode::Implied, 2}},
        {0x51, {&Cpu::eor, AddressingMode::IndirectY, 5}},
        {0x55, {&Cpu::eor, AddressingMode::ZeroPageX, 4}},
        {0x56, {&Cpu::lsr, AddressingMode::ZeroPageX, 6}},
        {0x58, {&Cpu::cli, AddressingMode::Implied, 2}},
        {0x59, {&Cpu::eor, AddressingMode::AbsoluteY, 4}},
        {0x5D, {&Cpu::eor, AddressingMode::AbsoluteX, 4}},
        {0x5E, {&Cpu::lsr, AddressingMode::AbsoluteX, 7}},
        {0x60, {&Cpu::rts, AddressingMode::Implied, 6}},
        {0x61, {&Cpu::adc, AddressingMode::IndirectX, 6}},
        {0x65, {&Cpu::adc, AddressingMode::ZeroPage, 3}},
        {0x66, {&Cpu::ror, AddressingMode::ZeroPage, 5}},
        {0x68, {&Cpu::pla, AddressingMode::Implied, 4}},
        {0x69, {&Cpu::adc, AddressingMode::Immediate, 2}},
        {0x6A, {&Cpu::ror, AddressingMode::Accumulator, 2}},
        {0x6C, {&Cpu::jmp, AddressingMode::Indirect, 5}},
        {0x6D, {&Cpu::adc, AddressingMode::Absolute, 4}},
        {0x6E, {&Cpu::ror, AddressingMode::Absolute, 6}},
        {0x70, {&Cpu::bvs, AddressingMode::Implied, 2}},
        {0x71, {&Cpu::adc, AddressingMode::IndirectY, 5}},
        {0x75, {&Cpu::adc, AddressingMode::ZeroPageX, 4}},
        {0x76, {&Cpu::ror, AddressingMode::ZeroPageX, 6}},
        {0x78, {&Cpu::sei, AddressingMode::Implied, 2}},
        {0x79, {&Cpu::adc, AddressingMode::AbsoluteY, 4}},
        {0x7D, {&Cpu::adc, AddressingMode::AbsoluteX, 4}},
        {0x7E, {&Cpu::ror, AddressingMode::AbsoluteX, 7}},
        {0x81, {&Cpu::sta, AddressingMode::IndirectX, 6}},
        {0x84, {&Cpu::sty, AddressingMode::ZeroPage, 3}},
        {0x85, {&Cpu::sta, AddressingMode::ZeroPage, 3}},
        {0x86, {&Cpu::stx, AddressingMode::ZeroPage, 3}},
        {0x88, {&Cpu::dey, AddressingMode::Implied, 2}},
        {0x8A, {&Cpu::txa, AddressingMode::Implied, 2}},
        {0x8C, {&Cpu::sty, AddressingMode::Absolute, 4}},
        {0x8D, {&Cpu::sta, AddressingMode::Absolute, 4}},
        {0x8E, {&Cpu::stx, AddressingMode::Absolute, 4}},
        {0x90, {&Cpu::bcc, AddressingMode::Implied, 2}},
        {0x91, {&Cpu::sta, AddressingMode::IndirectY, 6}},
        {0x94, {&Cpu::sty, AddressingMode::ZeroPageX, 4}},
        {0x95, {&Cpu::sta, AddressingMode::ZeroPageX, 4}},
        {0x96, {&Cpu::stx, AddressingMode::ZeroPageY, 4}},
        {0x98, {&Cpu::tya, AddressingMode::Implied, 2}},
        {0x99, {&Cpu::sta, AddressingMode::AbsoluteY, 5}},
        {0x9A, {&Cpu::txs, AddressingMode::Implied, 2}},
        {0x9D, {&Cpu::sta, AddressingMode::AbsoluteX, 5}},
        {0xA0, {&Cpu::ldy, AddressingMode::Immediate, 2}},
        {0xA1, {&Cpu::lda, AddressingMode::IndirectX, 6}},
        {0xA2, {&Cpu::ldx, AddressingMode::Immediate, 2}},
        {0xA4, {&Cpu::ldy, AddressingMode::ZeroPage, 3}},
        {0xA5, {&Cpu::lda, AddressingMode::ZeroPage, 3}},
        {0xA6, {&Cpu::ldx, AddressingMode::ZeroPage, 3}},
        {0xA8, {&Cpu::tay, AddressingMode::Implied, 2}},
        {0xA9, {&Cpu::lda, AddressingMode::Immediate, 2}},
        {0xAA, {&Cpu::tax, AddressingMode::Implied, 2}},
        {0xAC, {&Cpu::ldy, AddressingMode::Absolute, 4}},
        {0xAD, {&Cpu::lda, AddressingMode::Absolute, 4}},
        {0xAE, {&Cpu::ldx, AddressingMode::Absolute, 4}},
        {0xB0, {&Cpu::bcs, AddressingMode::Implied, 2}},
        {0xB1, {&Cpu::lda, AddressingMode::IndirectY, 5}},
        {0xB4, {&Cpu::ldy, AddressingMode::ZeroPageX, 4}},
        {0xB5, {&Cpu::lda, AddressingMode::ZeroPageX, 4}},
        {0xB6, {&Cpu::ldx, AddressingMode::ZeroPageY, 4}},
        {0xB8, {&Cpu::clv, AddressingMode::Implied, 2}},
        {0xB9, {&Cpu::lda, AddressingMode::AbsoluteY, 4}},
        {0xBA, {&Cpu::tsx, AddressingMode::Implied, 2}},
        {0xBC, {&Cpu::ldy, AddressingMode::AbsoluteX, 4}},
        {0xBD, {&Cpu::lda, AddressingMode::AbsoluteX, 4}},
        {0xBE, {&Cpu::ldx, AddressingMode::AbsoluteY, 4}},
        {0xC0, {&Cpu::cpy, AddressingMode::Immediate, 2}},
        {0xC1, {&Cpu::cmp, AddressingMode::IndirectX, 6}},
        {0xC4, {&Cpu::cpy, AddressingMode::ZeroPage, 3}},
        {0xC5, {&Cpu::cmp, AddressingMode::ZeroPage, 3}},
        {0xC6, {&Cpu::dec, AddressingMode::ZeroPage, 5}},
        {0xC8, {&Cpu::iny, AddressingMode::Implied, 2}},
        {0xC9, {&Cpu::cmp, AddressingMode::Immediate, 2}},
        {0xCA, {&Cpu::dex, AddressingMode::Implied, 2}},
        {0xCC, {&Cpu::cpy, AddressingMode::Absolute, 4}},
        {0xCD, {&Cpu::cmp, AddressingMode::Absolute, 4}},
        {0xCE, {&Cpu::dec, AddressingMode::Absolute, 6}},
        {0xD0, {&Cpu::bne, AddressingMode::Implied, 2}},
        {0xD1, {&Cpu::cmp, AddressingMode::IndirectY, 5}},
        {0xD5, {&Cpu::cmp, AddressingMode::ZeroPageX, 4}},
        {0xD6, {&Cpu::dec, AddressingMode::ZeroPageX, 6}},
        {0xD8, {&Cpu::cld, AddressingMode::Implied, 2}},
        {0xD9, {&Cpu::cmp, AddressingMode::AbsoluteY, 4}},
        {0xDD, {&Cpu::cmp, AddressingMode::AbsoluteX, 4}},
        {0xDE, {&Cpu::dec, AddressingMode::AbsoluteX, 7}},
        {0xE0, {&Cpu::cpx, AddressingMode::Immediate, 2}},
        {0xE1, {&Cpu::sbc, AddressingMode::IndirectX, 6}},
        {0xE4, {&Cpu::cpx, AddressingMode::ZeroPage, 3}},
        {0xE5, {&Cpu::sbc, AddressingMode::ZeroPage, 3}},
        {0xE6, {&Cpu::inc, AddressingMode::ZeroPage, 5}},
        {0xE8, {&Cpu::inx, AddressingMode::Implied, 2}},
        {0xE9, {&Cpu::sbc, AddressingMode::Immediate, 2}},
        {0xEA, {&Cpu::nop, AddressingMode::Implied, 2}},
        {0xEC, {&Cpu::cpx, AddressingMode::Absolute, 4}},
        {0xED, {&Cpu::sbc, AddressingMode::Absolute, 4}},
        {0xEE, {&Cpu::inc, AddressingMode::Absolute, 6}},
        {0xF0, {&Cpu::beq, AddressingMode::Implied, 2}},
        {0xF1, {&Cpu::sbc, AddressingMode::IndirectY, 5}},
        {0xF5, {&Cpu::sbc, AddressingMode::ZeroPageX, 4}},
        {0xF6, {&Cpu::inc, AddressingMode::ZeroPageX, 6}},
        {0xF8, {&Cpu::sed, AddressingMode::Implied, 2}},
        {0xF9, {&Cpu::sbc, AddressingMode::AbsoluteY, 4}},
        {0xFD, {&Cpu::sbc, AddressingMode::AbsoluteX, 4}},
        {0xFE, {&Cpu::inc, AddressingMode::AbsoluteX, 7}},
    };
}

Cpu::~Cpu() {}

void Cpu::connect_bus(Bus &bus_ptr)
{
    bus = &bus_ptr;
}

/// Read a byte from memory
uint8_t Cpu::read(uint16_t addr)
{
    return bus->read(addr);
}

/// Read two bytes from memory at address and address + 1
uint16_t Cpu::read_u16(uint16_t addr)
{
    // need to convert from Little-Endian to Big-Endian
    uint8_t byte_a = read(addr);
    uint8_t byte_b = read(addr + 1);
    return ((uint16_t)byte_b) << 8 | (uint16_t)byte_a;
}

/// Write a byte to memory
void Cpu::write(uint16_t addr, uint8_t data)
{
    bus->write(addr, data);
}

/// Get a value from memory using the program counter given a specific addressing mode
Cpu::Fetched Cpu::get_address_value(AddressingMode am)
{
    uint8_t value;
    bool page_boundary_crossed = false;

    if (am == AddressingMode::Immediate)
    {
        value = read(pc);
        pc++;
    }
    else if (am == AddressingMode::ZeroPage)
    {
        uint8_t addr = read(pc);
        pc++;
        value = read(addr);
    }
    else if (am == AddressingMode::ZeroPageX)
    {
        uint8_t addr = read(pc);
        pc++;
        value = read(addr + x);
    }
    else if (am == AddressingMode::ZeroPageY)
    {
        uint8_t addr = read(pc);
        pc++;
        value = read(addr + y);
    }
    else if (am == AddressingMode::Absolute)
    {
        uint16_t addr = read_u16(pc);
        pc += 2;
        value = read(addr);
    }
    else if (am == AddressingMode::AbsoluteX)
    {
        uint16_t addr = read_u16(pc);
        pc += 2;
        value = read(addr + x);
        page_boundary_crossed = ((addr + x) & 0xFF00) != (addr & 0xFF00);
    }
    else if (am == AddressingMode::AbsoluteY)
    {
        uint16_t addr = read_u16(pc);
        pc += 2;
        value = read(addr + y);
        page_boundary_crossed = ((addr + y) & 0xFF00) != (addr & 0xFF00);
    }
    else if (am == AddressingMode::IndirectX)
    {
        uint8_t addr = read(pc);
        uint16_t addr2 = read_u16(addr + x);
        pc++;
        value = read(addr2);
    }
    else if (am == AddressingMode::IndirectY)
    {
        uint8_t addr = read(pc);
        uint16_t addr2 = read_u16(addr);
        pc++;
        value = read(addr2 + y);
        page_boundary_crossed = ((addr2 + y) & 0xFF00) != (addr2 & 0xFF00);
    }
    else if (am == AddressingMode::Accumulator)
    {
        value = a;
    }
    else
    {
        std::cerr << "Invalid addressing mode\n";
        exit(1);
    }

    return {value, page_boundary_crossed};
}

/// Write data to memory using the program counter given a specific addressing mode
void Cpu::write_to_next_address(uint8_t data, AddressingMode am)
{
    if (am == AddressingMode::Immediate)
    {
        write(pc, data);
        pc++;
    }
    else if (am == AddressingMode::ZeroPage)
    {
        uint8_t addr = read(pc);
        pc++;
        write(addr, data);
    }
    else if (am == AddressingMode::ZeroPageX)
    {
        uint8_t addr = read(pc);
        pc++;
        write(addr + x, data);
    }
    else if (am == AddressingMode::ZeroPageY)
    {
        uint8_t addr = read(pc);
        pc++;
        write(addr + y, data);
    }
    else if (am == AddressingMode::Absolute)
    {
        uint16_t addr = read_u16(pc);
        pc += 2;
        write(addr, data);
    }
    else if (am == AddressingMode::AbsoluteX)
    {
        uint16_t addr = read_u16(pc);
        pc += 2;
        write(addr + x, data);
    }
    else if (am == AddressingMode::AbsoluteY)
    {
        uint16_t addr = read_u16(pc);
        pc += 2;
        write(addr + y, data);
    }
    else if (am == AddressingMode::IndirectX)
    {
        uint8_t addr = read(pc);
        uint16_t addr2 = read_u16(addr + x);
        pc++;
        write(addr2, data);
    }
    else if (am == AddressingMode::IndirectY)
    {
        uint8_t addr = read(pc);
        uint16_t addr2 = read_u16(addr + y);
        pc++;
        write(addr2, data);
    }
    else if (am == AddressingMode::Accumulator)
    {
        pc++;
        a = data;
    }
    else
    {
        std::cerr << "Invalid addressing mode\n";
        exit(1);
    }
}

/// Push data onto the stack
void Cpu::stack_push(uint8_t data)
{
    write(0x0100 + sp, data);
    sp--;
}

/// Pull data from the stack
uint8_t Cpu::stack_pull()
{
    sp++;
    return read(0x0100 + sp);
}

void Cpu::clock()
{
    if (cycles == 0)
    {
        uint8_t opcode = read(pc);
        pc++;

        Instruction current_instruction = lookup[opcode];
        std::cout << "opcode: " << std::hex << +opcode << "\n";
        cycles = current_instruction.cycles;
        // std::cout << "new cycles: " << +cycles << "\n";

        bool extra_cycle = (this->*current_instruction.function)(current_instruction.addressing_mode);

        if (extra_cycle)
            cycles++;
    }
    cycles--;
}

/// Reset the state of the CPU
void Cpu::reset()
{
    a = 0;
    x = 0;
    y = 0;
    sp = 0xFD;
    status = 0;
    pc = read_u16(0xFFFC);

    cycles = 8;
}

/// Handle interrupt
void Cpu::irq()
{
    if (!get_flag(Flag::I))
    {
        stack_push(pc >> 8);
        stack_push(pc & 0x00FF);
        set_flag(Flag::B, false);
        set_flag(Flag::I, true);
        stack_push(status);
        pc = read_u16(0xFFFE);

        cycles = 7;
    }
}

/// Handle non-maskable interrupt (can't be ignored)
void Cpu::nmi()
{
    stack_push(pc >> 8);
    stack_push(pc & 0x00FF);
    set_flag(Flag::B, false);
    set_flag(Flag::I, true);
    stack_push(status);
    pc = read_u16(0xFFFA);

    cycles = 8;
}

bool Cpu::get_flag(Flag f)
{
    return status & f;
}

void Cpu::set_flag(Flag f, bool on)
{
    if (on)
    {
        status |= f;
    }
    else
    {
        status &= ~f;
    }
}

/// Return a string representation of the CPU for debugging
std::string Cpu::display()
{
    std::stringstream output;
    output << "\n         NV-BDIZC";
    output << "\n status: " << std::bitset<8>(status);
    output << "\n pc: " << std::hex << +pc;
    output << "\n a: " << std::hex << +a;
    output << " x: " << std::hex << +x;
    output << " y: " << std::hex << +y;
    output << "\n sp: " << std::hex << +sp;
    output << "\n";
    return output.str();
}
