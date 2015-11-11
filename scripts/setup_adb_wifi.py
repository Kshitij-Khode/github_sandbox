#!/usr/bin/python
import subprocess, sys, argparse, os

parser = argparse.ArgumentParser(description='sample usage: ./setup_adb_wifi.py --phones 172 173 174')

rc  =   {
    -1: '[FAIL] Could not remount all devices',
    -2: '[FAIL] Could not push install-recovery.sh to all devices',
    -3: '[FAIL] Could not push ap.conf to all devices',
    -4: '[FAIL] Could not push unlock_screen.sh to all devices',
    -5: '[FAIL] Could not push lock_screen.sh to all devices',
    -6: '[FAIL] Could not change install-recovery.sh permission on all devices',
    -7: '[FAIL] Could not change unlock_screen.sh permission on all devices',
    -8: '[FAIL] Could not change lock_screen.sh permission on all devices',
    -9: '[FAIL] Only connect the devices on which to setup adb over wifi',
    -10: '[FAIL] Could not change smartwakelock_teststartup.py permission on all devices',
    -11: '[FAIL] Could not install wakelock.apk on all devices'
}

sp  =   {
    'install-recovery.sh':
'''#!/system/bin/sh
$self_dir/smartwakelock_teststartup.py &
''',

    'ap.conf':
'''ctrl_interface=/data/misc/wifi/m87
update_config=1
network={
ssid="$SSID"
psk="$PWD"
key_mgmt=WPA-PSK
priority=1
}
''',

    'unlock_screen.sh':
'''#!/bin/bash
input keyevent 26
input keyevent 82
''',

    'lock_screen.sh':
'''#!/bin/bash
input keyevent 26
''',

    'smartwakelock_teststartup.py':
'''#!/system/bin/python
import subprocess, time, os

self_dir       = '$self_dir'
some_server    = '$some_server'
dev_name       = '$DEV_NAME'

def shell(command):
    return subprocess.Popen(command.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT).communicate()[0]

def shell_to_file(command, filepath):
    file = open(filepath, 'w')
    subprocess.Popen(command.split(), stdout=file, stderr=subprocess.STDOUT).communicate()

def get_unique_filename(filepath):
    suffix = 0
    filepath = '%s$SUFFIX' % filepath
    while(os.path.isfile(filepath.replace('$SUFFIX', str(suffix)))):
        suffix += 1
    return filepath.replace('$SUFFIX', str(suffix))

time.sleep(25)
shell('wpa_supplicant.sys -dd -B -iwlan0 -c%s/ap.conf' % self_dir)
shell('ip address add 192.168.31.%s/16 broadcast 192.168.255.255 dev wlan0' % dev_name)
time.sleep(10)
while True:
    time.sleep(180)
    if 'bytes from %s' % some_server not in shell('timeout -t 5 ping -c5 -i0.1 -w3 %s' % some_server):
        shell_to_file('logcat -d wpa_supplicant:V *:S', get_unique_filename('%s/logs/wpa_debug' % self_dir))
        shell('reboot')
'''
}

phone_dir = '$PHONE_DIR'

def add_list_arg(id, help, required):
    parser.add_argument('--%s' % id, help=help, nargs='+', default=[], required=required)

def parse_args():
    return parser.parse_args().__dict__

def shell(command):
    return subprocess.Popen(command.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT).communicate()[0]

def write_file(content, filepath):
    with open(filepath, 'w') as sfile:
        sfile.write(content)
        sfile.flush()

def get_connected_devices():
    return [ x for x in [x.split()[0].strip() for x in shell('adb devices').strip().split('\n')[1:] if 'device' in x] ]

def adb_push_string(content, filepath, devices):
    tmp_file = './adb_push_string.tmp'
    write_file(content, tmp_file)
    return_list = [d for d in devices if 'failed' in shell('adb -s %s push %s %s' % (d, tmp_file, filepath))]
    os.remove(tmp_file)
    return return_list

def exit(returncode):
    print rc[returncode]
    sys.exit(returncode)

add_list_arg('phones', 'phones to be setup for adb over wifi', True)
s_args = parse_args()
devices = get_connected_devices()
boot_script = sp['smartwakelock_teststartup.py'].replace('$some_server', s_args['ping'])
if [d for d in devices if 'remount succeeded' not in shell('adb -s %s remount' % d)]: exit(-1)
if adb_push_string(sp['install-recovery.sh'], '%s/install-recovery.sh' % phone_dir, devices) != []: exit(-2)
if adb_push_string(sp['ap.conf'], '%s/ap.conf' % phone_dir, devices) != []: exit(-3)
if adb_push_string(sp['unlock_screen.sh'], '%s/unlock_screen.sh' % phone_dir, devices) != []: exit(-4)
if adb_push_string(sp['lock_screen.sh'], '%s/lock_screen.sh' % phone_dir, devices) != []: exit(-5)
if [d for d in devices if 'Unable' in shell('adb -s %s shell chmod 777 %s/install-recovery.sh' % (d, phone_dir))]: exit(-6)
if [d for d in devices if 'Unable' in shell('adb -s %s shell chmod 777 %s/unlock_screen.sh' % (d, phone_dir))]: exit(-7)
if [d for d in devices if 'Unable' in shell('adb -s %s shell chmod 777 %s/lock_screen.sh' % (d, phone_dir))]: exit(-8)
if len(devices) != len(s_args['phones']): exit(-9)
for dev, lgd_id in zip(devices, s_args['phones']):
    boot_script = boot_script.replace('$DEV_NAME', lgd_id)
    adb_push_string(boot_script, '%s/smartwakelock_teststartup.py' % phone_dir, [dev])
    shell('adb -s %s shell echo service.adb.tcp.port=5555 >> /system/build.prop' % dev)
if [d for d in devices if 'Unable' in shell('adb -s %s shell chmod 777 %s/smartwakelock_teststartup.py' % (d, phone_dir))]: exit(-10)
if [d for d in devices if 'Success' not in shell('adb -s %s install ../wakelock.apk' % d)]: exit(-11)
