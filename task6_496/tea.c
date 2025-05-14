void decipher(unsigned int *const v, unsigned int *const w, const unsigned int *const k) {
    register unsigned int y = v[0], z = v[1], sum = 0xC6EF3720;
    unsigned int delta = 0x9E3779B9;
    register unsigned int a = k[0], b = k[1], c = k[2], d = k[3];
    for (int n = 0; n < 32; n++) {
        z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
        y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
        sum -= delta;
    }
    w[0] = y; w[1] = z;
}
