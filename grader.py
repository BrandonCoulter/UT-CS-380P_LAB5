#!/usr/bin/env python3

import sys, re, os
import numpy as np
import subprocess


def compare_outputs(actual_file, expected_file, tolerance=1e-5):
    try:
        with open(actual_file) as f1, open(expected_file) as f2:
            actual = np.array(
                [float(x) for line in f1.readlines()[1:] for x in line.split()]
            )
            expected = np.array(
                [float(x) for line in f2.readlines()[1:] for x in line.split()]
            )

        if actual.shape != expected.shape:
            return False
        return np.allclose(actual, expected, rtol=tolerance)
    except:
        return False


def main():
    inputs = ["nb-10.txt", "nb-100.txt", "nb-100000.txt"]
    steps = [1, 10, 100]

    print(f"{'Input':<15} {'Steps':<8} {'Result'}")
    print("-" * 35)

    for input_file in inputs:
        num = re.search(r"nb-(\d+)", input_file).group(1)
        for step in steps:
            cmd = f"mpirun -n 1 ./nbody -s {step} -i ./input/{input_file} -t 0.5 -d 0.005 -o output.txt"

            if subprocess.run(cmd, shell=True).returncode == 0:
                expected = f"expected_output/nb-{num}-{step}-steps.results.txt"
                result = (
                    "PASS"
                    if os.path.exists(expected)
                    and compare_outputs("output.txt", expected)
                    else "FAIL"
                )
            else:
                result = "FAIL"

            print(f"{input_file:<15} {step:<8} {result}")


if __name__ == "__main__":
    main()