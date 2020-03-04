#!/usr/bin/env python3

import re
from operator import itemgetter

with open('./ulysses', 'r') as f:
    # read the text and transform to lowercase
    text = f.read().lower()

    # get all words via regex
    words_regex = r'\b[a-z]+\b'
    all_words = re.findall(words_regex, text)

    # count all words
    frequency = {}
    for word in all_words:
        count = frequency.get(word, 0)
        frequency[word] = count + 1

    # sort the dictionary by count
    # and transform into a list (sorted by word count)
    sorted_frequency = list(
        reversed(sorted(frequency.items(), key=lambda entry: entry[1]))
    )

    most_freq = sorted_frequency[0]
    longest = max(sorted_frequency, key=lambda entry: len(entry[0]))
    longest_count_gt_25 = max(sorted_frequency, key=lambda entry: entry[1] > 25 and len(
        entry[0]))

    print(frequency['handkerchief'])
    print(f'1. most frequent: {most_freq}')
    print(f'2. longest: {longest}')
    print(f'3. longest (with count > 25): {longest_count_gt_25}')
