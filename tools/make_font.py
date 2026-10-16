#!/usr/bin/env python3
"""
Builds fonts/ARPCalcSans-Regular.ttf: the parts of Noto Sans that ARPCalc
needs, plus the arrows and mathematical operators (which Noto Sans doesn't
have) from Noto Sans Math, in one font.

One font with every character the app shows means Qt never looks for a
fallback font, which on Windows means reading every installed font (about
0.5 s at start-up with ~1100 fonts).  The fonts are licensed under the SIL
Open Font License 1.1 (fonts/OFL.txt); the result has its own family name so
that it can't be confused with an installed Noto Sans.

Needs fonttools (pip install fonttools).  Usage:
    tools/make_font.py NotoSans-Regular.ttf NotoSansMath-Regular.ttf
with the fonts from https://github.com/notofonts/notofonts.github.io
(fonts/NotoSans/hinted/ttf and fonts/NotoSansMath/hinted/ttf).
"""
import os
import sys
import tempfile

from fontTools import subset
from fontTools.merge import Merger
from fontTools.ttLib import TTFont

FAMILY = "ARPCalc Sans"
OUTPUT = os.path.join(os.path.dirname(os.path.abspath(__file__)),
        "..", "fonts", "ARPCalcSans-Regular.ttf")

# From Noto Sans: Basic Latin, Latin-1 Supplement, Latin Extended-A, Greek,
# General Punctuation, superscripts and subscripts, currency symbols
TEXT_RANGES = "U+0020-007E,U+00A0-017F,U+0370-03FF,U+2000-206F,U+2070-209F,U+20A0-20CF"
# From Noto Sans Math: Arrows and Mathematical Operators
MATHS_RANGES = "U+2190-21FF,U+2200-22FF"


def make_subset(source, ranges, destination, layout_features):
    options = subset.Options()
    options.layout_features = layout_features
    options.drop_tables += ["MATH"]
    options.name_IDs = ["*"]
    options.notdef_outline = True
    font = TTFont(source)
    subsetter = subset.Subsetter(options)
    subsetter.populate(unicodes=subset.parse_unicodes(ranges))
    subsetter.subset(font)
    font.save(destination)


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit(1)
    noto_sans, noto_sans_math = sys.argv[1:]

    with tempfile.TemporaryDirectory() as tmp:
        text = os.path.join(tmp, "text.ttf")
        maths = os.path.join(tmp, "maths.ttf")
        make_subset(noto_sans, TEXT_RANGES, text, ["*"])
        # Without Noto Sans Math's layout features or MATH table (dropped by
        # make_subset): they bring in the large versions of the operators
        # (e.g. display-size integrals, nearly three times the text's height)
        make_subset(noto_sans_math, MATHS_RANGES, maths, [])
        text_font = TTFont(text)
        font = Merger().merge([text, maths])

    # The merged font's vertical metrics are the largest of the two fonts'
    # (and its bounding box covers every glyph), which puts the text off
    # centre on Android (which pads text to the bounding box) and spaces
    # lines further apart.  Use Noto Sans's metrics, which suit the text.
    for table, attributes in (
            ("hhea", ["ascent", "descent", "lineGap"]),
            ("OS/2", ["sTypoAscender", "sTypoDescender", "sTypoLineGap",
                      "usWinAscent", "usWinDescent"]),
            ("head", ["yMin", "yMax"])):
        for attribute in attributes:
            setattr(font[table], attribute, getattr(text_font[table], attribute))

    for record in font["name"].names:
        if record.nameID in (1, 16):
            record.string = FAMILY
        elif record.nameID == 4:
            record.string = FAMILY + " Regular"
        elif record.nameID == 6:
            record.string = FAMILY.replace(" ", "") + "-Regular"
        elif record.nameID == 3:
            record.string = "ARPCalc: " + FAMILY + " Regular (from Noto Sans "
            record.string += "and Noto Sans Math)"
    font.save(OUTPUT)
    print("Created " + os.path.normpath(OUTPUT))


if __name__ == "__main__":
    main()
