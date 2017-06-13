import sys
import subprocess
import ipaddress


def setup_net(ip, cont_pid):
    host_ip = str(int(ipaddress.ip_address(ip)) + 1)
    host_veth_name = "veth" + cont_pid
    cont_veth_name = "veth1"

    subprocess.call(["ip", "link", "add", host_veth_name, "type", "veth", "peer", "name", cont_veth_name])
    subprocess.call(["ip", "link", "set", cont_veth_name, "netns", cont_pid])
    subprocess.call(["ip", "netns", "exec", cont_pid, "ip", "link", "set", cont_veth_name, "up"])
    subprocess.call(["ip", "netns", "exec", cont_pid, "ip", "addr", "add", ip + "/24", "dev", cont_veth_name])
    subprocess.call(["ip", "link", "set", host_veth_name, "up", "&&",
                     "ip", "addr", "add", host_ip + "/24", "dev", host_veth_name])


if __name__ == '__main__':
    print(tag, file=sys.stderr)
    setup_net(sys.argv[1], argv[2])
