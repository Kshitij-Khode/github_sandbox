#!/usr/bin/python
import subprocess, sys, re

rc = {
    -1: '[FAIL] no args given',
    -2: '[FAIL] no device connected',
    -3: '[FAIL] error selecting device',
}

def shell(command):
    return subprocess.Popen(command.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT).communicate()[0]

def shell_list(command_list):
    return subprocess.Popen(command_list, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).communicate()[0]

def shell_call(command):
    return subprocess.call(command.split())

def get_connected_devices():
    return [ x for x in [x.split()[0].strip() for x in shell('adb devices').strip().split('\n')[1:] if re.search(r'\b%s\b' % re.escape('device'), x)] ]

def user_select_device(devices):
    if len(devices) == 1: return devices[0]
    for choice, dev in zip(range(len(devices)), devices): print 'device [%s]: %s' % (choice, dev)
    try: choice = int(raw_input('choose device: '))
    except KeyboardInterrupt: return ''
    except Exception: return ''
    return devices[choice] if choice > -1 and choice < len(devices) else ''  

def print_return(preprint, output):
    print preprint + '\n' + output
    return output

def exit(returncode):
    print rc[returncode]
    sys.exit(returncode)

args = sys.argv[1:]
if args == []: exit(-1)
devices = get_connected_devices()
if devices == []: exit(-2)
if len(args) == 1:
    if args[0] == 'shell':
        s_dev = user_select_device(devices)
        print_return('adb -s %s shell' % s_dev, shell_call('adb -s %s shell' % s_dev)) if s_dev != '' else exit(-3)
    else:
        [print_return('adb -s %s %s' % (d, args[0]), shell('adb -s %s %s' % (d, args[0]))) for d in devices]
else:
    [print_return(str(['adb', '-s', d] + args), shell_list(['adb', '-s', d] + args)) for d in devices]