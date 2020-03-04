#!/usr/bin/env python

from collections import Counter
from itertools import cycle
import string
import re


def get_text():
    with open('ulysses', 'r') as f:
        # strip punctuation
        text = ''.join(
            list(filter(lambda ch: ch not in set(string.punctuation), f.read())))

        # remove new lines and convert to uppercase
        return ''.join(text.split()).upper()

    return ''


def get_cipher(bytes=False):
    filename = 'transmission3'

    if bytes:
        return open(filename, 'rb').read()

    with open(filename, 'r') as f:
        return f.read()
    return ''


def xor_ch(a, b):
    return chr(ord(a) ^ ord(b))


def break_viginere(plaintext, ciphertext):
    # get the text that will be used to break the cipher
    # XOR a char from the plaintext with a char from the ciphertext (zip)
    # cycle through all of the chars in the ciphertext (cycle)
    # this will help us to find the duplicate copies of the XORed key
    # and will help to perform kasiski's analysis
    to_decrypt = [xor_ch(a, b) for (a, b) in zip(plaintext, cycle(ciphertext))]

    # convert the result to string
    text_to_decrypt = ''.join(to_decrypt)

    found_key = ''
    for key_len in range(1, len(text_to_decrypt)):
        counter = Counter(text_to_decrypt[j: j + key_len]
                          for j in range(len(text_to_decrypt) - key_len))
        key, found_count = counter.most_common(1)[0]

        # skip small words
        if key_len < 3:
            continue

        # if a key appears less than 5 times, break
        if found_count > 5:
            print(
                f'key_len: {key_len}  found_count: {found_count}  key: {key}'
            )
            found_key = key
        else:
            break

    # get the decrypted text by XORing the key char with the ciphertext char
    decrypted_text = [xor_ch(a, b)
                      for (a, b) in zip(cycle(found_key), ciphertext)]

    return ''.join(decrypted_text), found_key


def main():
    plaintext = get_text()
    ciphertext = get_cipher()

    if len(plaintext) > 0 and len(ciphertext) > 0:
        decrypted, key = break_viginere(plaintext, ciphertext)
        print('---------------------------------------------')
        print(f'Key: {key}')
        print('Found text: ')
        print(decrypted)


if __name__ == "__main__":
    main()
