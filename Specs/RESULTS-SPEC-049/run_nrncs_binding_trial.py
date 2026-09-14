#!/usr/bin/env python3
import datetime as dt
import pathlib
import subprocess
import time

ROOT = pathlib.Path('/home/gandalf/workspace/novagenesis')
OUT = ROOT / 'Specs/RESULTS-SPEC-049/nrncs-binding-trial'
OUT.mkdir(parents=True, exist_ok=True)
KEY = str(pathlib.Path.home() / '.ssh/id_ed25519_hermes')
EXPECTED = '1af604dad66148618cc0e578c28adb4a9829e607'

def stamp(): return dt.datetime.now(dt.timezone.utc).isoformat()
def run(cmd, output=None): return subprocess.run(cmd, stdout=output, stderr=subprocess.STDOUT, text=True, check=False)
def ssh(ip, command): return ['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','-i',KEY,f'root@{ip}',command]
def launch(name, cmd):
    f=open(OUT/name,'w',buffering=1); p=subprocess.Popen(cmd,stdout=f,stderr=subprocess.STDOUT,text=True); return p,f

contract=OUT/'trial-contract.txt'
contract.write_text(f'start_utc={stamp()}\ncommit={EXPECTED}\npgcs_window=240s\nnrncs_window=100s\n',encoding='utf-8')

# Start and wait for both guests.
run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','qm start 101; qm start 102'])
ok=0
for _ in range(30):
    ok=sum(run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=3','-i',KEY,f'root@{ip}','hostname'],open(OUT/'ssh-probe.txt','w')).returncode==0 for ip in ('192.168.0.61','192.168.0.36'))
    if ok==2: break
    time.sleep(5)
if ok != 2: raise SystemExit(2)

# Clean and preflight guests.
for role,ip in (('repo','192.168.0.61'),('source','192.168.0.36')):
    cmd=('cd /root/workspace/novagenesis && bash Scripts/Simple/clean.sh; '
         f'test "$(git rev-parse HEAD)" = "{EXPECTED}"; '
         'test -x cmake-build-debug/PGCS -a -x cmake-build-debug/NRNCS; '
         'test -f IO/PGCS/PGCS.ini -a -f IO/NRNCS/NRNCS.ini')
    rc=run(ssh(ip,cmd),open(OUT/f'{role}-preflight.txt','w')).returncode
    contract.open('a').write(f'{role}_preflight_rc={rc}\n')
    if rc: raise SystemExit(2)

# GDB observer for NRNCS publication ingress and HT storage invocation.
gdb='''set pagination off
set confirm off
break NRNCS/src/NRPubBind01.cpp:92
commands
silent
printf "NRPUB_INGRESS\\n"
print Category.at(0)
print Key.at(0)
print Values.size()
if Values.size() > 0
print Values.at(0)
end
continue
end
break Common/src/HTStoreBind01.cpp:113
commands
silent
printf "HT_STORE_CALL\\n"
print Category
print Key
print PArguments.size()
if PArguments.size() > 0
print PArguments.at(0)
end
continue
end
run
'''
gdb_lines='\n'.join(gdb.splitlines())
setup=("cd /root/workspace/novagenesis && "
       "printf '%s\\n' '"+gdb_lines+"' > /tmp/ng049-nrncs.gdb && "
       "test -s /tmp/ng049-nrncs.gdb && "
       "gdb -batch -ex 'info functions NRPubBind01::Run' -ex 'info functions HTStoreBind01::Run' ./cmake-build-debug/NRNCS >/dev/null 2>&1")
rc=run(ssh('192.168.0.36',setup),open(OUT/'nrncs-gdb-preflight.txt','w')).returncode
contract.open('a').write(f'nrncs_gdb_preflight_rc={rc}\n')
if rc: raise SystemExit(2)

capture=['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','rm -f /tmp/ng049-tcpdump.txt; timeout --signal=TERM --kill-after=5s 250s tcpdump -eni vmbr0 "ether proto 0x1234" -c 4000 > /tmp/ng049-tcpdump.txt 2>&1']
repo=ssh('192.168.0.61','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 240 --term-grace 10 --log /tmp/ng049-repo-pgcs.log -- ./cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:79:bb:15 1200')
source=ssh('192.168.0.36','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 240 --term-grace 10 --log /tmp/ng049-source-pgcs.log -- ./cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:65:00:08 1200')
nrncs=ssh('192.168.0.36','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 100 --term-grace 10 --log /tmp/ng049-nrncs.log -- gdb -batch -x /tmp/ng049-nrncs.gdb --args ./cmake-build-debug/NRNCS /root/workspace/novagenesis/IO/NRNCS/')

(cap,capf)=launch('tcpdump-supervisor.txt',capture)
time.sleep(2)
(repo_p,repo_f)=launch('repo-pgcs-supervisor.txt',repo)
(src_p,src_f)=launch('source-pgcs-supervisor.txt',source)
# The PGCS first periodic is 120 s; launch NRNCS after that configured boundary.
time.sleep(130)
(nr_p,nr_f)=launch('nrncs-supervisor.txt',nrncs)
results={}
for name,p,f in [('repo_pgcs',repo_p,repo_f),('source_pgcs',src_p,src_f),('nrncs',nr_p,nr_f),('tcpdump',cap,capf)]:
    results[name]=p.wait(); f.close()

for role,ip,remote in [('repo','192.168.0.61','/tmp/ng049-repo-pgcs.log'),('source','192.168.0.36','/tmp/ng049-source-pgcs.log'),('nrncs','192.168.0.36','/tmp/ng049-nrncs.log')]:
    results[role+'_fetch']=run(['scp','-q','-o','BatchMode=yes','-i',KEY,f'root@{ip}:{remote}',str(OUT/f'{role}.log')],open(OUT/f'{role}-fetch.txt','w')).returncode
results['tcpdump_fetch']=run(['scp','-q','-o','BatchMode=yes','root@192.168.0.200:/tmp/ng049-tcpdump.txt',str(OUT/'ng049-tcpdump.txt')],open(OUT/'tcpdump-fetch.txt','w')).returncode

# Cleanup and stop VMs autonomously.
for role,ip in [('repo','192.168.0.61'),('source','192.168.0.36')]:
    cleanup='cd /root/workspace/novagenesis && bash Scripts/Simple/clean.sh; printf "FINAL_PROCESSES\\n"; pgrep -a -f "(^|/)(PGCS|NRNCS|ContentApp|NBTestApp)( |$)" || true; printf "FINAL_SHM\\n"; ipcs -m; printf "FINAL_SEM\\n"; ipcs -s'
    results[role+'_cleanup']=run(ssh(ip,cleanup),open(OUT/f'{role}-cleanup.txt','w')).returncode
results['vm_stop']=run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','qm stop 101; qm stop 102; qm status 101; qm status 102'],open(OUT/'vm-stop.txt','w')).returncode
with contract.open('a') as f:
    for k,v in results.items(): f.write(f'{k}_rc={v}\n')
    f.write(f'end_utc={stamp()}\n')
print(results); print(contract.read_text())
raise SystemExit(0)
