import os
import subprocess
import sys

tag = "[aucont_list.py]"

def main():
    img_path = "/test/images"
    print("{0} img_path={1}".format(tag, img_path), file=sys.stderr)
    if os.path.exists(img_path):
        print("{0} calling ls".format(tag), file=sys.stderr)
        subprocess.call("ls /test/images", shell=True)
    print("{0} exit".format(tag), file=sys.stderr)


if __name__ == '__main__':
    print("[aucont_list.py]", file=sys.stderr)
    main()
