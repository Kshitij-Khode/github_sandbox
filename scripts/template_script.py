#!/usr/bin/python
import os, subprocess, argparse, sys

parser = argparse.ArgumentParser(description='$CHANGE_HELP_MSG')

rc = {
    -1: '[FAIL] $CHANGE_FAIL_MSG'
}

sp = {
    'install-recovery.sh':
'''#!/system/bin/sh
/data/QA/scripts/power_management/boot_force_sta_connect.py &
'''
}

def add_list_arg(id, help, required):
    parser.add_argument('--%s' % id, help=help, nargs='+', default=[], required=required)
    
def add_string_arg(id, help, required):
    parser.add_argument('--%s' % id, help=help, type=str, default='', required=required)

def parse_args():
    return parser.parse_args().__dict__

def shell(command):
    return subprocess.Popen(command.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT).communicate()[0]

def shell_list(command_list):
    return subprocess.Popen(command_list, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).communicate()[0]

def shell_script(command, filepath):
    write_file('#!/bin/bash\n%s' % command, filepath)
    out = shell('sh %s' % filepath)
    os.remove(filepath)
    return out

def shell_to_file(command, filepath):
    with open(filepath, 'w') as file: subprocess.Popen(command.split(), stdout=file, stderr=subprocess.STDOUT).communicate()

def shell_call(command):
    return subprocess.call(command.split())

def write_file(content, filepath):
    with open(filepath, 'w') as sfile:
        sfile.write(content)
        sfile.close()

def get_unique_filename(filepath):
    suffix = 0
    filepath = '%s$SUFFIX' % filepath
    while(os.path.isfile(filepath.replace('$SUFFIX', str(suffix)))):
        suffix += 1
    return filepath.replace('$SUFFIX', str(suffix))

def get_connected_devices():
    return [ x for x in [x.split()[0].strip() for x in shell('adb devices').strip().split('\n')[1:] if 'device' in x] ]

def user_select_device(devices):
    if len(devices) == 1: return devices[0]
    for choice, dev in zip(range(len(devices)), devices): print 'device [%s]: %s' % (choice, dev)
    try: choice = int(raw_input('choose device: '))
    except Exception: return ''
    return devices[choice] if choice > -1 and choice < len(devices) else ''  

def adb_push_string(content, filepath, devices):
    tmp_file = './adb_push_string.tmp'
    write_file(content, tmp_file)
    return_list = [d for d in devices if 'failed' in shell('adb -s %s push %s %s' % (d, tmp_file, filepath))]
    os.remove(tmp_file)
    return return_list

def print_return(preprint, output):
    print preprint + '\n' + output
    return output

def exit(returncode):
    print rc[returncode]
    sys.exit(returncode)

# def remount_devices(devices):
#     return [d for d in devices if 'remount succeeded' not in shell('adb -s %s remount' % d)]
