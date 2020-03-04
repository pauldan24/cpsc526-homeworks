#!/usr/bin/env python3

import sys
# 3. kasiski's analysis

# 2. c[i] = P[i] ^ k[i]
transmission1 = open('./transmission1', 'rb').read()
transmission2 = open('./transmission2', 'rb').read()
plaintext = open('./plain', 'r').read()

idx = 0
key = ''
for byte in transmission1:
    key += chr(byte ^ ord(plaintext[idx]))
    idx += 1

# key 'snowboard'
actual_key = ''
for i in range(9):
    actual_key += key[i]

i = 0
plain2 = ''
for c in transmission2:
    plain2 += chr(c ^ ord(actual_key[i % len(actual_key)]))
    i += 1

# The summer evening...
print(plain2)
