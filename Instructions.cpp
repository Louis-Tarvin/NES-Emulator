#include "Cpu.h"
#include <cstdint>
#include <iostream>

/// Helper function to update a status registers zero and negative flags based on the input value
uint8_t update_zero_and_negative_flags(uint8_t value, uint8_t status)
{
    uint8_t new_status;
    if (value == 0)
    {
        // set the zero flag and unset the negative flag
        new_status = (status | 0b00000010) & 0b01111111;
    }
    else if ((value >> 7) == 1)
    {
        // set the negative flag and unset the zero flag
        new_status = (status | 0b10000000) & 0b11111101;
    }
    else
    {
        // unset both flags
        new_status = status & 0b01111101;
    }

    return new_status;
}

// All instructions return true if they require an extra cycle (page boundary crossed)

/////////////////////////////////////////////////
/// Loading data from memory
/////////////////////////////////////////////////

/// Load Accumulator
bool Cpu::lda(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    a = fetched.value;
    status = update_zero_and_negative_flags(a, status);
    return fetched.page_boundary_crossed;
}

/// Load X register
bool Cpu::ldx(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    x = fetched.value;
    status = update_zero_and_negative_flags(x, status);
    return fetched.page_boundary_crossed;
}

/// Load Y register
bool Cpu::ldy(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    y = fetched.value;
    set_flag(Flag::Z, y == 0x00);
    set_flag(Flag::N, y & 0x80);
    // status = update_zero_and_negative_flags(y, status);
    return fetched.page_boundary_crossed;
}

/////////////////////////////////////////////////
/// Writing data to memory
/////////////////////////////////////////////////

/// Store Accumulator
bool Cpu::sta(AddressingMode am)
{
    write_to_next_address(a, am);
    return false;
}

/// Store X register
bool Cpu::stx(AddressingMode am)
{
    write_to_next_address(x, am);
    return false;
}

/// Store Y register
bool Cpu::sty(AddressingMode am)
{
    write_to_next_address(y, am);
    return false;
}

/////////////////////////////////////////////////
/// Transferring data between registers
/////////////////////////////////////////////////

/// Transfer Accumulator to X
bool Cpu::tax(AddressingMode am)
{
    x = a;
    status = update_zero_and_negative_flags(x, status);
    return false;
}

/// Transfer X to Accumulator
bool Cpu::txa(AddressingMode am)
{
    a = x;
    status = update_zero_and_negative_flags(a, status);
    return false;
}

/// Transfer Accumulator to Y
bool Cpu::tay(AddressingMode am)
{
    y = a;
    status = update_zero_and_negative_flags(y, status);
    return false;
}

/// Transfer Y to Accumulator
bool Cpu::tya(AddressingMode am)
{
    a = y;
    status = update_zero_and_negative_flags(a, status);
    return false;
}

/// Transfer Stack Pointer to X
bool Cpu::tsx(AddressingMode am)
{
    x = sp;
    status = update_zero_and_negative_flags(x, status);
    return false;
}

/// Transfer X to Stack Pointer
bool Cpu::txs(AddressingMode am)
{
    sp = x;
    return false;
}

/////////////////////////////////////////////////
/// Stack operations
/////////////////////////////////////////////////

/// Push Accumulator onto the stack
bool Cpu::pha(AddressingMode am)
{
    stack_push(a);
    return false;
}

/// Pull Accumulator off the stack
bool Cpu::pla(AddressingMode am)
{
    a = stack_pull();
    status = update_zero_and_negative_flags(a, status);
    return false;
}

/// Push Status register onto the stack
bool Cpu::php(AddressingMode am)
{
    set_flag(Flag::B, true);
    set_flag(Flag::U, true); // Turns out this flag is used apparently
    stack_push(status);
    set_flag(Flag::B, false);
    set_flag(Flag::U, false);
    return false;
}

/// Pull Status register off the stack
bool Cpu::plp(AddressingMode am)
{
    status = stack_pull();
    return false;
}

/////////////////////////////////////////////////
/// Arithmetic operations
/////////////////////////////////////////////////

/// Add with carry
bool Cpu::adc(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    uint16_t value = (uint16_t)a + (uint16_t)fetched.value;
    if (get_flag(Flag::C))
    {
        value++;
    }
    set_flag(Flag::C, value > 255);
    set_flag(Flag::Z, (value & 0x00FF) == 0);
    set_flag(Flag::V, ((value ^ a) & ~(fetched.value ^ a)) & 0x0080);
    set_flag(Flag::N, value & 0x80);
    a = value & 0x00FF;
    return fetched.page_boundary_crossed;
}

/// Subtract with carry
bool Cpu::sbc(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    uint16_t value = ((uint16_t)fetched.value ^ 0x00FF);
    uint16_t sum = value + (uint16_t)a;
    if (get_flag(Flag::C))
    {
        sum++;
    }
    set_flag(Flag::C, sum > 0xFF);
    set_flag(Flag::Z, (sum & 0x00FF) == 0);
    set_flag(Flag::V, (sum ^ (uint16_t)a) & (sum ^ value) & 0x0080);
    set_flag(Flag::N, sum & 0x80);
    a = sum & 0x00FF;
    return fetched.page_boundary_crossed;
}

/// Logical AND
bool Cpu::and_(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    a = a & fetched.value;
    status = update_zero_and_negative_flags(a, status);
    return fetched.page_boundary_crossed;
}

/// Inclusive OR
bool Cpu::ora(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    a = a | fetched.value;
    status = update_zero_and_negative_flags(a, status);
    return fetched.page_boundary_crossed;
}

/// Exclusive OR
bool Cpu::eor(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    a = a ^ fetched.value;
    status = update_zero_and_negative_flags(a, status);
    return fetched.page_boundary_crossed;
}

/// Arithmetic shift left
bool Cpu::asl(AddressingMode am)
{
    uint8_t value = get_address_value(am).value;
    uint16_t new_value = (uint16_t)value << 1;
    set_flag(Flag::C, (new_value & 0xFF00) > 0);
    set_flag(Flag::Z, (new_value & 0x00FF) == 0);
    set_flag(Flag::N, new_value & 0x80);
    if (am == AddressingMode::Absolute || am == AddressingMode::AbsoluteX)
    {
        pc -= 2;
    }
    else
    {
        pc -= 1;
    }
    write_to_next_address(new_value & 0x00FF, am);
    return false;
}

/// Logical shift right
bool Cpu::lsr(AddressingMode am)
{
    uint8_t value = get_address_value(am).value;
    set_flag(Flag::C, (value & 0x01));
    uint16_t new_value = (uint16_t)value >> 1;
    set_flag(Flag::Z, (new_value & 0x00FF) == 0);
    set_flag(Flag::N, new_value & 0x80);
    if (am == AddressingMode::Absolute || am == AddressingMode::AbsoluteX)
    {
        pc -= 2;
    }
    else
    {
        pc -= 1;
    }
    write_to_next_address(new_value & 0x00FF, am);
    return false;
}

/// Rotate left
bool Cpu::rol(AddressingMode am)
{
    uint8_t value = get_address_value(am).value;
    uint16_t new_value = (uint16_t)value << 1 | get_flag(Flag::C);
    set_flag(Flag::C, (new_value & 0xFF00) > 0);
    set_flag(Flag::Z, (new_value & 0x00FF) == 0);
    set_flag(Flag::N, new_value & 0x80);
    if (am == AddressingMode::Absolute || am == AddressingMode::AbsoluteX)
    {
        pc -= 2;
    }
    else
    {
        pc -= 1;
    }
    write_to_next_address(new_value & 0x00FF, am);
    return false;
}

/// Rotate right
bool Cpu::ror(AddressingMode am)
{
    uint8_t value = get_address_value(am).value;
    uint16_t new_value = ((uint16_t)value >> 1) | ((uint16_t)get_flag(Flag::C) << 7);
    set_flag(Flag::C, (value & 0x01));
    set_flag(Flag::Z, (new_value & 0x00FF) == 0);
    set_flag(Flag::N, new_value & 0x80);
    if (am == AddressingMode::Absolute || am == AddressingMode::AbsoluteX)
    {
        pc -= 2;
    }
    else
    {
        pc -= 1;
    }
    write_to_next_address(new_value & 0x00FF, am);
    return false;
}

/// Bit test
bool Cpu::bit(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    uint8_t value = a & fetched.value;
    set_flag(Flag::Z, value == 0);
    set_flag(Flag::V, (fetched.value & 0b01000000) == 0b01000000);
    set_flag(Flag::N, (fetched.value & 0b10000000) == 0b10000000);
    return false;
}

/// Compare accumulator
bool Cpu::cmp(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    uint16_t value = (uint16_t)a - (uint16_t)fetched.value;
    set_flag(Flag::C, a >= fetched.value);
    set_flag(Flag::Z, (value & 0x00FF) == 0);
    set_flag(Flag::N, (value & 0b10000000) == 0b10000000);
    return fetched.page_boundary_crossed;
}

/// Compare X register
bool Cpu::cpx(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    uint16_t value = (uint16_t)x - (uint16_t)fetched.value;
    set_flag(Flag::C, x >= fetched.value);
    set_flag(Flag::Z, (value & 0x00FF) == 0);
    set_flag(Flag::N, (value & 0b10000000) == 0b10000000);
    return false;
}

/// Compare Y register
bool Cpu::cpy(AddressingMode am)
{
    Fetched fetched = get_address_value(am);
    uint16_t value = (uint16_t)y - (uint16_t)fetched.value;
    set_flag(Flag::C, y >= fetched.value);
    set_flag(Flag::Z, (value & 0x00FF) == 0);
    set_flag(Flag::N, (value & 0b10000000) == 0b10000000);
    return false;
}

/// Increment memory
bool Cpu::inc(AddressingMode am)
{
    uint8_t value = get_address_value(am).value + 1;
    status = update_zero_and_negative_flags(value, status);
    if (am == AddressingMode::Absolute || am == AddressingMode::AbsoluteX)
    {
        pc -= 2;
    }
    else
    {
        pc -= 1;
    }
    write_to_next_address(value, am);
    return false;
}

/// Decrement memory
bool Cpu::dec(AddressingMode am)
{
    uint8_t value = get_address_value(am).value - 1;
    status = update_zero_and_negative_flags(value, status);
    if (am == AddressingMode::Absolute || am == AddressingMode::AbsoluteX)
    {
        pc -= 2;
    }
    else
    {
        pc -= 1;
    }
    write_to_next_address(value, am);
    return false;
}

/// Increment X register
bool Cpu::inx(AddressingMode am)
{
    x++;
    status = update_zero_and_negative_flags(x, status);
    return false;
}

/// Increment Y register
bool Cpu::iny(AddressingMode am)
{
    y++;
    status = update_zero_and_negative_flags(y, status);
    return false;
}

/// Decrement X register
bool Cpu::dex(AddressingMode am)
{
    x--;
    status = update_zero_and_negative_flags(x, status);
    return false;
}

/// Decrement Y register
bool Cpu::dey(AddressingMode am)
{
    y--;
    status = update_zero_and_negative_flags(y, status);
    return false;
}

/////////////////////////////////////////////////
/// Setting flags
/////////////////////////////////////////////////

/// Clear carry flag
bool Cpu::clc(AddressingMode am)
{
    set_flag(Flag::C, false);
    return false;
}

/// Clear decimal mode flag
bool Cpu::cld(AddressingMode am)
{
    set_flag(Flag::D, false);
    return false;
}

/// Clear interrupt disable flag
bool Cpu::cli(AddressingMode am)
{
    set_flag(Flag::I, false);
    return false;
}

/// Clear overflow flag
bool Cpu::clv(AddressingMode am)
{
    set_flag(Flag::V, false);
    return false;
}

/// Set carry flag
bool Cpu::sec(AddressingMode am)
{
    set_flag(Flag::C, true);
    return false;
}

/// Set decimal mode
bool Cpu::sed(AddressingMode am)
{
    set_flag(Flag::D, true);
    return false;
}

/// Set interrupt disable flag
bool Cpu::sei(AddressingMode am)
{
    set_flag(Flag::I, true);
    return false;
}

/////////////////////////////////////////////////
/// Control flow
/////////////////////////////////////////////////

/// Jump
bool Cpu::jmp(AddressingMode am)
{
    uint16_t addr;
    switch (am)
    {
    case AddressingMode::Indirect:
        addr = read_u16(pc);
        if ((addr & 0x00FF) == 0xFF)
        {
            // Simulating a hardware bug where if the target address is on a page boundary
            // the LSB will wrap. E.g. $01FF -> $0100
            pc = ((u_int16_t)read(addr & 0xFF00) << 8) | read(addr);
        }
        else
        {
            pc = ((u_int16_t)read(addr + 1) << 8) | read(addr);
        }
        break;
    case AddressingMode::Absolute:
        addr = read_u16(pc);
        pc = addr;
        break;
    default:
        std::cerr << "Invalid addressing mode for JMP instruction\n";
        exit(1);
    }
    return false;
}

/// Jump to subroutine
bool Cpu::jsr(AddressingMode am)
{
    u_int16_t addr = read_u16(pc);
    u_int16_t old_addr = pc + 1;
    pc = addr;
    stack_push(old_addr >> 8);
    stack_push(old_addr & 0x00FF);
    return false;
}

/// Return from subroutine
bool Cpu::rts(AddressingMode am)
{
    uint8_t lsb = stack_pull();
    uint8_t msb = stack_pull();
    pc = ((u_int16_t)msb << 8) | lsb;
    pc++;
    return false;
}

/// Force interrupt
bool Cpu::brk(AddressingMode am)
{
    pc++;
    stack_push(pc >> 8);
    stack_push(pc & 0x00FF);
    stack_push(status);
    pc = ((u_int16_t)read(0xFFFF) << 8) | read(0xFFFE);
    set_flag(Flag::B, true);
    return false;
}

/// Return from interrupt
bool Cpu::rti(AddressingMode am)
{
    status = stack_pull();
    uint8_t lsb = stack_pull();
    uint8_t msb = stack_pull();
    pc = ((u_int16_t)msb << 8) | lsb;
    // pc++;
    return false;
}

/// Branch if carry clear
bool Cpu::bcc(AddressingMode am)
{
    if (!get_flag(Flag::C))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/// Branch if carry set
bool Cpu::bcs(AddressingMode am)
{
    if (get_flag(Flag::C))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/// Branch if equal (zero flag set)
bool Cpu::beq(AddressingMode am)
{
    if (get_flag(Flag::Z))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/// Branch if not equal (zero flag clear)
bool Cpu::bne(AddressingMode am)
{
    if (!get_flag(Flag::Z))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/// Branch if minus (negative flag set)
bool Cpu::bmi(AddressingMode am)
{
    if (get_flag(Flag::N))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/// Branch if positive (negative flag clear)
bool Cpu::bpl(AddressingMode am)
{
    if (!get_flag(Flag::N))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/// Branch if overflow clear
bool Cpu::bvc(AddressingMode am)
{
    if (!get_flag(Flag::V))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/// Branch if overflow set
bool Cpu::bvs(AddressingMode am)
{
    if (get_flag(Flag::V))
    {
        cycles++;
        uint16_t offset = read(pc);
        pc++;
        if (offset & 0x80)
            offset = offset | 0xFF00;

        if (((pc + offset) & 0xFF00) != (pc & 0xFF00))
            cycles++;
        pc += offset;
    }
    else
    {
        pc++;
    }
    return false;
}

/////////////////////////////////////////////////
/// Misc
/////////////////////////////////////////////////

/// No-operation (does nothing)
bool Cpu::nop(AddressingMode am)
{
    return false;
}