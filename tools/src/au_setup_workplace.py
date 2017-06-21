#!/usr/bin

import sys
import os
import subprocess

tag = "meow workplace"

def main(argv):
    # setup workplace
    print("{0} setup workplace".format(tag), file=sys.stderr)
    dest_path = "/test/images/" + argv[2] + "/"

    os.makedirs(dest_path, exist_ok=False)
    subprocess.call("(cd {0}; tar cf - .) | (cd {1}; tar xf -)".format(argv[1], dest_path), shell=True)


if __name__ == '__main__':
    main(sys.argv)
