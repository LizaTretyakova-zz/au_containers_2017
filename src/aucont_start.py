import os
import subprocess
import sys
import ipaddress


def setup_net(ip, cont_pid):
    host_ip = str(int(ipaddress.ip_address(ip)) + 1)
    host_veth_name = "veth" + str(cont_pid)
    cont_veth_name = "veth1"

    subprocess.call(["ip", "link", "add", host_veth_name, "type", "veth", "peer", "name", cont_veth_name])
    subprocess.call(["ip", "link", "set", cont_veth_name, "netns", cont_pid])
    subprocess.call(["ip", "netns", "exec", cont_pid, "ip", "link", "set", cont_veth_name, "up"])
    subprocess.call(["ip", "netns", "exec", cont_pid, "ip", "addr", "add", ip + "/24", "dev", cont_veth_name])
    subprocess.call(["ip", "link", "set", host_veth_name, "up", "&&",
                     "ip", "addr", "add", host_ip + "/24", "dev", host_veth_name])
# ip link add veth0 type veth peer name veth1
# ip link set veth1 netns my_netns
# ip netns exec my_netns ip link set veth1 up
# ip netns exec my_netns ip addr add 10.0.0.1/24 dev veth1
# ip link set veth0 up && ip addr add 10.0.0.2/24 dev veth0


def main(argv):
    print(argv[0], file=sys.stderr)

    c_cmd = ["./aucont_start.elf"]
    net_cmd = ["./aucont_setup_net"]
    cpu_cmd = ["./aucont_setup_cpu"]

    i = 1
    while argv[i][0] == "-":
        if argv[i] == "-d":
            c_cmd.append("1")
            i += 1
        elif argv[i] == "--net":
            net_cmd.append(argv[i + 1])
            i += 2
        elif argv[i] == "--cpu":
            cpu_cmd.append(argv[i + 1])
            i += 2
    if len(c_cmd) == 1:
        c_cmd.append("0")
    c_cmd.extend(argv[i:])

    output = subprocess.check_output(c_cmd, stderr=subprocess.STDOUT)
    cont_pid = output.decode('UTF-8')[:-1]
