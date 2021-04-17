import sys
sys.path.insert(0, "../cmake-build-debug-gcc/test")

import jpeg2000_test as m


def test():
    assert m.__version__ == "dev"
    assert m.add(1, 2) == 3
    assert m.subtract(1, 2) == -1
    print("Finished")


if __name__ == "__main__":
    test()
