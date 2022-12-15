#!/usr/bin/python

import glob
import os
import re

checks = [
        'publish/windows',
        'publish/linux',
        'publish/android',
        ]

latestRE = re.compile(r'.*/(?P<latest>arpcalc-.*?)"')

for folder in checks:
    archives = glob.glob(os.path.join(folder, 'arpcalc-*'))
    with open(os.path.join(folder, 'latest.html'), 'r', encoding='utf8') as fh:
        line = fh.read().strip()
    m = latestRE.match(line)

    latest = m.group('latest')

    for f in archives:
        if os.path.basename(f) != latest:
            os.remove(f)
