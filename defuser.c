#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t low  = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t mid  = (uint32_t)(challenge >> 16);
    uint32_t s2 = (high + low * 2) ^ 0x767cu;
    uint32_t s0 = (low + low * 2 + mid) ^ 0x53feu;
    uint32_t ans = (s0 + s0 * 2) ^ (s2 + 0xb3f7u);
    values[count++] = ans;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t a[6];
    a[0] = (uint32_t)((challenge >> 7) & 0xfu) + 0x11u;
    uint32_t shift = 0x1b;
    for (uint32_t i = 1; i < 6; i++) {
        uint32_t digit = (uint32_t)((challenge >> shift) & 0xfu);
        a[i] = (digit ^ (i + 0xeu)) + 5u * a[i - 1];
        shift += 5;
    }
    for (int i = 0; i < 6; i++) values[count++] = a[i];
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t sel = (uint32_t)(challenge & 7u);
    uint32_t x   = (uint32_t)((challenge >> 3) ^ (challenge >> 32));
    uint32_t ans = 0;
    switch (sel) {
        case 0: ans = 5u * x + 0x2a323u;                    break;
        case 1: ans = (4u * x) ^ 0xebd7u;                   break;
        case 2: ans = (x ^ 0xe83fu) + (x >> 2);             break;
        case 3: ans = (x ^ 0xba8cu) + (x >> 1);             break;
        case 4: ans = (4u * x) ^ 0x9899u;                   break;
        case 5: ans = 5u * x + 0x16e1u;                     break;
        case 6: ans = 3u * x + 0x481au;                     break;
        case 7: ans = 5u * x + 0x19172u;                    break;
    }
    values[count++] = sel;
    values[count++] = ans;
    return write_answer(out, cap, values, count);
}


static uint32_t p4_walk(uint32_t target, uint32_t lo, uint32_t hi, uint32_t key) {
    uint32_t mid = lo + (hi - lo) / 2;
    if (mid == target)
        return (mid + 0xa6u) ^ key;
    if (target < mid) {
        uint32_t r = p4_walk(target, lo, mid - 1u, key);
        return (3u * r ^ (mid + key)) ^ 0x5u;
    } else {
        uint32_t r = p4_walk(target, mid + 1u, hi, key);
        return (3u * r ^ (mid + key)) ^ 0x15u;
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t target = (uint32_t)(challenge & 0x7fu);
    uint32_t key = (uint32_t)((challenge >> 7) ^ (challenge >> 32));
    values[count++] = p4_walk(target, 0, 0x7fu, key);
    return write_answer(out, cap, values, count);
}



static const struct { uint8_t next; uint32_t a, b; } p5_nodes[] = {
    /* 0 */ { 7, 0x0020u, 0x6083u },
    /* 1 */ { 8, 0x072fu, 0x0fecu },
    /* 2 */ { 5, 0x0640u, 0xb1efu },
    /* 3 */ { 4, 0x040fu, 0xcda3u },
    /* 4 */ { 9, 0x0951u, 0xd620u },
    /* 5 */ { 1, 0x010au, 0x41b4u },
    /* 6 */ { 11, 0x0a15u, 0x5132u },
    /* 7 */ { 3, 0x0859u, 0x18deu },
    /* 8 */ { 10, 0x0b18u, 0x23ffu },
    /* 9 */ { 2, 0x0331u, 0xd9f8u },
    /* 10 */{ 0, 0x0536u, 0xaf8au},
    /* 11 */{ 255, 0x0255u, 0x0013u },
};
int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t idx   = (uint32_t)(challenge & 7u);
    uint32_t steps = (uint32_t)((challenge >> 3) & 3u);
    static const uint8_t heads[8] = { 10, 0, 7, 3, 4, 9, 2, 5 };
    uint32_t cur = heads[idx];
    uint32_t acc = (uint32_t)((challenge >> 6) ^ (challenge >> 32));
    acc ^= p5_nodes[cur].b;
    acc ^= 0x462au;
    acc += p5_nodes[cur].a;
    uint32_t last_a = p5_nodes[cur].a;
    for (uint32_t i = 0; i < steps; i++) {
        cur = p5_nodes[cur].next;
        acc ^= p5_nodes[cur].b;
        acc += p5_nodes[cur].a;
        last_a = p5_nodes[cur].a;
    }
    values[count++] = acc;
    values[count++] = last_a;
    return write_answer(out, cap, values, count);
}



static uint32_t p6_op(uint32_t idx, uint32_t x) {
    switch (idx) {
        case 0: return (x ^ 0x63a0u) + 4u * x;
        case 1: return ((x >> 5) ^ x) + 0xe2f9u;
        case 2: return (2u * x + 0x3fa8u) ^ (5u * x);
        case 3: return (x >> 2) | (x << 30);
        case 4: return ((x >> 5) ^ x) + 0xe2f9u;
        case 5: return ((x << 2) | (x >> 30)) ^ 0xbf2au;
        case 6: return (2u * x + 0x3fa8u) ^ (5u * x);
        default: return (x + 0xdb2au) ^ (x >> 3);
    }
}
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t x  = (uint32_t)((challenge >> 6) ^ (challenge >> 32));
    uint32_t r1 = p6_op((uint32_t)(challenge & 7u), x);
    uint32_t r2 = p6_op((uint32_t)((challenge >> 3) & 7u), r1 ^ 0x8f94u);
    values[count++] = r2 + 0x7074u;
    return write_answer(out, cap, values, count);
}






int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    uint32_t e1 = (uint32_t)((challenge >> 32) ^ challenge);
    uint32_t e2 = (uint32_t)((context >> 32) ^ context);
    uint32_t A  = (uint32_t)context & 3u;
    uint32_t r9 = (0xb1b8u * A) ^ (uint32_t)(context >> 16);
    uint32_t v  = e1 ^ e2;
    uint32_t m  = (((v << 3) | (v >> 29)) + 0xb1b8u) ^ 0xcd5fu;
    uint32_t B  = m ^ r9;
    uint32_t C  = B ^ ((e1 >> 3) + e2 + A + 0xcd5fu);
    snprintf(out, cap, "%016lx%016lxOPEN %u %u %u",
         (unsigned long)challenge, (unsigned long)context, A, B, C);
    return 0;
}
