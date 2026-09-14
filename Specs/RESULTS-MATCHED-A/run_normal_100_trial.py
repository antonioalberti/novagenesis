#!/usr/bin/env python3
import datetime as dt
import hashlib
import pathlib
import shutil
import subprocess
import time

ROOT=pathlib.Path('/home/gandalf/workspace/novagenesis')
OUT=ROOT/'Specs/RESULTS-MATCHED-A/normal-100-1af604d'; OUT.mkdir(parents=True,exist_ok=True)
KEY=str(pathlib.Path.home()/'.ssh/id_ed25519_hermes'); EXPECTED='1af604dad66148618cc0e578c28adb4a9829e607'
def stamp(): return dt.datetime.now(dt.timezone.utc).isoformat()
def run(c,o=None): return subprocess.run(c,stdout=o,stderr=subprocess.STDOUT,text=True,check=False)
def ssh(ip,c): return ['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','-i',KEY,f'root@{ip}',c]
def launch(n,c):
 f=open(OUT/n,'w',buffering=1); p=subprocess.Popen(c,stdout=f,stderr=subprocess.STDOUT,text=True); return p,f
contract=OUT/'trial-contract.txt'; contract.write_text(f'start_utc={stamp()}\ncommit={EXPECTED}\nworkload=100 fresh JPEGs\n',encoding='utf-8')
# source helper: fresh immutable staging, then Source ContentApp
helper=OUT/'source-helper.sh'
helper.write_text('''#!/bin/sh
set -eu
BASE=/root/workspace/novagenesis
RUN_ID=$(date -u +%Y%m%dT%H%M%SZ)-matched-a
IO_DIR="$BASE/IO/SourceStaging/$RUN_ID"
printf '%s\\n' "$RUN_ID" > /tmp/ng050-source-run-id
cd "$BASE"
python3 Scripts/Python/BuildPhotos.py --staging "$IO_DIR" different 100 800 600
(cd "$IO_DIR" && sha256sum -c manifest.sha256 >/dev/null)
printf 'RUN_ID=%s\\n' "$RUN_ID"
printf 'MANIFEST='; sha256sum "$IO_DIR/manifest.sha256"
cd "$BASE/cmake-build-debug"
exec ./ContentApp "$IO_DIR/" Source
''',encoding='utf-8'); helper.chmod(0o755)
# Start guests and wait SSH
run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','qm start 101; qm start 102'])
ok=0
for _ in range(30):
 ok=sum(run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=3','-i',KEY,f'root@{ip}','hostname'],open(OUT/'probe.txt','w')).returncode==0 for ip in ('192.168.0.61','192.168.0.36'))
 if ok==2: break
 time.sleep(5)
if ok!=2: raise SystemExit(2)
# Preserve provenance, clean, preflight.
for role,ip in [('repo','192.168.0.61'),('source','192.168.0.36')]:
 c=('cd /root/workspace/novagenesis && bash Scripts/Simple/clean.sh; '
    f'test "$(git rev-parse HEAD)" = "{EXPECTED}"; '
    'test -x cmake-build-debug/PGCS -a -x cmake-build-debug/NRNCS -a -x cmake-build-debug/ContentApp; '
    'sha256sum cmake-build-debug/PGCS cmake-build-debug/NRNCS cmake-build-debug/ContentApp')
 rc=run(ssh(ip,c),open(OUT/f'{role}-preflight.txt','w')).returncode; contract.open('a').write(f'{role}_preflight_rc={rc}\n')
 if rc: raise SystemExit(2)
# Copy helper to Source.
run(['scp','-q','-i',KEY,str(helper),'root@192.168.0.36:/tmp/ng050-source-helper.sh'])
cap=['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','rm -f /tmp/ng050a-tcpdump.txt; timeout --signal=TERM --kill-after=5s 380s tcpdump -eni vmbr0 "ether proto 0x1234" -c 10000 > /tmp/ng050a-tcpdump.txt 2>&1']
repo=ssh('192.168.0.61','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 360 --term-grace 10 --log /tmp/ng050a-repo-pgcs.log -- ./cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:79:bb:15 1200')
source=ssh('192.168.0.36','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 360 --term-grace 10 --log /tmp/ng050a-source-pgcs.log -- ./cmake-build-debug/PGCS /root/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:65:00:08 1200')
nrncs=ssh('192.168.0.36','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 300 --term-grace 10 --log /tmp/ng050a-nrncs.log -- ./cmake-build-debug/NRNCS /root/workspace/novagenesis/IO/NRNCS/')
repository=ssh('192.168.0.61','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 260 --term-grace 10 --log /tmp/ng050a-repository.log -- ./cmake-build-debug/ContentApp /root/workspace/novagenesis/IO/Repository1/ Repository')
sourceapp=ssh('192.168.0.36','cd /root/workspace/novagenesis && python3 Scripts/AlpineVMs/supervise_process.py --timeout 180 --term-grace 10 --log /tmp/ng050a-source-app.log -- sh /tmp/ng050-source-helper.sh')
(c,cf)=launch('tcpdump-supervisor.txt',cap); time.sleep(2); (rp,rf)=launch('repo-pgcs-supervisor.txt',repo); (sp,sf)=launch('source-pgcs-supervisor.txt',source)
time.sleep(130); (np,nf)=launch('nrncs-supervisor.txt',nrncs); time.sleep(10); (ap,af)=launch('repository-supervisor.txt',repository); time.sleep(10); (sap,saf)=launch('source-app-supervisor.txt',sourceapp)
results={}
for n,p,f in [('repo_pgcs',rp,rf),('source_pgcs',sp,sf),('nrncs',np,nf),('repository',ap,af),('source_app',sap,saf),('tcpdump',c,cf)]: results[n]=p.wait(); f.close()
# Fetch logs and run id/artifacts.
for role,ip,remote in [('repo-pgcs','192.168.0.61','/tmp/ng050a-repo-pgcs.log'),('source-pgcs','192.168.0.36','/tmp/ng050a-source-pgcs.log'),('nrncs','192.168.0.36','/tmp/ng050a-nrncs.log'),('repository','192.168.0.61','/tmp/ng050a-repository.log'),('source-app','192.168.0.36','/tmp/ng050a-source-app.log')]:
 results[role+'_fetch']=run(['scp','-q','-o','BatchMode=yes','-i',KEY,f'root@{ip}:{remote}',str(OUT/f'{role}.log')],open(OUT/f'{role}-fetch.txt','w')).returncode
run_id_file=OUT/'source-run-id.txt'; results['run_id_fetch']=run(['scp','-q','-o','BatchMode=yes','-i',KEY,'root@192.168.0.36:/tmp/ng050-source-run-id',str(run_id_file)],open(OUT/'run-id-fetch.txt','w')).returncode
if run_id_file.exists():
 run_id=run_id_file.read_text().strip(); results['source_staging_fetch']=run(['scp','-q','-r','-o','BatchMode=yes','-i',KEY,f'root@192.168.0.36:/root/workspace/novagenesis/IO/SourceStaging/{run_id}',str(OUT/'SourceStaging')],open(OUT/'staging-fetch.txt','w')).returncode
results['tcpdump_fetch']=run(['scp','-q','-o','BatchMode=yes','root@192.168.0.200:/tmp/ng050a-tcpdump.txt',str(OUT/'ng050a-tcpdump.txt')],open(OUT/'tcpdump-fetch.txt','w')).returncode
# Fetch data directories before cleanup.
for name,ip,path in [('nrncs-cache','192.168.0.36','/root/workspace/novagenesis/IO/NRNCS'),('repository-output','192.168.0.61','/root/workspace/novagenesis/IO/Repository1')]:
 results[name+'_fetch']=run(['scp','-q','-r','-o','BatchMode=yes','-i',KEY,f'root@{ip}:{path}',str(OUT/name)],open(OUT/f'{name}-fetch.txt','w')).returncode
# Hash reconciliation for JPEGs.
def hashes(p):
 d={}
 for f in pathlib.Path(p).rglob('*.jpg'):
  d[f.name]=hashlib.sha256(f.read_bytes()).hexdigest()
 return d
if (OUT/'SourceStaging').exists():
 src=hashes(OUT/'SourceStaging'); nr=hashes(OUT/'nrncs-cache'); rep=hashes(OUT/'repository-output')
 (OUT/'hash-reconciliation.txt').write_text(f'Source={len(src)} NRNCS={len(nr)} Repository={len(rep)}\nSource==NRNCS={src==nr}\nSource==Repository={src==rep}\nNRNCS==Repository={nr==rep}\n',encoding='utf-8')
# Cleanup and stop.
for role,ip in [('repo','192.168.0.61'),('source','192.168.0.36')]:
 c='cd /root/workspace/novagenesis && bash Scripts/Simple/clean.sh; printf "FINAL_PROCESSES\\n"; pgrep -a -f "(^|/)(PGCS|NRNCS|ContentApp|NBTestApp)( |$)" || true; printf "FINAL_SHM\\n"; ipcs -m; printf "FINAL_SEM\\n"; ipcs -s'
 results[role+'_cleanup']=run(ssh(ip,c),open(OUT/f'{role}-cleanup.txt','w')).returncode
results['vm_stop']=run(['ssh','-o','BatchMode=yes','-o','ConnectTimeout=10','root@192.168.0.200','qm stop 101; qm stop 102; qm status 101; qm status 102'],open(OUT/'vm-stop.txt','w')).returncode
with contract.open('a') as f:
 for k,v in results.items(): f.write(f'{k}_rc={v}\n')
 f.write(f'end_utc={stamp()}\n')
print(results); print(contract.read_text())
raise SystemExit(0)
