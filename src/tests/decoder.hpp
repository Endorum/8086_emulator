#include <iostream>



#include "DECODE.hpp"
#include "toString.hpp"

static int counter = 0;

inline static int runSingle(const std::array<uint8_t, 16>& bytes, const std::string& exp) {



    Instruction instr = decode(bytes.data(), 0);
    std::string actual = strInstruction(instr);
    // std::cout << std::format("{:<35}{}", exp, actual) << std::endl;

    if (exp != actual) {
        std::cout << "discrepancy: " << exp << " != " << actual << std::endl;
        counter++;
    }

    return 0;
}


inline int decoder_run_tests() {
    // RegRm: reg/reg, D, W
    runSingle({0x89, 0xD9}, "mov cx, bx");
    runSingle({0x88, 0xE5}, "mov ch, ah");
    runSingle({0x00, 0xD8}, "add al, bl");
    runSingle({0x31, 0xF6}, "xor si, si");
    runSingle({0x8B, 0x10}, "mov dx, [bx + si]");
    runSingle({0x89, 0x10}, "mov [bx + si], dx");
    // RegRm: all r/m bases, mod=00
    runSingle({0x8A, 0x01}, "mov al, [bx + di]");
    runSingle({0x88, 0x0A}, "mov [bp + si], cl");
    runSingle({0x8B, 0x1B}, "mov bx, [bp + di]");
    runSingle({0x8B, 0x04}, "mov ax, [si]");
    runSingle({0x89, 0x1D}, "mov [di], bx");
    runSingle({0x29, 0x0F}, "sub [bx], cx");
    runSingle( {0x8B, 0x16, 0x34, 0x12}, "mov dx, [0x1234]" );
    // mod=01 / mod=10
    runSingle({0x8B, 0x56, 0x00}, "mov dx, [bp]");
    runSingle( {0x8A, 0x7F, 0x05}, "mov bh, [bx + 5]" );
    runSingle( {0x8B, 0x4C, 0xF8}, "mov cx, [si - 8]" );
    runSingle( {0x89, 0x43, 0xFF}, "mov [bp + di - 1], ax" );
    runSingle( {0x3A, 0x45, 0x7F}, "cmp al, [di + 127]" );
    runSingle( {0x3B, 0x84, 0x80, 0x00}, "cmp ax, [si + 128]" );
    runSingle( {0x8A, 0x80, 0x87, 0x13}, "mov al, [bx + si + 4999]" );
    runSingle( {0x89, 0xB6, 0xE8, 0x03}, "mov [bp + 1000], si" );
    runSingle( {0x03, 0xBB, 0xD4, 0xFE}, "add di, [bp + di - 300]" );
    // segment override
    runSingle( {0x26, 0x8B, 0x07}, "mov ax, es:[bx]" );
    runSingle( {0x2E, 0x8B, 0x56, 0x04}, "mov dx, cs:[bp + 4]" );
    runSingle( {0x36, 0x00, 0x04}, "add ss:[si], al" );
    // SRegRm
    runSingle({0x8E, 0xC0}, "mov es, ax");
    runSingle({0x8C, 0xD8}, "mov ax, ds");
    runSingle({0x8C, 0x17}, "mov [bx], ss");
    runSingle( {0x8E, 0x5E, 0x02}, "mov ds, [bp + 2]" );
    // RmReg
    runSingle({0x84, 0x0F}, "test [bx], cl");
    runSingle({0x85, 0xD0}, "test ax, dx");
    runSingle({0x87, 0x46, 0x00}, "xchg [bp], ax");
    runSingle({0x86, 0xD9}, "xchg cl, bl");
    // RegMem
    runSingle( {0x8D, 0x71, 0x08}, "lea si, [bx + di + 8]" );
    runSingle({0xC4, 0x7E, 0x00}, "les di, [bp]");
    runSingle( {0xC5, 0x1E, 0x34, 0x12}, "lds bx, [0x1234]" );
    // RegImm
    runSingle({0xB1, 0x0C}, "mov cl, 12");
    runSingle({0xB5, 0xF4}, "mov ch, 244");
    runSingle({0xB9, 0x0C, 0x00}, "mov cx, 12");
    runSingle({0xB9, 0xF4, 0xFF}, "mov cx, 65524");
    runSingle({0xBC, 0x34, 0x12}, "mov sp, 4660");
    // AccImm
    runSingle({0x04, 0x09}, "add al, 9");
    runSingle({0x05, 0xE8, 0x03}, "add ax, 1000");
    runSingle({0x3C, 0xE2}, "cmp al, 226");
    runSingle({0x2D, 0x34, 0x12}, "sub ax, 4660");
    runSingle({0xA8, 0x01}, "test al, 1");
    runSingle( {0xA9, 0x00, 0x80}, "test ax, 32768" );
    // AccMem
    runSingle( {0xA0, 0x34, 0x12}, "mov al, [0x1234]" );
    runSingle( {0xA1, 0x34, 0x12}, "mov ax, [0x1234]" );
    runSingle( {0xA2, 0x34, 0x12}, "mov [0x1234], al" );
    runSingle( {0xA3, 0x34, 0x12}, "mov [0x1234], ax" );
    runSingle( {0x26, 0xA0, 0x34, 0x12}, "mov al, es:[0x1234]" );
    // RmImm
    runSingle( {0xC6, 0x07, 0x07}, "mov byte [bx], 7" );
    runSingle( {0xC7, 0x07, 0x34, 0x12}, "mov word [bx], 4660" );
    runSingle( {0xC7, 0x82, 0xE8, 0x03, 0x1D, 0x00}, "mov word [bp + si + 1000], 29" );
    runSingle( {0xC6, 0x45, 0xFE, 0xFF}, "mov byte [di - 2], 255" );
    // RmImmSExt
    runSingle( {0x80, 0x07, 0x22}, "add byte [bx], 34" );
    runSingle( {0x83, 0x82, 0xE8, 0x03, 0x1D}, "add word [bp + si + 1000], 29" );
    runSingle({0x83, 0xC6, 0x02}, "add si, 2");
    runSingle({0x83, 0xE9, 0xF4}, "sub cx, 65524");
    runSingle( {0x81, 0x27, 0x34, 0x12}, "and word [bx], 4660" );
    runSingle( {0x83, 0x3E, 0x34, 0x12, 0x1D}, "cmp word [0x1234], 29" );
    runSingle({0x80, 0xCA, 0x80}, "or dl, 128");
    // RmShift
    runSingle({0xD1, 0xE0}, "shl ax, 1");
    runSingle({0xD0, 0x2F}, "shr byte [bx], 1");
    runSingle({0xD3, 0xFA}, "sar dx, cl");
    runSingle( {0xD3, 0x46, 0x04}, "rol word [bp + 4], cl" );
    runSingle({0xD0, 0xDB}, "rcr bl, 1");
    // Group3
    runSingle( {0xF6, 0x07, 0x07}, "test byte [bx], 7" );
    runSingle( {0xF7, 0x02, 0x34, 0x12}, "test word [bp + si], 4660" );
    runSingle({0xF7, 0xD0}, "not ax");
    runSingle({0xF6, 0x1F}, "neg byte [bx]");
    runSingle({0xF6, 0xE1}, "mul cl");
    runSingle({0xF7, 0x2C}, "imul word [si]");
    runSingle({0xF7, 0xF3}, "div bx");
    runSingle( {0xF6, 0x7E, 0xFF}, "idiv byte [bp - 1]" );
    // Rm
    runSingle({0xFE, 0x07}, "inc byte [bx]");
    runSingle( {0xFF, 0x4E, 0x02}, "dec word [bp + 2]" );
    runSingle({0xFF, 0x37}, "push word [bx]");
    runSingle({0x8F, 0x00}, "pop word [bx + si]");
    runSingle({0xFF, 0x17}, "call [bx]");
    runSingle({0xFF, 0x23}, "jmp [bp + di]");
    runSingle({0xFF, 0x1F}, "call far [bx]");
    runSingle( {0xFF, 0x2E, 0x34, 0x12}, "jmp far [0x1234]" );
    // Reg16
    runSingle({0x40}, "inc ax");
    runSingle({0x41}, "inc cx");
    runSingle({0x4E}, "dec si");
    runSingle({0x55}, "push bp");
    runSingle({0x5F}, "pop di");
    // AccReg16
    runSingle({0x91}, "xchg ax, cx");
    runSingle({0x97}, "xchg ax, di");
    runSingle({0x90}, "nop");
    // SReg
    runSingle({0x06}, "push es");
    runSingle({0x1F}, "pop ds");
    runSingle({0x0E}, "push cs");
    runSingle({0x17}, "pop ss");
    // AccPort / AccDx
    runSingle({0xE4, 0x60}, "in al, 96");
    runSingle({0xE5, 0x60}, "in ax, 96");
    runSingle({0xE6, 0x61}, "out 97, al");
    runSingle({0xE7, 0x61}, "out 97, ax");
    runSingle({0xEC}, "in al, dx");
    runSingle({0xED}, "in ax, dx");
    runSingle({0xEE}, "out dx, al");
    runSingle({0xEF}, "out dx, ax");
    // Imm8 / Imm16
    runSingle({0xCD, 0x21}, "int 33");
    runSingle({0xD4, 0x0A}, "aam");
    runSingle({0xD5, 0x0A}, "aad");
    runSingle({0xC2, 0x04, 0x00}, "ret 4");
    runSingle({0xCA, 0x08, 0x00}, "retf 8");
    // Rel8 / Rel16 / Far
    runSingle({0x75, 0xFC}, "jne $-2");
    runSingle({0x74, 0x00}, "je $+2");
    runSingle({0x7C, 0x08}, "jl $+10");
    runSingle({0xE3, 0x9A}, "jcxz $-100");
    runSingle({0xE2, 0xF8}, "loop $-6");
    runSingle({0xEB, 0x7F}, "jmp short $+129");
    runSingle( {0xE9, 0xE8, 0x03}, "jmp near $+1003" );
    runSingle({0xE8, 0x09, 0xFE}, "call $-500");
    runSingle( {0xEA, 0x78, 0x56, 0x34, 0x12}, "jmp 0x1234:22136" );
    runSingle( {0x9A, 0x78, 0x56, 0x34, 0x12}, "call 0x1234:22136" );
    // None
    runSingle({0xC3}, "ret");
    runSingle({0xCB}, "retf");
    runSingle({0xCC}, "int3");
    runSingle({0xCE}, "into");
    runSingle({0xCF}, "iret");
    runSingle({0xF4}, "hlt");
    runSingle({0xF8}, "clc");
    runSingle({0x98}, "cbw");
    runSingle({0x99}, "cwd");
    runSingle({0x9C}, "pushf");
    runSingle({0xD7}, "xlat");
    runSingle({0x27}, "daa");
    // String
    runSingle({0xA4}, "movsb");
    runSingle({0xA5}, "movsw");
    runSingle({0xA6}, "cmpsb");
    runSingle({0xAB}, "stosw");
    runSingle({0xAC}, "lodsb");
    runSingle({0xAF}, "scasw");
    runSingle({0xF3, 0xA4}, "rep movsb");
    runSingle({0xF3, 0xAB}, "rep stosw");
    runSingle({0xF2, 0xAE}, "repne scasb");
    runSingle({0xF3, 0xA7}, "repe cmpsw");
    // Lock
    runSingle( {0xF0, 0x87, 0x07}, "lock xchg [bx], ax" );
    // Undocumented aliases (not nasm-verified, derived from the opcode map)
    runSingle({0x82, 0xC0, 0x05}, "add al, 5");     // 82 = alias of 80
    runSingle({0xF6, 0xC8, 0x05}, "test al, 5");    // F6 reg=001 = alias of TEST
    runSingle({0x60, 0x02}, "jo $+4");              // 60 = alias of 70
    runSingle({0xC1}, "ret");                       // C1 = alias of C3
    runSingle({0x0F}, "pop cs");                    // 8086 only

    std::cout << "Counter: " << counter << std::endl;


    return 0;
}