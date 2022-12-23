class mt19937:
    def __init__(self):
        self.mt = [0] * 624
        self.mti = 625

    def seed(self, seed):
        self.mt[0] = seed & 0xFFFFFFFF
        for i in range(1, 624):
            self.mt[i] = (
                1812433253 * (self.mt[i - 1] ^ (self.mt[i - 1] >> 30)) + i
            ) & 0xFFFFFFFF
        self.mti = 624

    def init_by_array(self, key):
        self.seed(19650218)
        i = 1
        j = 0
        k = max(624, len(key))
        for ki in range(k):
            self.mt[i] = (
                (
                    self.mt[i]
                    ^ ((self.mt[i - 1] ^ (self.mt[i - 1] >> 30)) * 1664525)
                )
                + key[j]
                + j
            ) & 0xFFFFFFFF
            i += 1
            j += 1
            if i >= 624:
                self.mt[0] = self.mt[623]
                i = 1
            if j >= len(key):
                j = 0
        for ki in range(624):
            self.mt[i] = (
                (
                    self.mt[i]
                    ^ ((self.mt[i - 1] ^ (self.mt[i - 1] >> 30)) * 1566083941)
                )
                - i
            ) & 0xFFFFFFFF
            i += 1
            if i >= 624:
                self.mt[0] = self.mt[623]
                i = 1
        self.mt[0] = 0x80000000

    def int32(self):
        if self.mti >= 624:
            if self.mti == 625:
                self.seed(5489)

            for k in range(623):
                y = (self.mt[k] & 0x80000000) | (self.mt[k + 1] & 0x7FFFFFFF)
                if k < 624 - 397:
                    self.mt[k] = (
                        self.mt[k + 397]
                        ^ (y >> 1)
                        ^ (0x9908B0DF if y & 1 else 0)
                    )
                else:
                    self.mt[k] = (
                        self.mt[k + 397 - 624]
                        ^ (y >> 1)
                        ^ (0x9908B0DF if y & 1 else 0)
                    )

            y = (self.mt[623] & 0x80000000) | (self.mt[0] & 0x7FFFFFFF)
            self.mt[623] = (
                self.mt[396] ^ (y >> 1) ^ (0x9908B0DF if y & 1 else 0)
            )
            self.mti = 0

        y = self.mt[self.mti]
        self.mti += 1

        y ^= y >> 11
        y ^= (y << 7) & 0x9D2C5680
        y ^= (y << 15) & 0xEFC60000
        y ^= y >> 18

        return y

    def int32b(self):
        if self.mti == 625:
            self.seed(5489)

        k = self.mti

        if k == 624:
            k = 0
            self.mti = 0

        if k == 623:
            y = (self.mt[623] & 0x80000000) | (self.mt[0] & 0x7FFFFFFF)
            self.mt[623] = (
                self.mt[396] ^ (y >> 1) ^ (0x9908B0DF if y & 1 else 0)
            )
        else:
            y = (self.mt[k] & 0x80000000) | (self.mt[k + 1] & 0x7FFFFFFF)
            if k < 624 - 397:
                self.mt[k] = (
                    self.mt[k + 397] ^ (y >> 1) ^ (0x9908B0DF if y & 1 else 0)
                )
            else:
                self.mt[k] = (
                    self.mt[k + 397 - 624]
                    ^ (y >> 1)
                    ^ (0x9908B0DF if y & 1 else 0)
                )

        y = self.mt[self.mti]
        self.mti += 1

        y ^= y >> 11
        y ^= (y << 7) & 0x9D2C5680
        y ^= (y << 15) & 0xEFC60000
        y ^= y >> 18

        return y
