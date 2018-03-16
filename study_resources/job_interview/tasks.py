import numpy as np

def dropNDigitsLowestNo(input, n):
    if n == 0: return input

    for iter in xrange(0,n):
        for cI in xrange(1, len(input)):
            if int(input[cI-1]) > int(input[cI]): return dropNDigitsLowestNo(input[:cI-1]+input[cI:], n-1)

    return input[:-1]

def dropNDigitsLowestNoDriver():
    input = '12342895'
    print('dropNDigitsLowestNoDriver:%s' % dropNDigitsLowestNo(input, 3))

def fibonacciNum(n):
    if n <= 1: return n
    return fibonacciNum(n-1) + fibonacciNum(n-2);

def firstFibonacciDriver(n):
    print(fibonacciNum(n))

def randomWeighted(n_w_tuples):
    weightedList = []
    for (n, w) in n_w_tuples:
        for i in xrange(w):
            weightedList.append(n)
    return weightedList[int(np.random.choice(len(weightedList), 1))]

def randomWeightedDriver():
    print('weighted_rand: %s' % randomWeighted([(0, 10), (1, 20), (2, 30), (3, 40)]))

def main():
    # dropNDigitsLowestNoDriver()
    # firstFibonacciDriver(6)
    # randomWeightedDriver()

if __name__ == '__main__':
    main()