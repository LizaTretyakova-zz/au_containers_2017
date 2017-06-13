import sys
import subprocess


archive_path = "/test/images/"
dest_path = "/test/containers/"
tag = "[aucont_stop.py]"


def main(argv):
    pid = argv[1]
    if len(argv) == 3:
        print("{0} kill {1} with pid {2}".format(tag, argv[2], pid), file=sys.stderr)
        subprocess.call(["kill", "-s", argv[2], "--", pid])
    id_file = open(archive_path + pid, 'r')
    unique_name = id_file.read()
    print("{0} rm {1}".format(tag, archive_path + pid), file=sys.stderr)
    subprocess.call(["rm", archive_path + pid])
    print("{0} rm {1}".format(tag, dest_path + unique_name), file=sys.stderr)
    subprocess.call(["rm", "-rd", dest_path + unique_name])


if __name__ == '__main__':
    print(tag, file=sys.stderr)
    main(sys.argv)
