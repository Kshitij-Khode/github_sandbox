# coding: utf-8

from __future__ import division, absolute_import
from __future__ import print_function, unicode_literals

import numpy    as np
import random   as rand

import copy

def print_policy(policy, action_names):
    """Print the policy in human-readable format.

    Parameters
    ----------
    policy: np.ndarray
      Array of state to action number mappings
    action_names: dict
      Mapping of action numbers to characters representing the action.
    """
    str_policy = policy.astype('str')
    for action_num, action_name in action_names.items():
        np.place(str_policy, policy == action_num, action_name)

    print(str_policy)


def value_function_to_policy(env, gamma, value_function):
    """Output action numbers for each state in value_function.

    Parameters
    ----------
    env: gym.core.Environment
      Environment to compute policy for. Must have nS, nA, and P as
      attributes.
    gamma: float
      Discount factor. Number in range [0, 1)
    value_function: np.ndarray
      Value of each state.

    Returns
    -------
    np.ndarray
      An array of integers. Each integer is the optimal action to take
      in that state according to the environment dynamics and the
      given value function.
    """
    policy = np.zeros(env.nS, dtype='int')

    for sI in xrange(0, env.nS):
        oV = -1
        oA =  0
        for aI in xrange(0, env.nA):
            nV = 0
            for (prob, sprime, rew, term) in env.P[sI][aI]:
                nV += prob*(rew+(0 if term else gamma*value_function[sprime]))
            if nV >= oV:
                oV = nV
                oA = aI
        policy[sI] = oA

    return policy


def evaluate_policy_sync(env, gamma, policy, max_iterations=int(1e3), tol=1e-3):
    """Performs policy evaluation.

    Evaluates the value of a given policy.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    policy: np.array
      The policy to evaluate. Maps states to actions.
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, int
      The value for the given policy and the number of iterations till
      the value function converged.
    """
    value_func = np.zeros(env.nS)
    value_stor = np.zeros(env.nS)

    for iter in xrange(0, max_iterations):
        stop = True
        for sI in range(0, env.nS):
            [(prob, sprime, rew, term)] = env.P[sI][policy[sI]]
            value_stor[sI] = prob*(rew + (0 if term else gamma*value_func[sprime]))
            if abs(value_func[sI]-value_stor[sI]) > tol: stop = False


        value_func = copy.deepcopy(value_stor)
        if stop: return value_func, iter

    print('[DEBUG] @evaluate_policy_sync: did not converge')
    return value_func, max_iterations


def evaluate_policy_async_ordered(env, gamma, policy, max_iterations=int(1e3), tol=1e-3):
    """Performs policy evaluation.

    Evaluates the value of a given policy by asynchronous DP.  Updates states in
    their 1-N order.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    policy: np.array
      The policy to evaluate. Maps states to actions.
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, int
      The value for the given policy and the number of iterations till
      the value function converged.
    """
    value_func = np.zeros(env.nS)

    for iter in xrange(0, max_iterations):
        stop = True
        for sI in xrange(0, env.nS):
            [(prob, sprime, rew, term)] = env.P[sI][policy[sI]]
            nV             = prob*(rew+(0 if term else gamma*value_func[sprime]))
            if abs(value_func[sI]-nV) > tol: stop = False
            value_func[sI] = nV
        if stop: return value_func, iter

    print('[DEBUG] @evaluate_policy_async_ordered: did not converge')
    return value_func, max_iterations


def evaluate_policy_async_randperm(env, gamma, policy, max_iterations=int(1e3), tol=1e-3):
    """Performs policy evaluation.

    Evaluates the value of a policy.  Updates states by randomly sampling index
    order permutations.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    policy: np.array
      The policy to evaluate. Maps states to actions.
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, int
      The value for the given policy and the number of iterations till
      the value function converged.
    """
    value_func = np.zeros(env.nS)

    for iter in xrange(0, max_iterations):
        stop    = True
        rStates = range(0, env.nS)
        rand.shuffle(rStates)
        for sI in rStates:
            [(prob, sprime, rew, term)] = env.P[sI][policy[sI]]
            nV             = prob*(rew+(0 if term else gamma*value_func[sprime]))
            if abs(value_func[sI]-nV) > tol: stop = False
            value_func[sI] = nV
        if stop: return value_func, iter

    print('[DEBUG] @evaluate_policy_async_randperm: did not converge')
    return value_func, max_iterations


def evaluate_policy_async_custom(env, gamma, policy, max_iterations=int(1e3), tol=1e-3):
    """Performs policy evaluation.

    Evaluate the value of a policy. Updates states by a student-defined
    heuristic.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    policy: np.array
      The policy to evaluate. Maps states to actions.
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, int
      The value for the given policy and the number of iterations till
      the value function converged.
    """
    return np.zeros(env.nS), 0


def improve_policy(env, gamma, value_func, policy):
    """Performs policy improvement.

    Given a policy and value function, improves the policy.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    value_func: np.ndarray
      Value function for the given policy.
    policy: dict or np.array
      The policy to improve. Maps states to actions.

    Returns
    -------
    bool, np.ndarray
      Returns true if policy changed. Also returns the new policy.
    """
    npolicy = value_function_to_policy(env, gamma, value_func)

    if np.array_equal(npolicy, policy):
        return False, policy
    else:
        return True, npolicy


def policy_iteration_sync(env, gamma, max_iterations=int(1e3), tol=1e-3):
    """Runs policy iteration.

    See page 85 of the Sutton & Barto Second Edition book.

    You should use the improve_policy() and evaluate_policy_sync() methods to
    implement this method.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    (np.ndarray, np.ndarray, int, int)
       Returns optimal policy, value function, number of policy
       improvement iterations, and number of value iterations.
    """
    total_val_iter = 0
    policy         = np.zeros(env.nS, dtype='int')

    for iter in xrange(0,max_iterations):
        value_func, val_iter = evaluate_policy_sync(env, gamma, policy, max_iterations, tol)
        imp, policy          = improve_policy(env, gamma, value_func, policy)
        total_val_iter      += val_iter

        if not imp: return policy, value_func, iter, total_val_iter

    print('[DEBUG] @policy_iteration_sync: did not converge')
    return policy, value_func, max_iterations, 0



def policy_iteration_async_ordered(env, gamma, max_iterations=int(1e3),
                                   tol=1e-3):
    """Runs policy iteration.

    You should use the improve_policy and evaluate_policy_async_ordered methods
    to implement this method.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    (np.ndarray, np.ndarray, int, int)
       Returns optimal policy, value function, number of policy
       improvement iterations, and number of value iterations.
    """
    total_val_iter = 0
    policy         = np.zeros(env.nS, dtype='int')

    for iter in xrange(0,max_iterations):
        value_func, val_iter = evaluate_policy_async_ordered(env, gamma, policy, max_iterations, tol)
        imp, opolicy         = improve_policy(env, gamma, value_func, policy)
        total_val_iter      += val_iter
        policy               = opolicy

        if not imp: return opolicy, value_func, iter, total_val_iter

    print('[DEBUG] @policy_iteration_async_ordered: did not converge')
    return policy, value_func, 0, 0


def policy_iteration_async_randperm(env, gamma, max_iterations=int(1e3),
                                    tol=1e-3):
    """Runs policy iteration.

    You should use the improve_policy and evaluate_policy_async_randperm methods
    to implement this method.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    (np.ndarray, np.ndarray, int, int)
       Returns optimal policy, value function, number of policy
       improvement iterations, and number of value iterations.
    """
    total_val_iter = 0
    policy         = np.zeros(env.nS, dtype='int')

    for iter in xrange(0,max_iterations):
        value_func, val_iter = evaluate_policy_async_randperm(env, gamma, policy, max_iterations, tol)
        imp, opolicy         = improve_policy(env, gamma, value_func, policy)
        total_val_iter      += val_iter
        policy               = opolicy

        if not imp: return opolicy, value_func, iter, total_val_iter

    print('[DEBUG] @policy_iteration_async_randperm: did not converge')
    return policy, value_func, 0, 0


def policy_iteration_async_custom(env, gamma, max_iterations=int(1e3),
                                  tol=1e-3):
    """Runs policy iteration.

    You should use the improve_policy and evaluate_policy_async_custom methods
    to implement this method.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    (np.ndarray, np.ndarray, int, int)
       Returns optimal policy, value function, number of policy
       improvement iterations, and number of value iterations.
    """
    policy = np.zeros(env.nS, dtype='int')
    value_func = np.zeros(env.nS)

    return policy, value_func, 0, 0


def value_iteration_sync(env, gamma, max_iterations=int(1e3), tol=1e-3):
    """Runs value iteration for a given gamma and environment.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, iteration
      The value function and the number of iterations it took to converge.
    """
    value_func = np.zeros(env.nS)
    value_stor = np.zeros(env.nS)

    for iter in xrange(0, max_iterations):
        stop = True
        for sI in xrange(0, env.nS):
            oV = -1
            for aI in xrange(0, env.nA):
                nV = 0
                for (prob, sprime, rew, term) in env.P[sI][aI]:
                    nV += prob*(rew+(0 if term else gamma*value_func[sprime]))
                if nV >= oV: oV = nV
            value_stor[sI] = oV
            if abs(value_func[sI] - value_stor[sI]) > tol: stop = False
        value_func = copy.deepcopy(value_stor)
        if stop: return value_func, iter

    return value_func, max_iterations


def value_iteration_async_ordered(env, gamma, max_iterations=int(1e3), tol=1e-3):
    """Runs value iteration for a given gamma and environment.
    Updates states in their 1-N order.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, iteration
      The value function and the number of iterations it took to converge.
    """
    value_func = np.zeros(env.nS)

    for iter in xrange(0, max_iterations):
        stop = True
        for sI in xrange(0, env.nS):
            oV = -1
            for aI in xrange(0, env.nA):
                [(prob, sprime, rew, term)] = env.P[sI][aI]
                nV = prob*(rew+(0 if term else gamma*value_func[sprime]))
                if nV > oV: oV = nV
            if abs(value_func[sI] - oV) > tol: stop = False
            value_func[sI] = oV
        if stop: return value_func, iter

    return value_func, max_iterations



def value_iteration_async_randperm(env, gamma, max_iterations=int(1e3),
                                   tol=1e-3):
    """Runs value iteration for a given gamma and environment.
    Updates states by randomly sampling index order permutations.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, iteration
      The value function and the number of iterations it took to converge.
    """
    value_func = np.zeros(env.nS)

    for iter in xrange(0, max_iterations):
        stop    = True
        rStates = range(0, env.nS)
        rand.shuffle(rStates)
        for sI in rStates:
            oV = -1
            for aI in xrange(0, env.nA):
                [(prob, sprime, rew, term)] = env.P[sI][aI]
                nV = prob*(rew+(0 if term else gamma*value_func[sprime]))
                if nV > oV: oV = nV
            if abs(value_func[sI] - oV) > tol: stop = False
            value_func[sI] = oV
        if stop: return value_func, iter

    return value_func, max_iterations


def value_iteration_async_custom(env, gamma, max_iterations=int(1e3), tol=1e-3):
    """Runs value iteration for a given gamma and environment.
    Updates states by student-defined heuristic.

    Parameters
    ----------
    env: gym.core.Environment
      The environment to compute value iteration for. Must have nS,
      nA, and P as attributes.
    gamma: float
      Discount factor, must be in range [0, 1)
    max_iterations: int
      The maximum number of iterations to run before stopping.
    tol: float
      Determines when value function has converged.

    Returns
    -------
    np.ndarray, iteration
      The value function and the number of iterations it took to converge.
    """
    value_func = np.zeros(env.nS)

    nR, nC     = env.nS/2, env.nS/2
    for sI in range(0, env.nS):
        for aI in range(0, env.nA):
            for (prob, sprime, rew, term) in env.P[sI][aI]:
                if term and rew == 1:
                    gLoc = sprime
    gR, gC = int(gLoc/nR), gLoc%nC

    state_dist = {}
    for sI in range(0, env.nS):
        state_dist[sI] = abs(int(sI/nR)-gR)+abs((sI%nC)-gC)
    state_dist = sorted(state_dist.iteritems(), key=lambda(k,v):(v,k))

    for iter in xrange(0, max_iterations):
        stop = True
        for sI,dist in state_dist:
            oV = -1
            for aI in range(0, env.nA):
                nV = 0
                for (prob, sprime, rew, term) in env.P[sI][aI]:
                    nV += prob*(rew+(0 if term else gamma*value_func[sprime]))
                if nV > oV: oV = nV
            if abs(value_func[sI] - oV) > tol: stop = False
            value_func[sI] = oV
        if stop: return value_func, iter

    return value_func, max_iterations
