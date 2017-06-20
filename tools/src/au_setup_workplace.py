#!/usr/bin

import sys

tag = "meow workplace"
print(tag, file=sys.stderr)

def main(argv):
    # setup workplace
    print("{0} setup workplace".format(tag), file=sys.stderr)
    dest_path = "/test/images/" + argv[2] + "/"

    os.makedirs(dest_path, exist_ok=False)
    subprocess.call("(cd {0}; tar cf - .) | (cd {1}; tar xf -)".format(argv[1], dest_path))


if __name__ == '__main__':
    print(tag, file=sys.stderr)
    main(sys.argv)
