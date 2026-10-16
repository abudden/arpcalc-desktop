#!/usr/bin/python

import glob
import os
import re
import sys

checks = [
        'publish/windows',
        'publish/linux',
        'publish/android',
        ]

latestRE = re.compile(r'.*/(?P<latest>arpcalc-.*?)"')
winVersionRE = re.compile(r'arpcalc-windows-(shared|static)-(?P<version>.*)')

for folder in checks:
    archives = glob.glob(os.path.join(folder, 'arpcalc-*'))
    with open(os.path.join(folder, 'latest.html'), 'r', encoding='utf8') as fh:
        line = fh.read().strip()
    print(folder)
    m = latestRE.match(line)

    latest = m.group('latest')

    for f in archives:
        if 'windows' in folder:
            if m := winVersionRE.match(latest):
                if n := winVersionRE.match(os.path.basename(f)):
                    if m.group('version') == n.group('version'):
                        print(f"Leaving {f}")
                        continue
            if '--dry-run' in sys.argv:
                print(f"Removing {f}")
            else:
                os.remove(f)
        else:
            if os.path.basename(f) != latest:
                if '--dry-run' in sys.argv:
                    print(f"Removing {f}")
                else:
                    os.remove(f)
            else:
                print(f"Leaving {f}")
