import sys
import subprocess


archive_path = "/test/images/"
dest_path = "/test/containers/"


def main(argv):
    pid = argv[1]
    if len(argv) == 3:
        subprocess.call(["kill", "-s", argv[2], "--", pid])
    id_file = open(archive_path + pid, 'r')
    unique_name = id_file.read()
    subprocess.call(["rm", archive_path + pid])
    subprocess.call(["rm", "-rd", dest_path + unique_name])


if __name__ == '__main__':
    main(sys.argv)