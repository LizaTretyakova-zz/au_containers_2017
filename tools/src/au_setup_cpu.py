import subprocess
import sys


def setup_cpu(perc, cont_pid):
    cgroup_path = "/sys/fs/cgroup/cpu/" + cont_pid
    period = 1000000
    quota = int(perc) * period / 100

    subprocess.call(["mkdir", cgroup_path])
    subprocess.call(["echo", str(cont_pid), ">", cgroup_path + "/tasks"])
    subprocess.call(["echo", period, ">", cgroup_path + "cpu.cfs_period_us"])
    subprocess.call(["echo", quota, ">", cgroup_path + "cpu.cfs_quota_us"])


if __name__ == '__main__':
    print(tag, file=sys.stderr)
    setup_cpu(argv[1], argv[2])
