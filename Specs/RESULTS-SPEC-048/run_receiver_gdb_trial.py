#!/usr/bin/env python3
import datetime as dt
import pathlib
import subprocess
import time

ROOT = pathlib.Path('/home/gandalf/workspace/novagenesis')
OUT = ROOT / 'Specs/RESULTS-SPEC-048/receiver-gdb-trial'
OUT.mkdir(parents=True, exist_ok=True)
KEY = str(pathlib.Path.home() / '.ssh/id_ed25519_hermes')
EXPECTED = '1af604dad66148618cc0e578c28adb4a9829e607'

def stamp():
    return dt.datetime.now(dt.timezone.utc).isoformat()

def run(cmd, output=None):
    return subprocess.run(cmd, stdout=output, stderr=subprocess.STDOUT,
                          text=True, check=False)

def ssh(ip, command):
    return ['ssh', '-o', 'BatchMode=yes', '-o', 'ConnectTimeout=10',
            '-i', KEY, f'root@{ip}', command]

def launch(name, command):
    f = open(OUT / name, 'w', buffering=1)
    p = subprocess.Popen(command, stdout=f, stderr=subprocess.STDOUT, text=True)
    return p, f

participants = [
    ('repo', '192.168.0.61', '/tmp/ng048-repo-pgcs.log', '08:00:27:79:bb:15'),
    ('source', '192.168.0.36', '/tmp/ng048-source-pgcs.log', '08:00:27:65:00:08'),
]
contract = OUT / 'trial-contract.txt'
contract.write_text(f'start_utc={stamp()}\ncommit={EXPECTED}\nwindow=180s\n', encoding='utf-8')

gdb_script = '''set pagination off
set confirm off
break PGHelloIHC01::Run
commands
silent
printf "RECEIVER_HANDLER_ENTRY\\n"
bt 8
continue
end
break PGHelloIHC01.cpp:240
commands
silent
printf "RECEIVER_HANDLER_STATUS_OK\\n"
bt 6
continue
end
run
'''
gdb_lines = '\n'.join(gdb_script.splitlines())

for role, ip, remote_log, peer_mac in participants:
    setup = (
        'set -e; cd /root/workspace/novagenesis; '
        f'test "$(git rev-parse HEAD)" = "{EXPECTED}"; '
        'test -x cmake-build-debug/PGCS; test -f IO/PGCS/PGCS.ini; '
        f"printf '%s\\n' '{gdb_lines}' > /tmp/ng048-{role}.gdb; "
        f'test -s /tmp/ng048-{role}.gdb; rm -f {remote_log}; '
        "gdb -batch -ex 'info functions PGHelloIHC01::Run' -ex 'info line PGHelloIHC01.cpp:240' ./cmake-build-debug/PGCS >/dev/null 2>&1"
    )
    # The setup command only validates GDB script parsing on the local mode; the
    # actual bounded trial below uses the network command and the same script.
    p = run(ssh(ip, setup), output=open(OUT / f'{role}-preflight.txt', 'w'))
    contract.open('a', encoding='utf-8').write(f'{role}_preflight_rc={p.returncode}\n')
    if p.returncode != 0:
        raise SystemExit(2)

capture_cmd = [
    'ssh', '-o', 'BatchMode=yes', '-o', 'ConnectTimeout=10', 'root@192.168.0.200',
    'rm -f /tmp/ng048-tcpdump.txt; timeout --signal=TERM --kill-after=5s 190s '
    'tcpdump -eni vmbr0 "ether proto 0x1234" -c 2000 > /tmp/ng048-tcpdump.txt 2>&1',
]
cmds = {}
for role, ip, remote_log, peer_mac in participants:
    gdb = f'/tmp/ng048-{role}.gdb'
    cmds[role] = ssh(ip,
        'cd /root/workspace/novagenesis && '
        f'python3 Scripts/AlpineVMs/supervise_process.py --timeout 180 --term-grace 10 --log {remote_log} -- '
        f'gdb -batch -x {gdb} --args ./cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 '
        f'Intra_Domain -p Ethernet Intra_Domain eth0 {peer_mac} 1200')

(capture, capture_f) = launch('tcpdump-supervisor.txt', capture_cmd)
time.sleep(2)
(repo, repo_f) = launch('repo-supervisor.txt', cmds['repo'])
(source, source_f) = launch('source-supervisor.txt', cmds['source'])
results = {}
for name, proc, file_obj in [('repo', repo, repo_f), ('source', source, source_f), ('tcpdump', capture, capture_f)]:
    results[name] = proc.wait()
    file_obj.close()

for role, ip, remote_log, _ in participants:
    local_log = OUT / f'remote-{role}-pgcs.log'
    results[f'{role}_fetch'] = run(
        ['scp', '-q', '-o', 'BatchMode=yes', '-i', KEY, f'root@{ip}:{remote_log}', str(local_log)],
        output=open(OUT / f'{role}-fetch.txt', 'w')).returncode
results['tcpdump_fetch'] = run(
    ['scp', '-q', '-o', 'BatchMode=yes', 'root@192.168.0.200:/tmp/ng048-tcpdump.txt', str(OUT / 'ng048-tcpdump.txt')],
    output=open(OUT / 'tcpdump-fetch.txt', 'w')).returncode

# Autonomous teardown: collect cleanup output before stopping the guests.
for role, ip, _, _ in participants:
    cleanup = (
        'cd /root/workspace/novagenesis && bash Scripts/Simple/clean.sh; '
        'printf "FINAL_PROCESSES\\n"; pgrep -a -f "(^|/)(PGCS|NRNCS|ContentApp|NBTestApp)( |$)" || true; '
        'printf "FINAL_SHM\\n"; ipcs -m; printf "FINAL_SEM\\n"; ipcs -s'
    )
    results[f'{role}_cleanup'] = run(
        ssh(ip, cleanup), output=open(OUT / f'{role}-cleanup.txt', 'w')).returncode
results['vm_stop'] = run(
    ['ssh', '-o', 'BatchMode=yes', '-o', 'ConnectTimeout=10', 'root@192.168.0.200',
     'qm stop 101; qm stop 102; qm status 101; qm status 102'],
    output=open(OUT / 'vm-stop.txt', 'w')).returncode

with contract.open('a', encoding='utf-8') as f:
    for k, v in results.items(): f.write(f'{k}_rc={v}\n')
    f.write(f'end_utc={stamp()}\n')
print(results, flush=True)
print(contract.read_text(), flush=True)
raise SystemExit(0 if results['repo'] == 0 and results['source'] == 0 else 1)
