#!/usr/bin/python

import argparse
import os
import random

import pint
u = pint.UnitRegistry()

def get_unit_mapping():

    unit_mapping = {
            "Acres": u.acre,
            "Angstroms": u.angstrom,
            "Atmosphere": u.atmosphere,
            "Bar": u.bar,
            "British Thermal Units": u.btu,
            "BTUs Per Hour": u.btu / u.hour,
            "Bytes": u.byte,
            "Calories Per Second": u.calorie / u.second,
            "Calories": u.calorie,
            "Celsius": u.celsius,
            "Centimetres": u.centimetre,
            "Chains": u.chain,
            "Cubic Centimetres": u.cc,
            "Cubic Decimetres": u.decimetre * u.decimetre * u.decimetre,
            "Cubic Feet": u.cubic_feet,
            "Cubic Inches": u.cubic_inch,
            "Cubic Metres": u.metre * u.metre * u.metre,
            "Cubic Millimetres": u.millimetre * u.millimetre * u.millimetre,
            "Cubic Yards": u.cubic_yard,
            "Days": u.day,
            "Degrees": u.degree,
            "Fahrenheit": u.fahrenheit,
            "Fathoms": u.fathom,
            "Feet Per Second": u.feet / u.second,
            "Feet": u.feet,
            "Fluid Ounces": u.imperial_fluid_ounce,
            "Furlongs": u.furlong,
            "Gallons": u.imperial_gallon,
            "Gibibytes": u.gibibyte,
            "Gigabytes": u.gigabyte,
            "Grams": u.gram,
            "Gram-Force": u.gram_force,
            "Gram-Force Centimetres": u.gram_force * u.centimetre,
            "Gram-Force Metres": u.gram_force * u.metre,
            "Gram-Force Millimetres": u.gram_force * u.millimetre,
            "Hectares": u.hectare,
            "Hectopascal": u.hectopascal,
            "Hertz": u.revolution / u.second,
            "Horsepower (Mech)": u.horsepower,
            "Horsepower (Metric)": u.metric_horsepower,
            "Hours": u.hour,
            "Hundredweight": u.UK_hundredweight,
            "Inches of Mercury": u.inch_Hg_60F, # ?
            "Inches": u.inch,
            "Joules": u.joule,
            "Kelvin": u.kelvin,
            "Kibibytes": u.kibibyte,
            "Kilobytes": u.kilobyte,
            "Kilocalories": u.kilocalorie,
            "Kilogram-Force": u.kilogram_force,
            "Kilogram-Force Centimetres": u.kilogram_force * u.centimetre,
            "Kilogram-Force Metres": u.kilogram_force * u.metre,
            "Kilogram-Force Millimetres": u.kilogram_force * u.millimetre,
            "Kilograms Per Sq. cm": u.kilogram_force / (u.cm * u.cm),
            "Kilograms": u.kilogram,
            "Kilojoules": u.kilojoule,
            "Kilometres Per Hour": u.kilometre / u.hour,
            "Kilometres Per Litre": u.kilometre / u.litre,
            "Kilometres": u.kilometre,
            "Kilonewtons": u.kilonewton,
            "Kilopascal": u.kilopascal,
            "Kilowatt-Hours": u.kilowatt_hour,
            "Kilowatts": u.kilowatt,
            "Knots": u.knot,
            "Light Years": u.light_year,
            "Litres Per 100 Kilometres": u.litre / (100 * u.kilometre),
            "Litres": u.litre,
            "Mebibytes": u.mebibyte,
            "Megabytes": u.megabyte,
            "Megajoules": u.megajoule,
            "Megapascal": u.megapascal,
            "Megawatts": u.megawatt,
            "Metres Per Hour": u.metre / u.hour,
            "Metres Per Second": u.metre / u.second,
            "Metres": u.metre,
            "Microgram": u.microgram,
            "Micrometres": u.micrometre,
            "Micronewtons": u.micronewton,
            "Microns": u.micrometre,
            "Microseconds": u.microsecond,
            "Miles Per Gallon": u.mile / u.imperial_gallon,
            "Miles Per Hour": u.mile / u.hour,
            "Miles Per Litre": u.mile / u.litre,
            "Miles Per US Gallon": u.mile / u.US_liquid_gallon,
            "Miles": u.mile,
            "Millibar": u.millibar,
            "Milligrams": u.milligram,
            "Millilitres": u.millilitre,
            "Millimetres": u.millimetre,
            "Millinewtons": u.millinewton,
            "Milliseconds": u.millisecond,
            "Mils": u.mil,
            "Minutes": u.minute,
            "Nanometres": u.nanometre,
            "Nanoseconds": u.nanosecond,
            "Nautical Miles": u.nautical_mile,
            "Newton Centimetres": u.newton * u.centimetre,
            "Newton Metres": u.newton * u.metre,
            "Newton Millimetres": u.newton * u.millimetre,
            "Newtons": u.newton,
            "Ounces": u.ounce,
            "Ounce-Force": u.ounce_force,
            "Ounce-Force Feet": u.ounce_force * u.feet,
            "Ounce-Force Inches": u.ounce_force * u.inches,
            "Pascal": u.pascal,
            "Pints": u.imperial_pint,
            "Points": u.point,
            "Pound-Force": u.pound_force,
            "Pound-Force Feet": u.pound_force * u.feet,
            "Pound-Force Inches": u.pound_force * u.inches,
            "Pounds Per Sq. Inch": u.psi,
            "Pounds": u.pound,
            "RPM": u.rpm,
            "Radians Per Second": u.radian / u.second,
            "Radians": u.radian,
            "Seconds": u.second,
            "Sq. Centimetres": u.centimetre * u.centimetre,
            "Sq. Feet": u.feet * u.feet,
            "Sq. Inches": u.inch * u.inch,
            "Sq. Kilometres": u.kilometre * u.kilometre,
            "Sq. Metres": u.metre * u.metre,
            "Sq. Miles": u.mile * u.mile,
            "Sq. Millimetres": u.millimetre * u.millimetre,
            "Sq. Yards": u.yard * u.yard,
            "Stone": u.stone,
            "Tebibytes": u.tebibyte,
            "Terabytes": u.terabyte,
            "Thou": u.thou,
            "Tonnes": u.tonne,
            "Tons": u.UK_ton,
            "Torr": u.torr,
            "US Fluid Ounces": u.US_fluid_ounce,
            "US Gallons": u.US_liquid_gallon,
            "US Hundredweight": u.US_hundredweight,
            "US Pints": u.US_pint,
            "US Tons": u.US_ton,
            "Watts": u.watt,
            "Weeks": u.week,
            "Yards": u.yard,
            }
    return unit_mapping


def arghandler():
    parser = argparse.ArgumentParser()
    parser.add_argument("--unit-list",
            type=str,
            required=True,
            dest="unitlist")
    parser.add_argument("--out-file",
            type=str,
            required=True,
            dest="outfile")
    args = parser.parse_args()
    return args

def read_units(infile_name):
    result = {}
    with open(infile_name, 'r') as fh:
        units = [i.strip().split('_') for i in fh]
    for unit in units:
        if unit[0] not in result:
            result[unit[0]] = []
        result[unit[0]].append(unit[1])
    return result


def generate_test_list(units, outfile):
    # Tests that have been manually checked and which
    # python-pint seems to give inaccurate answers
    skip_these = [
            "Litres Per 100 Kilometres",
            "Inches of Mercury",
            # Seems too messy to bother with:
            "Day of Year",
            "Date in Year",
            ]

    tests = []
    unit_mapping = get_unit_mapping()
    for category in units:
        for from_unit in units[category]:
            for to_unit in units[category]:
                if from_unit == to_unit:
                    continue
                if from_unit in skip_these or to_unit in skip_these:
                    # Tests that have been manually checked and which
                    # python-pint seems to give inaccurate answers
                    continue
                for unit in [from_unit, to_unit]:
                    if unit not in unit_mapping:
                        raise Exception("Unrecognised unit: %s" % unit)
                for from_value in [1.0, random.uniform(1.0, 1000.0)]:
                    with_unit = u.Quantity(from_value, unit_mapping[from_unit])
                    try:
                        expected_result = with_unit.to(unit_mapping[to_unit]).magnitude
                    except pint.errors.DimensionalityError:
                        # Try inverting the input unit just in case the problem is simple
                        with_unit = (1.0/from_value) * (1 / unit_mapping[from_unit])
                        expected_result = with_unit.to(unit_mapping[to_unit]).magnitude
                    tests.append([
                            category,
                            "%0.20g" % from_value,
                            from_unit,
                            to_unit,
                            "%0.20g" % expected_result
                            ])
    with open(outfile, 'w') as fh:
        for test in tests:
            fh.write("_".join(test)+"\n")

def main():
    random.seed()

    args = arghandler()

    # In case of exception, clean up previous runs to cause any
    # error to stop higher processes
    if os.path.exists(args.outfile):
        os.remove(args.outfile)

    units = read_units(args.unitlist)

    generate_test_list(units, args.outfile)

if __name__ == "__main__":
    main()
