#!/usr//bin/python
#
# driver.py - The driver tests the correctness of the students
# arclab handin
#
from __future__ import division
import subprocess;
import re;
import os;
import sys;
import optparse;

if os.path.isdir('/afs/cs.cmu.edu/academic/class/18600-f17/gem5'):
    gem5_cmd = ("/afs/cs.cmu.edu/academic/class/18600-f17/gem5/build/X86/gem5.opt "
                "{cpu} "
                "--cmd={bin} "
                "--l1d_size=64kB "
                "--l1i_size=16kB "
                "--directory={out} "
                "--options={flags}"
               )
elif os.path.isdir('/usr/local/depot/gem5'):
    gem5_cmd = ("/usr/local/depot/gem5/build/X86/gem5.opt "
                "{cpu} "
                "--cmd={bin} "
                "--l1d_size=64kB "
                "--l1i_size=16kB "
                "--directory={out} "
                "--options={flags}"
               )

def test_gem5(cpu_model_path, binary, flags, out_dir, feature, params, rungem5=False):
    p = subprocess.Popen(" ".join(["./"+binary, flags]), 
            shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0]
    stdout_data = re.split('\n', stdout_data)
    params_dict = {}
    metric = None
    for line in stdout_data:
        if re.match("Verified!", line):
            if rungem5:
                p = subprocess.Popen(gem5_cmd.format(cpu=cpu_model_path,
                                                     bin="./"+binary,
                                                     flags=flags,
                                                     out=out_dir), 
                                     shell=True, stdout=subprocess.PIPE)
                stdout_naive_gem5 = p.communicate()[0]
                stats = open(os.path.join(out_dir, 'stats.txt'), 'r')
                for line in stats:
                    if re.match(feature, line):
                        metric = float(line.split()[1])
                        break
                config = open(os.path.join(out_dir, 'config.ini'), 'r')
                for line in config:
                    if not params: 
                        break
                    for p in params:
                        if re.match(p, line):
                            params_dict[p] = float(line.split('=')[1])
                            params.remove(p)
                return True, metric, params_dict
            else:
                return True, None, None
    return False, None, None

#
# main - Main function
#
def main():

    # Configure maxscores
    maxscore= {};
    maxscore['parta'] = 30
    maxscore['partb'] = 45

    # Parse the command line arguments 
    p = optparse.OptionParser()
    p.add_option("-A", action="store_true", dest="autograde", 
                 help="emit autoresult string for Autolab");
    p.add_option("-g", action="store_true", dest="rungem5", 
                 help="Run Part C (Gem5)");
    p.add_option("-s", action="store_true", dest="runstar", 
                 help="Run Part C (Gem5) Star Config.");

    opts, args = p.parse_args()
    autograde = opts.autograde
    rungem5 = opts.rungem5
    runstar = opts.runstar

    if not runstar and rungem5:
        gem5_out = ''
        print "Part C: Testing Naive Matrix Multiplication"
        mm_naive_correct, mm_naive_cpi, mm_naive_params = test_gem5('o3_mm.py', 
                'mm', '-nu', 
                'out/o3_mm_naive', 
                'system.cpu.cpi', 
                ['numROBEntries', 'numIQEntries'], 
                rungem5)
        
        print "Part C: Testing DAXPY"
        daxpy_correct, daxpy_cpi, daxpy_params = test_gem5('o3_daxpy.py', 
                'daxpy', '-u', 
                'out/o3_daxpy', 
                'system.cpu.cpi', 
                ['numROBEntries', 'numIQEntries'], 
                rungem5)
 
        if mm_naive_correct:
            gem5_out += "Naive MM CPI = {0}\n".format(mm_naive_cpi)
            cost = 0
            for k,v in mm_naive_params.iteritems():
                gem5_out += "{0} = {1}\n".format(k,v)
                cost += v
            gem5_out += "Total Cost (C) = %d\n" % (cost)
            gem5_out += "CPI x C^{1/3} = %.4f\n" % (mm_naive_cpi * cost**(1/3))
        else:
            gem5_out += "Naive MM not verified.\n"

        gem5_out += "\n"

        if daxpy_correct:
            gem5_out += "DAXPY CPI = {0}\n".format(daxpy_cpi)
            cost = 0
            for k,v in daxpy_params.iteritems():
                gem5_out += "{0} = {1}\n".format(k,v)
                cost += v
            gem5_out += "Total Cost (C) = %d\n" % (cost)
            gem5_out += "CPI x C^{1/3} = %.4f" % (daxpy_cpi * cost**(1/3))
        else:
            gem5_out += "DAXPY not verified.\n"
        
        print gem5_out     
        return
 
    if runstar and rungem5:
        gem5_out = ''
        print "Part C: Testing Naive Matrix Multiplication With Star Config."
        mm_naive_correct, mm_naive_cpi, mm_naive_params = test_gem5('o3_star.py', 
                'mm', '-nu', 
                'out/o3_mm_naive', 
                'system.cpu.cpi', 
                ['numROBEntries', 'numIQEntries'], 
                rungem5)
        
        print "Part C: Testing DAXPY with Star Config."
        daxpy_correct, daxpy_cpi, daxpy_params = test_gem5('o3_star.py', 
                'daxpy', '-u', 
                'out/o3_daxpy', 
                'system.cpu.cpi', 
                ['numROBEntries', 'numIQEntries'], 
                rungem5)
 
        if mm_naive_correct:
            gem5_out += "Naive MM CPI = {0}\n".format(mm_naive_cpi)
            cost = 0
            for k,v in mm_naive_params.iteritems():
                gem5_out += "{0} = {1}\n".format(k,v)
                cost += v
            if cost < 48 or cost > 56:
                print 'Invalid cost; should be 48 <= C <= 56, exiting!'
                sys.exit(0)
            gem5_out += "Total Cost (C) = %d\n" % (cost)
            gem5_out += "CPI x C^{1/3} = %.4f\n" % (mm_naive_cpi * cost**(1/3))
        else:
            gem5_out += "Naive MM not verified.\n"

        gem5_out += "\n"

        if daxpy_correct:
            gem5_out += "DAXPY CPI = {0}\n".format(daxpy_cpi)
            cost = 0
            for k,v in daxpy_params.iteritems():
                gem5_out += "{0} = {1}\n".format(k,v)
                cost += v
            if cost < 48 or cost > 56:
                print 'Invalid cost; should be 48 <= C <= 56, exiting!'
                sys.exit(0)
            gem5_out += "Total Cost (C) = %d\n" % (cost)
            gem5_out += "CPI x C^{1/3} = %.4f" % (daxpy_cpi * cost**(1/3))
        else:
            gem5_out += "DAXPY not verified.\n"
        
        print gem5_out     
        return
       
    saved_dir = os.getcwd()
    # Check the correctness of part A
    print "Testing part A"
    try:
        os.chdir("test/parta")
        p = subprocess.Popen(
            "./grade-archlaba.pl -f ../../sim/misc -s ../../sim -e",
            shell=True, stdout=subprocess.PIPE)
        stdout_data = p.communicate()[0]
        os.chdir(saved_dir)
    except:
        print "An exception occurred when testing Part A. Skipping this part."
        stdout_data = ""

    # Emit the output from part A autograder
    results_parta = 0
    stdout_data = re.split('\n', stdout_data)
    for line in stdout_data:
        if re.match("PARTA_SCORE", line):
            results_parta = int(re.findall(r'(\d+)', line)[0])
        else:
            print "%s" % (line)
    
    # Check the correctness of part B
    print "Testing part B"
    try:
        os.chdir("test/partb")
        p = subprocess.Popen(
            "./grade-archlabb.pl -f ../../sim/pipe -s ../../sim -e",
            shell=True, stdout=subprocess.PIPE)
        stdout_data = p.communicate()[0]
        os.chdir(saved_dir)
    except:
        print "An exception occurred when testing Part B. Skipping this part."
        stdout_data = ""

    # Emit the output from part B autograder
    results_partb = 0
    stdout_data = re.split('\n', stdout_data)
    for line in stdout_data:
        if re.match("PARTB_SCORE", line):
            results_partb = int(re.findall(r'(\d+)', line)[0])
        else:
            print "%s" % (line)
    
    total_score = results_parta + results_partb #+ results_partc

    total_score = results_parta + results_partb

    # Summarize the results
    print "\nArclab Summary:"
    print "%-22s%8s%12s" % ("", "Points", "Max Points") 
    print "%-22s%8d%12d" % ("Part A", results_parta, maxscore['parta'])
    print "%-22s%8d%12d" % ("Part B", results_partb, maxscore['partb'])
    print "%-22s%8d" % ("Total Autograded Points", total_score)
                              #maxscore['parta'],
                              #maxscore['partb'])
    
    # Emit autoresult string for Autolab if called with -A option
    if autograde:
        autoresult="{\"scores\": {\"Part A\": %.1f, \"Part B\": %.1f}, \"scoreboard\": [%d, %d, %d]}" % (results_parta, results_partb, total_score, results_parta, results_partb)
        print "%s" % autoresult


if __name__ == "__main__":
  main()

