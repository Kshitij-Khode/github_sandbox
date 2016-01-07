#!/usr/bin/python
import subprocess, sys, re, template_script as ts

rc = {
    -1: '[FAIL] no args given',
    -2: '[FAIL] no device connected',
    -3: '[FAIL] error selecting device',
}

args = sys.argv[1:]
if args == []: exit(-1)
devices = ts.get_connected_devices()
if devices == []: exit(-2)
if len(args) == 1:
    if args[0] == 'shell':
        s_dev = ts.user_select_device(devices)
        ts.print_return('adb -s %s shell' % s_dev, ts.shell_call('adb -s %s shell' % s_dev)) if s_dev != '' else exit(-3)
    else:
        [ts.print_return('adb -s %s %s' % (d, args[0]), ts.shell('adb -s %s %s' % (d, args[0]))) for d in devices]
else:
    [ts.print_return(str(['adb', '-s', d] + args), ts.shell_list(['adb', '-s', d] + args)) for d in devices]