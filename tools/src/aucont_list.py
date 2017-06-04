import os
import subprocess


def main():
    img_path = "/test/images"
    if os.path.exists(img_path):
        subprocess.call("ls /test/images", shell=True)


if __name__ == '__main__':
    main()