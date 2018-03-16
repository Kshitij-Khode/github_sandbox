class Node:
    def __init__(self,key):
        self.left  = None
        self.right = None
        self.val   = key

def insert(root,node):
    if node.val <= root.val:
        if root.left is None:
            root.left = node
            return 1
        else: return 1 + insert(root.left, node)
    else:
        if root.right is None:
            root.right = node
            return 1
        else: return 1 + insert(root.right, node)

    return 0

def printInorder(node):
    if node != None:
        printInorder(node.left)
        print('printInorder:%s' % node.val)
        printInorder(node.right)

def printPreorder(node):
    if node != None:
        print('printPreorder:%s' % node.val)
        printPreorder(node.left)
        printPreorder(node.right)

def printPostorder(node):
    if node != None:
        printPostorder(node.left)
        printPostorder(node.right)
        print('printPostorder:%s' % node.val)

def binaryTreeDriver():
    keys = [1, 2, 3]
    root = Node(keys[0])
    cnt  = 1

    print(cnt)
    for key in keys[1:]:
        cnt += insert(root,Node(key))
        print(cnt)

    printInorder(root)
    printPreorder(root)
    printPostorder(root)

def main():
    binaryTreeDriver()

if __name__ == '__main__':
    main()
