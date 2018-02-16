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

def main():
    denv4 = gym.make('Deterministic-4x4-FrozenLake-v0')
    denv8 = gym.make('Deterministic-8x8-FrozenLake-v0')
    gamma = 0.9
    policy_iter_fstr = \
'''
%s:
  time_elapsed: %s
  pol_iter: %s
  total_val_iter: %s
'''
    value_iter_fstr = \
'''
%s:
  time_elapsed: %s
  iter: %s
'''

    denv4.render()
    denv8.render()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_sync(denv4, gamma)
    print(policy_iter_fstr % ('policy_iteration_sync',
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter))
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,4,0.1,4))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_sync(denv8, gamma)
    print(policy_iter_fstr % ('policy_iteration_sync',
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,8,0.1,8))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_sync(denv4, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        time.time()-t_start,
                        iter))
    opolicy    = rl.value_function_to_policy(denv4, gamma, value_func)
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,4,0.1,4))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_sync(denv8, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        time.time()-t_start,
                        iter))
    opolicy = rl.value_function_to_policy(denv8, gamma, value_func)
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,8,0.1,8))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_async_ordered(denv8, gamma)
    print(policy_iter_fstr % ('policy_iteration_async_ordered',
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,8,0.1,8))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (opolicy, value_func, pol_iter, total_val_iter) = rl.policy_iteration_async_randperm(denv8, gamma)
    print(policy_iter_fstr % ('policy_iteration_async_randperm',
                        time.time()-t_start,
                        pol_iter,
                        total_val_iter))
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,8,0.1,8))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_ordered(denv4, gamma)
    print(value_iter_fstr % ('value_iteration_async_ordered',
                        time.time()-t_start,
                        iter))
    opolicy    = rl.value_function_to_policy(denv4, gamma, value_func)
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,4,0.1,4))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_ordered(denv8, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        time.time()-t_start,
                        iter))
    opolicy = rl.value_function_to_policy(denv8, gamma, value_func)
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,8,0.1,8))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_randperm(denv4, gamma)
    print(value_iter_fstr % ('value_iteration_async_ordered',
                        time.time()-t_start,
                        iter))
    opolicy    = rl.value_function_to_policy(denv4, gamma, value_func)
    opolicy    = opolicy.reshape((4,4))
    value_func = value_func.reshape((4,4))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,4,0.1,4))
    # plt.colorbar()
    # plt.show()

    t_start = time.time()
    (value_func, iter) = rl.value_iteration_async_randperm(denv8, gamma)
    print(value_iter_fstr % ('value_iteration_sync',
                        time.time()-t_start,
                        iter))
    opolicy = rl.value_function_to_policy(denv8, gamma, value_func)
    opolicy    = opolicy.reshape((8,8))
    value_func = value_func.reshape((8,8))
    printPolicy(opolicy)
    # plt.imshow(value_func,                  \
    #            interpolation='nearest',     \
    #            cmap=plt.cm.ocean,           \
    #            extent=(0.1,8,0.1,8))
    # plt.colorbar()
    # plt.show()

if __name__ == '__main__':
    main()
