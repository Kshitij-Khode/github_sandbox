import gym, time
import deeprl_hw1.lake_envs as lake_env
import deeprl_hw1.rl        as rl
import matplotlib.pylab     as plt
import numpy                as np
import sys

action_lookup = ['L','D','R','U']

def printPolicy(policy):
    (rN,cN) = policy.shape
    for rI in xrange(0, rN):
        for cI in xrange(0, cN):
            sys.stdout.write('%s' % action_lookup[policy[rI][cI]])
            sys.stdout.flush()
        print('')

def runPolicy(env, gamma, value_func, policy, start, max_step=int(1e3)):
    env.reset()
    tR, step = 0, 0
    nS       = start
    for step in xrange(0,max_step):
        nS, rew, term, debug_info = env.step(policy[nS])
        tR   = rew*pow(gamma,step)
        if term: return tR, step

    return tR, max_step



def main():
    denv4 = gym.make('Deterministic-4x4-FrozenLake-v0')
    denv8 = gym.make('Deterministic-8x8-FrozenLake-v0')
    senv4 = gym.make('Stochastic-4x4-FrozenLake-v0')
    senv8 = gym.make('Stochastic-8x8-FrozenLake-v0')
    gamma = 0.9
    policy_iter_fstr = \
'''
%s: %s
  time_elapsed: %s
  pol_iter: %s
  total_val_iter: %s
  start_val: %s
'''
    value_iter_fstr = \
'''
%s: %s
  time_elapsed: %s
  iter: %s
  start_val: %s
'''

    denv4.render()
    denv8.render()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_sync(denv4, gamma)
    print(policy_iter_fstr % ('policy_iteration_sync',
                        denv4,
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter,
                        value_func[3]))
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,4,0.1,4))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_sync(denv8, gamma)
    print(policy_iter_fstr % ('policy_iteration_sync',
                        denv8,
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter,
                        value_func[3]))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,8,0.1,8))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_sync(denv4, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        denv4,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy    = rl.value_function_to_policy(denv4, gamma, value_func)
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,4,0.1,4))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_sync(denv8, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        denv8,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(denv8, gamma, value_func)
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,8,0.1,8))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_async_ordered(denv8, gamma)
    print(policy_iter_fstr % ('policy_iteration_async_ordered',
                        denv8,
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter,
                        value_func[3]))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,8,0.1,8))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_async_randperm(denv8, gamma)
    print(policy_iter_fstr % ('policy_iteration_async_randperm',
                        denv8,
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter,
                        value_func[3]))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,8,0.1,8))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_ordered(denv4, gamma)
    print(value_iter_fstr % ('value_iteration_async_ordered',
                        denv4,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy    = rl.value_function_to_policy(denv4, gamma, value_func)
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,4,0.1,4))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_ordered(denv8, gamma)
    print(value_iter_fstr % ('value_iteration_async_ordered',
                        denv8,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(denv8, gamma, value_func)
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,8,0.1,8))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_randperm(denv4, gamma)
    print(value_iter_fstr % ('value_iteration_async_randperm',
                        denv4,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy    = rl.value_function_to_policy(denv4, gamma, value_func)
    print('rew: %s, steps: %s' % runPolicy(denv4, gamma, value_func, opolicy, 3))
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,4,0.1,4))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_randperm(denv8, gamma)
    print(value_iter_fstr % ('value_iteration_async_randperm',
                        denv8,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(denv8, gamma, value_func)
    print('rew: %s, steps: %s' % runPolicy(denv8, gamma, value_func, opolicy, 3))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,8,0.1,8))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_sync(senv4, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        senv4,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(senv4, gamma, value_func)
    avg_irew  = 0
    avg_steps = 0
    for _ in xrange(0, 100):
        rew, steps = runPolicy(senv4, gamma, value_func, opolicy, 3)
        avg_irew  += rew
        avg_steps += steps
    avg_irew  /= 100
    avg_steps /= 100
    print('avg_rew: %s, avg_steps: %s' % (avg_irew, avg_steps))
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,4,0.1,4))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_sync(senv8, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        senv8,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(senv8, gamma, value_func)
    avg_irew  = 0
    avg_steps = 0
    for i in xrange(0, 100):
        rew, steps = runPolicy(senv8, gamma, value_func, opolicy, 3)
        avg_irew  += rew
        avg_steps += steps
    avg_irew  /= 100
    avg_steps /= 100
    print('avg_rew: %s, avg_steps: %s' % (avg_irew, avg_steps))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    plt.imshow(value_func,                  \
               interpolation='nearest',     \
               cmap=plt.cm.ocean,           \
               extent=(0.1,8,0.1,8))
    plt.colorbar()
    plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_custom(denv4, gamma)
    print(value_iter_fstr % ('value_iteration_async_custom',
                        denv4,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(denv4, gamma, value_func)
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_custom(denv8, gamma)
    print(value_iter_fstr % ('value_iteration_async_custom',
                        denv8,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(denv8, gamma, value_func)
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_custom(senv4, gamma)
    print(value_iter_fstr % ('value_iteration_async_custom',
                        senv4,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(senv4, gamma, value_func)
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_custom(senv8, gamma)
    print(value_iter_fstr % ('value_iteration_async_custom',
                        senv8,
                        time.time()-t_start,
                        iter,
                        value_func[3]))
    opolicy = rl.value_function_to_policy(senv8, gamma, value_func)
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)


if __name__ == '__main__':
    main()
