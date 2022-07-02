#include <map>
#include <string>

class Bus;

enum class AddressingMode
{
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndirectX,
    IndirectY,
    Accumulator,
    Implied,
};

class Cpu
{
private:
    Bus *bus;
    //                 NV-BDIZC
    uint8_t status = 0b00110100; // status register
    uint8_t a = 0;               // Accumulator register
    uint8_t x = 0;               // X register
    uint8_t y = 0;               // Y register
    uint8_t sp = 0xFD;           // Stack Pointer
    uint16_t pc = 0;             // Program Counter

    uint8_t cycles = 0; // Used to control how long an instruction takes

    struct Fetched
    {
        uint8_t value;
        bool page_boundary_crossed;
    };

    uint8_t read(uint16_t addr);
    uint16_t read_u16(uint16_t addr);
    uint16_t read_u16_wrapped(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    Fetched get_address_value(AddressingMode am);
    void write_to_next_address(uint8_t data, AddressingMode am);
    void stack_push(uint8_t data);
    uint8_t stack_pull();

public:
    Cpu();
    ~Cpu();

    std::string display();

    void connect_bus(Bus &bus);
    void clock();
    void reset();
    void irq();
    void nmi();

    enum Flag
    {
        N = (1 << 7), // Negative
        V = (1 << 6), // Overflow
        U = (1 << 5), // Unused
        B = (1 << 4), // Break
        D = (1 << 3), // Decimal Mode
        I = (1 << 2), // Disable Interrupts
        Z = (1 << 1), // Zero
        C = (1 << 0), // Carry Bit
    };

    typedef bool (Cpu::*instruction_fn)(AddressingMode);

    struct Instruction
    {
        instruction_fn function;
        AddressingMode addressing_mode;
        uint8_t cycles;
    };

private:
    bool get_flag(Flag f);
    void set_flag(Flag f, bool on);

    // Instructions

    bool lda(AddressingMode am);
    bool ldx(AddressingMode am);
    bool ldy(AddressingMode am);
    bool sta(AddressingMode am);
    bool stx(AddressingMode am);
    bool sty(AddressingMode am);
    bool tax(AddressingMode am);
    bool txa(AddressingMode am);
    bool tay(AddressingMode am);
    bool tya(AddressingMode am);
    bool tsx(AddressingMode am);
    bool txs(AddressingMode am);
    bool pha(AddressingMode am);
    bool pla(AddressingMode am);
    bool php(AddressingMode am);
    bool plp(AddressingMode am);
    bool adc(AddressingMode am);
    bool sbc(AddressingMode am);
    bool and_(AddressingMode am);
    bool ora(AddressingMode am);
    bool eor(AddressingMode am);
    bool asl(AddressingMode am);
    bool lsr(AddressingMode am);
    bool rol(AddressingMode am);
    bool ror(AddressingMode am);
    bool bit(AddressingMode am);
    bool cmp(AddressingMode am);
    bool cpx(AddressingMode am);
    bool cpy(AddressingMode am);
    bool dec(AddressingMode am);
    bool inc(AddressingMode am);
    bool inx(AddressingMode am);
    bool iny(AddressingMode am);
    bool dex(AddressingMode am);
    bool dey(AddressingMode am);
    bool clc(AddressingMode am);
    bool cld(AddressingMode am);
    bool cli(AddressingMode am);
    bool clv(AddressingMode am);
    bool sec(AddressingMode am);
    bool sed(AddressingMode am);
    bool sei(AddressingMode am);
    bool jmp(AddressingMode am);
    bool jsr(AddressingMode am);
    bool rts(AddressingMode am);
    bool brk(AddressingMode am);
    bool rti(AddressingMode am);
    bool bcc(AddressingMode am);
    bool bcs(AddressingMode am);
    bool beq(AddressingMode am);
    bool bne(AddressingMode am);
    bool bmi(AddressingMode am);
    bool bpl(AddressingMode am);
    bool bvc(AddressingMode am);
    bool bvs(AddressingMode am);
    bool nop(AddressingMode am);

    std::map<uint8_t, Instruction> lookup;
};
