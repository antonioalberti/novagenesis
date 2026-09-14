#!/usr/bin/env python3
import datetime as dt
import pathlib
import subprocess
import time

ROOT = pathlib.Path('/home/gandalf/workspace/novagenesis')
OUT = ROOT / 'Specs/RESULTS-SPEC-047/network/trial'
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

def launch(path, command):
    f = open(OUT / path, 'w', buffering=1)
    p = subprocess.Popen(command, stdout=f, stderr=subprocess.STDOUT, text=True)
    return p, f

contract = OUT / 'trial-contract.txt'
contract.write_text(
    f'trial_start_utc={stamp()}\n'
    f'commit={EXPECTED}\nwindow=180s\n', encoding='utf-8')

participants = [
    ('repo', '192.168.0.61', '/tmp/ng047-repo-pgcs.log', '08:00:27:79:bb:15'),
    ('source', '192.168.0.36', '/tmp/ng047-source-pgcs.log', '08:00:27:65:00:08'),
]

# Preflight both guests before starting any participant.
for role, ip, remote_log, peer_mac in participants:
    preflight = (
        'set -e; '
        'cd /root/workspace/novagenesis; '
        f'test "$(git rev-parse HEAD)" = "{EXPECTED}"; '
        'test -x cmake-build-debug/PGCS; '
        'test -f IO/PGCS/PGCS.ini; '
        f'rm -f {remote_log}; '
        f'test ! -e {remote_log}; '
        f': > {remote_log}; '
        f'test -f {remote_log} -a -w {remote_log}; '
        f'rm -f {remote_log}'
    )
    p = run(ssh(ip, preflight), output=open(OUT / f'{role}-preflight.txt', 'w'))
    contract.open('a', encoding='utf-8').write(f'{role}_preflight_rc={p.returncode}\n')
    if p.returncode != 0:
        contract.open('a', encoding='utf-8').write(f'setup_failed={role}\n')
        raise SystemExit(2)

capture_cmd = [
    'ssh', '-o', 'BatchMode=yes', '-o', 'ConnectTimeout=10',
    'root@192.168.0.200',
    'rm -f /tmp/ng047-tcpdump.txt; '
    'timeout --signal=TERM --kill-after=5s 190s '
    'tcpdump -eni vmbr0 "ether proto 0x1234" -c 2000 > /tmp/ng047-tcpdump.txt 2>&1',
]
repo_cmd = ssh('192.168.0.61',
    'cd /root/workspace/novagenesis && '
    'python3 Scripts/AlpineVMs/supervise_process.py --timeout 180 --term-grace 10 '
    '--log /tmp/ng047-repo-pgcs.log -- '
    './cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 '
    'Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:79:bb:15 1200')
source_cmd = ssh('192.168.0.36',
    'cd /root/workspace/novagenesis && '
    'python3 Scripts/AlpineVMs/supervise_process.py --timeout 180 --term-grace 10 '
    '--log /tmp/ng047-source-pgcs.log -- '
    './cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 '
    'Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:65:00:08 1200')

(capture, capture_f) = launch('tcpdump-supervisor.txt', capture_cmd)
time.sleep(2)
(repo, repo_f) = launch('repo-supervisor.txt', repo_cmd)
(source, source_f) = launch('source-supervisor.txt', source_cmd)

results = {}
for name, proc, file_obj in [('repo', repo, repo_f), ('source', source, source_f), ('tcpdump', capture, capture_f)]:
    results[name] = proc.wait()
    file_obj.close()

# Fetch exact remote logs; failed retrieval is a setup/evidence failure.
for role, ip, remote_log, _ in participants:
    local_log = OUT / f'remote-{role}-pgcs.log'
    scp_cmd = ['scp', '-q', '-o', 'BatchMode=yes', '-i', KEY,
               f'root@{ip}:{remote_log}', str(local_log)]
    fetch = run(scp_cmd, output=open(OUT / f'{role}-log-fetch.txt', 'w'))
    results[f'{role}_log_fetch'] = fetch.returncode

# Fetch the bridge capture even when tcpdump terminates by its normal bound.
fetch_capture = run(
    ['scp', '-q', '-o', 'BatchMode=yes', 'root@192.168.0.200:/tmp/ng047-tcpdump.txt',
     str(OUT / 'ng047-tcpdump.txt')],
    output=open(OUT / 'tcpdump-fetch.txt', 'w'))
results['tcpdump_fetch'] = fetch_capture.returncode

with contract.open('a', encoding='utf-8') as f:
    for name, value in results.items():
        f.write(f'{name}_rc={value}\n')
    f.write(f'trial_end_utc={stamp()}\n')

print(f'results={results}', flush=True)
print(contract.read_text(), flush=True)
raise SystemExit(0 if results['repo'] == 0 and results['source'] == 0 else 1)
