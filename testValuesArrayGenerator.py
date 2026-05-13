#!/bin/python3
import numpy as np

lower_bound = 10 # units of us
upper_bound = 300
n = 20
SMCLK_8MHZ = 2.0**20 * 8.0
div = 1.0
clock_macro = "* SMCLK_8MHZ / " + str(div)

testValuesArray = np.linspace(lower_bound, upper_bound, n)
for value in testValuesArray:
    if value * 10.0**(-6.0) * SMCLK_8MHZ / div >= 2**16:
        raise Exception("Test Value exceeds 16-bit limit")
intValuesArray = np.uint16(testValuesArray * 10.0**(-6.0) * SMCLK_8MHZ / div)

print("#define TEST_SIZE " + str(n))

print("\n  uint16_t testValues[" + str(n) + "] = {")
for i in range(len(testValuesArray)):
    print("    (uint16_t)(", "%.4f" % round(testValuesArray[i], 4) + "E-6", clock_macro, "),"
          , end='')
    print(" // ", intValuesArray[i])
print("  };")

print("\n  float testValuesFloat[" + str(n) + "] = {")
for value in intValuesArray:
    print("    (", "%.4f" % (value / (10.0**(-6.0) * SMCLK_8MHZ / div)), "),")
print("  };")
