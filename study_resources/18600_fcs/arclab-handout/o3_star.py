"""
o3_star.py

kkhode

Large model with CPI=0.777(mm)/2.271(daxpy),C=42(35,17) & E=2.9003+8.477=11.3773, is best CPU model (lowest E)
Explaination: 
1. One thing to consider is increasing capacity to store instruction in queues without increasing
   capability to appropriately bookkeep those instruction does not make sense. Thus all possibilities
   of increasing IQ > ROB are ruled out.
2. Another thing to consider is that generally ROB bookkeeping generally involves storing instruction 
   addresses and rename registers, which diverges from simpler 1 bit bookkeeping fields such as "Busy", 
   "Finished" etc. This results in ROB requiring greater space to store per instruction than the
   instruction size itself. Therefore, increments of IQ == ROB are also ruled out.
3. Now for IQ < ROB values, if we provide equal wieghtage, the greater amount of multiplication
   increases the importance of ROB due to longer execution time of multiplication. As a result,
   in cases like ROB=35,IQ=17 vs ROB=35,IQ=18, although we were able to address bottleneck of 
   system.cpu.rename.IQFullEvents in comparsion to system.cpu.rename.ROBFullEvents, the lack of ROB
   increase to accomodate increased IQ got penalized in mm. Thus although CPI speedup was seen in daxpy
   CPI slowed down much more in mm.
4. The last thing to consider was, everything upwards of increase in ROB of 3, received only a minor
   speedup which wasn't able to compensate for the increasing cost of adding more hardware.

Thus the optimal E = E(mm) + E(daxpy) is found at ROB=35,IQ=17

"""

"""
Source -- https://www.cs.virginia.edu/~cr4bd/6354/F2016/homework2.html

This script executes a program with gem5's system call emulation mode.

Basic usage:
    path/to/gem5.opt ./this-script.py --cmd=./path/to/executable 
            --directory=output-directory

After running the above command, output from the simulation will be placed in
output-directory including these files:
    config.ini, config.json --- the full configuration of all the internal components of
                                the simulation.

    program.out, program.err --- the output of the executable program

    stats.txt --- statistics from the simulation, such as the number of instructions
                  executed, the cache hit ratios, etc., etc.

To customize the simulation, you can edit the create_cpu() function below to 
change the configurations of its CPUs. This function takes as input the index
of the simulation, so you can easily sweep across parameters.

If you'd like to add command-line parameters to this script, you can do so by editing
the get_options() command to add another call to parser.add_option(). The parsed
command-line parameters will be available create_cpu() via the options variable.

"""

import optparse
import os
import os.path
import sys

if os.path.isdir('/afs/cs.cmu.edu/academic/class/18600-f17/gem5'):
    GEM5_DIR = '/afs/cs.cmu.edu/academic/class/18600-f17/gem5'
elif os.path.isdir('/usr/local/depot/gem5'):
    GEM5_DIR = '/usr/local/depot/gem5'

# Interface to m5 simulation, implementing in gem5/src
import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal

addToPath(os.path.join(GEM5_DIR, 'configs'))

# Utilities included with m5 for configuring common simulations
# from gem5/configs/common
from common import Options
from common import Simulation
from common.Caches import *
from common import MemConfig

def eprint(*args):
    sys.stderr.write("".join(args))
    sys.stderr.write("\n")

"""
Main function. This is called once at the very bottom of the script.
"""
def main(options):
    process = create_process(options)
    run_one_simulation(options, process)

"""
Create a virtual CPU for the simulation.

To set which functional units (i.e. pipelines for execution) the processor
has, see the 'fuPool' parameter to the CPU. You can see the definition of 
DefaultFUPool, the default setting for this parameter, in src/cpu/o3/FUPool.py.
This uses definitions of the available functional units shown in
src/cpu/o3/FuncUnitConfig.py.
"""
def create_cpu(options, cpu_id):
    # DerivO3CPU is the configurable out-of-order CPU model supplied by gem5
    the_cpu = DerivO3CPU(cpu_id=cpu_id)
    icache = L1_ICache(size=options.l1i_size, assoc=options.l1i_assoc)
    dcache = L1_DCache(size=options.l1d_size, assoc=options.l1d_assoc)
    the_cpu.branchPred = LocalBP()
    
    # Parameter values
    the_cpu.numROBEntries = 35
    the_cpu.numIQEntries = 17
    
    the_cpu[cpu_id].addPrivateSplitL1Caches(icache, dcache, None, None)
    the_cpu[cpu_id].createInterruptController()
    return the_cpu

"""
Create a representation of the program to run for gem5. The
supplied version expects you to run this script with --cmd
specifying the executable to run and --options specifying
the command line arguments it will receive via argv.
"""
def create_process(options):
    process = Process()
    # use realpath to change something like ./naive-matmul into
    # something like /if22/cr4bd/naive-matmul
    process.executable = os.path.realpath(options.cmd)
    if options.options != "":
        process.cmd = [options.cmd] + options.options.split()
    else:
        process.cmd = [options.cmd]

    if options.input != "":
        process.input = options.input
    if options.output != "":
        process.output = options.output
    if options.errout != "":
        process.errout = options.errout
    
    return process



"""
Based on options, fork a child process and run one simulation in it.
Uses the create_cpu function above.
"""
def run_one_simulation(options, process):
    the_dir = os.path.join(options.directory)
    if not os.path.exists(the_dir):
        os.makedirs(the_dir)
    pid = os.fork()
    if pid == 0:
        # in child
        os.chdir(the_dir)
        run_system_with_cpu(process, options, os.path.realpath("."),
            real_cpu_create_function=lambda cpu_id: create_cpu(options, cpu_id)
        )
        sys.exit(0)
    else:
        # in parent
        exited_pid, exit_status = os.waitpid(pid, 0)
        # Check whether child reached exit(0)
        if os.WIFEXITED(exit_status) and os.WEXITSTATUS(exit_status) != 0:
            eprint("Child did not exit normally")

"""
Setup a simulation to run the specified program, writing output to the specified output directory.

Optionally supports running with a different (faster) CPU for the first instrunctions.
"""
def run_system_with_cpu(
        process, options, output_dir,
        warmup_cpu_class=None,
        warmup_instructions=0,
        real_cpu_create_function=lambda cpu_id: DerivO3CPU(cpu_id=cpu_id),
):
    # Override the -d outdir --outdir option to gem5
    m5.options.outdir = output_dir
    m5.core.setOutputDir(m5.options.outdir)

    m5.stats.reset()

    max_tick = options.abs_max_tick
    if options.rel_max_tick:
        max_tick = options.rel_max_tick
    elif options.maxtime:
        max_tick = int(options.maxtime * 1000 * 1000 * 1000 * 1000)

    eprint("Simulating until tick=%s" %  (max_tick))

    real_cpus = [real_cpu_create_function(0)]
    mem_mode = real_cpus[0].memory_mode()

    if warmup_cpu_class:
        warmup_cpus = [warmup_cpu_class(cpu_id=0)]
        warmup_cpus[0].max_insts_any_thread = warmup_instructions
    else:
        warmup_cpus = real_cpus

    system = System(cpu = warmup_cpus,
                    mem_mode = mem_mode,
                    mem_ranges = [AddrRange(options.mem_size)],
                    cache_line_size = options.cacheline_size)
    system.multi_thread = False
    system.voltage_domain = VoltageDomain(voltage = options.sys_voltage)
    system.clk_domain = SrcClockDomain(clock =  options.sys_clock,
                                       voltage_domain = system.voltage_domain)
    system.cpu_voltage_domain = VoltageDomain()
    system.cpu_clk_domain = SrcClockDomain(clock = options.cpu_clock,
                                           voltage_domain =
                                           system.cpu_voltage_domain)
    system.cache_line_size = options.cacheline_size
    if warmup_cpu_class:
        for cpu in real_cpus:
            cpu.clk_domain = system.cpu_clk_domain
            cpu.workload = process
            cpu.system = system
            cpu.switched_out = True
            cpu.createThreads()
        system.switch_cpus = real_cpus

    for cpu in system.cpu:
        cpu.clk_domain = system.cpu_clk_domain
        cpu.workload = process
        if options.prog_interval:
            cpu.progress_interval = options.prog_interval
        cpu.createThreads();

    MemClass = Simulation.setMemClass(options)
    system.membus = SystemXBar()
    system.system_port = system.membus.slave
    system.cpu[0].connectAllPorts(system.membus)
    MemConfig.config_mem(options, system)
    root = Root(full_system = False, system = system)

    m5.options.outdir = output_dir
    m5.instantiate(None) # None == no checkpoint
    if warmup_cpu_class:
        eprint("Running warmup with warmup CPU class (%d instrs.)" % (warmup_instructions))
    eprint("Starting simulation")
    exit_event = m5.simulate(max_tick)
    if warmup_cpu_class:
        max_tick -= m5.curTick()
        m5.stats.reset()
        debug_print("Finished warmup; running real simulation")
        m5.switchCpus(system, real_cpus)
        exit_event = m5.simulate(max_tick)
    eprint("Done simulation @ tick = %s: %s" % (m5.curTick(), exit_event.getCause()))
    m5.stats.dump()

"""Retrieve command-line options"""
def get_options():
    parser = optparse.OptionParser()
    Options.addCommonOptions(parser)
    Options.addSEOptions(parser)

    # base output directory to use.
    #
    # This takes precedence over gem5's built-in outdir option
    parser.add_option("--directory", type="str", default=None)

    parser.set_defaults(
        # Default to writing to program.out in the current working directory
        # below, we cd to the simulation output directory
        output='./program.out',
        errout='./program.err',

        mem_size=64 * 1024 * 1024,

        caches = True
    )

    (options, args) = parser.parse_args()

    # Always enable caches, DerivO3CPU will not work without it.

    if not options.directory:
        eprint("You must set --directory to the name of an output directory to create")
        sys.exit(1)

    if os.path.exists(options.directory):
        eprint("Output directory %s already exists -- refusing to overwrite" % (options.directory))
        sys.exit(1)

    # Some features are not supported by this script, but are added to parser by
    # Options.addSEOptions and Options.addCommonOptions

    # I check for these here to avoid confusion
    # If you are failing an assertion here, removing the assertion will not make the option work.
    assert(not options.smt)
    assert(options.num_cpus == 1)
    assert(not options.fastmem)
    assert(not options.standard_switch)
    assert(not options.repeat_switch)
    assert(not options.take_checkpoints)
    assert(not options.fast_forward)
    assert(not options.maxinsts)
    assert(not options.l2cache)

    if args:
        print "Error: script doesn't take any positional arguments"
        sys.exit(1)

    return options

main(get_options())