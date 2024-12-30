import random
import argparse

def parseargs():
    a = argparse.ArgumentParser()
    a.add_argument('count', type=int, help='Number of operations to include')
    return a.parse_args()


def random_operator():
    return random.choice(["+", "-", "*", "/"])


def random_expr(len, filename):
    count = len // 3
    opstack = []
    opwritten = 0
    with open(filename, "w") as f:
        for i in range(count):
            a = random.randint(1, 25)
            b = random.randint(1, 25)
            o = random_operator()
            f.write(f"{a} {b} ")
            if random.random() < 0.5:
                opstack.append(o)
            else:
                f.write(f"{o} ")
                opwritten += 1

            while opstack:
                if random.random() < 0.25:
                    f.write(f"{opstack.pop()} ")
                    opwritten += 1
                else:
                    break

        while opstack:
            f.write(f"{opstack.pop()} ")
            opwritten += 1

        while opwritten < ((count * 2) - 1):
            f.write(f"{random_operator()} ")
            opwritten += 1


opts = parseargs()
random_expr(opts.count, "rin.txt")
