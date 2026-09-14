#!/usr/bin/env python3
import datetime as dt
import pathlib
import subprocess
import time

ROOT=pathlib.Path('/home/gandalf/workspace/novagenesis'); OUT=ROOT/'Specs/RESULTS-SPEC-050/repository-trial'; OUT.mkdir(parents=True,exist_ok=True)
KEY=str(pathlib.Path.home()/'.ssh/id_ed25519_hermes'); EXPECTED='1af604dad66148618cc0e578c28adb4a9829e607'
def stamp(): return dt.datetime.now(dt.timezone.utc).isoformat()
def run(c,o=None): return subprocess.run(c,stdout=o,stderr=subprocess.STDOUT,text=True,check=False)
def ssh(ip,c): return ['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','-i',KEY,f'root@{ip}',c]
def launch(n,c):
 f=open(OUT/n,'w',buffering=1); p=subprocess.Popen(c,stdout=f,stderr=subprocess.STDOUT,text=True); return p,f
contract=OUT/'trial-contract.txt'; contract.write_text(f'start_utc={stamp()}\ncommit={EXPECTED}\n',encoding='utf-8')
run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','qm start 101; qm start 102'])
ok=0
for _ in range(30):
 ok=sum(run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=3','-i',KEY,f'root@{ip}','hostname'],open(OUT/'probe.txt','w')).returncode==0 for ip in ('192.168.0.61','192.168.0.36'))
 if ok==2: break
 time.sleep(5)
if ok!=2: raise SystemExit(2)
for role,ip in [('repo','192.168.0.61'),('source','192.168.0.36')]:
 c=('cd /root/workspace/novagenesis && bash Scripts/Simple/clean.sh; '
    f'test "$(git rev-parse HEAD)" = "{EXPECTED}"; '
    'test -x cmake-build-debug/PGCS -a -x cmake-build-debug/NRNCS -a -x cmake-build-debug/ContentApp; '
    'sha256sum IO/Repository1/App.ini IO/NRNCS/NRNCS.ini')
 rc=run(ssh(ip,c),open(OUT/f'{role}-preflight.txt','w')).returncode; contract.open('a').write(f'{role}_preflight_rc={rc}\n')
 if rc: raise SystemExit(2)
cap=['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','rm -f /tmp/ng050-tcpdump.txt; timeout --signal=TERM --kill-after=5s 280s tcpdump -eni vmbr0 "ether proto 0x1234" -c 5000 > /tmp/ng050-tcpdump.txt 2>&1']
repo=ssh('192.168.0.61','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 260 --term-grace 10 --log /tmp/ng050-repo-pgcs.log -- ./cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:79:bb:15 1200')
source=ssh('192.168.0.36','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 260 --term-grace 10 --log /tmp/ng050-source-pgcs.log -- ./cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:65:00:08 1200')
nrncs=ssh('192.168.0.36','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 180 --term-grace 10 --log /tmp/ng050-nrncs.log -- ./cmake-build-debug/NRNCS /root/workspace/novagenesis/IO/NRNCS/')
app=ssh('192.168.0.61','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 100 --term-grace 10 --log /tmp/ng050-repository.log -- ./cmake-build-debug/ContentApp /root/workspace/novagenesis/IO/Repository1/ Repository')
(c,cf)=launch('tcpdump-supervisor.txt',cap); time.sleep(2); (rp,rf)=launch('repo-pgcs-supervisor.txt',repo); (sp,sf)=launch('source-pgcs-supervisor.txt',source)
time.sleep(130); (np,nf)=launch('nrncs-supervisor.txt',nrncs); time.sleep(10); (ap,af)=launch('repository-supervisor.txt',app)
results={}
for n,p,f in [('repo_pgcs',rp,rf),('source_pgcs',sp,sf),('nrncs',np,nf),('repository',ap,af),('tcpdump',c,cf)]: results[n]=p.wait(); f.close()
for role,ip,remote in [('repo-pgcs','192.168.0.61','/tmp/ng050-repo-pgcs.log'),('source-pgcs','192.168.0.36','/tmp/ng050-source-pgcs.log'),('nrncs','192.168.0.36','/tmp/ng050-nrncs.log'),('repository','192.168.0.61','/tmp/ng050-repository.log')]:
 results[role+'_fetch']=run(['scp','-q','-o','BatchMode=yes','-i',KEY,f'root@{ip}:{remote}',str(OUT/f'{role}.log')],open(OUT/f'{role}-fetch.txt','w')).returncode
results['tcpdump_fetch']=run(['scp','-q','-o','BatchMode=yes','root@192.168.0.200:/tmp/ng050-tcpdump.txt',str(OUT/'ng050-tcpdump.txt')],open(OUT/'tcpdump-fetch.txt','w')).returncode
for role,ip in [('repo','192.168.0.61'),('source','192.168.0.36')]:
 c='cd /root/workspace/novagenesis && bash Scripts/Simple/clean.sh; printf "FINAL_PROCESSES\\n"; pgrep -a -f "(^|/)(PGCS|NRNCS|ContentApp|NBTestApp)( |$)" || true; printf "FINAL_SHM\\n"; ipcs -m; printf "FINAL_SEM\\n"; ipcs -s'
 results[role+'_cleanup']=run(ssh(ip,c),open(OUT/f'{role}-cleanup.txt','w')).returncode
results['vm_stop']=run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','qm stop 101; qm stop 102; qm status 101; qm status 102'],open(OUT/'vm-stop.txt','w')).returncode
with contract.open('a') as f:
 for k,v in results.items(): f.write(f'{k}_rc={v}\n')
 f.write(f'end_utc={stamp()}\n')
print(results); print(contract.read_text())
raise SystemExit(0)
