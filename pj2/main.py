import csv
import random
import operator
import copy
import threading
import time

TRAIN_RATIO = 0.8
NUM_OF_TREE = 5
EXTREME_RANDOM = True
TREE_DEPTH_LIMIT = 999
FILE_LIST = {"cross200.txt": "Cross 200", "ellipse100.txt": "Ellipse 100", "iris.txt": "Iris", "optical-digits.txt": "Optical Recognition of Handwritten Digits", "winequality-red.txt": "Wine"}

class Node:
    def __init__(self):
        self.left = None
        self.right = None
        self.data = None
        self.is_leaf = False
        self.attr = None

    def __repr__(self, depth=0):
        ret = "\t" * depth + repr(self.data) + " " + repr(self.attr) + "\n"
        if not self.is_leaf:
            ret += self.left.__repr__(depth + 1)
            ret += self.right.__repr__(depth + 1)
        return ret


def readFile(file_name):
    with open(file_name, newline='') as csvfile:
        tmp_list = list(csv.reader(csvfile))
        for item in tmp_list:
            for j in range(0, len(item)-1):
                item[j] = float(item[j])
        return tmp_list

def splitDataSet(data_set):
    random.shuffle(data_set)
    split_point = int(len(data_set) * TRAIN_RATIO)
    return data_set[:split_point], data_set[split_point:]

def calTimes(list_of_item):
    times = {}
    for item in list_of_item:
        if item in times:
            times[item] += 1
        else:
            times[item] = 1
    return times

def vote(list_of_item):
    times = calTimes(list_of_item)
    return max(times.items(), key=operator.itemgetter(1))[0]

def calGiniImpurity(left, right):
    left_times = calTimes([data[len(left[0])-1] for data in left])
    right_time = calTimes([data[len(right[0])-1] for data in right])
    left_gini = 1
    right_gini = 1
    for key, value in left_times.items():
        left_gini -= (value / len(left)) ** 2
    for key, value in right_time.items():
        right_gini -= (value / len(right)) ** 2
    return left_gini + right_gini

def selectThreshold(data_set, attr):
    impurity = 2
    is_leaf = False
    left_data_set = []
    right_data_set = []
    for i in range(0, len(data_set)-1):
        tmp_threshold = (data_set[i][attr] + data_set[i+1][attr]) / 2
        tmp_left = []
        tmp_right = []
        for data in data_set:
            if data[attr] < tmp_threshold:
                tmp_left.append(data)
            else:
                tmp_right.append(data)
        tmp_impurity = calGiniImpurity(tmp_left, tmp_right)
        if tmp_impurity < impurity:
            impurity = tmp_impurity
            left_data_set = tmp_left
            right_data_set = tmp_right
            threshold = tmp_threshold
    if impurity == 0:
        is_leaf = True
        threshold = vote([data[len(data_set[0])-1] for data in left_data_set] + [data[len(data_set[0])-1] for data in right_data_set])
    return threshold, left_data_set, right_data_set, is_leaf



def buildTree(node, data_set, attr_left, depth = 0):
    if len(attr_left) == 0 or depth >= TREE_DEPTH_LIMIT or len(data_set) <= 1:
        node.is_leaf = True
        node.data = vote([data[len(data_set[0])-1] for data in data_set])
        return
    if EXTREME_RANDOM:
        selected_attr = attr_left[random.randint(0, len(attr_left)-1)]
    else:
        selected_attr = attr[0]
    attr_left.remove(selected_attr)
    node.attr = selected_attr
    node.data, left_data_set, right_data_set, node.is_leaf = selectThreshold(data_set, selected_attr)

    if not node.is_leaf:
        node.left = Node()
        node.right = Node()
        if len(left_data_set) == 0:
            node.left = node.right
        else:
            left_thread = threading.Thread(target=buildTree,
                                           args=(node.left, left_data_set, copy.deepcopy(attr_left), depth + 1,))
            left_thread.start()
        if len(right_data_set) == 0:
            node.right == node.left
        else:
            right_thread = threading.Thread(target = buildTree, args = (node.right, right_data_set, copy.deepcopy(attr_left), depth + 1, ))
            right_thread.start()
        try:
            left_thread.join()
        except UnboundLocalError:
            pass
        try:
            right_thread.join()
        except UnboundLocalError:
            pass

def predict(root, data, predictions):
    if root.is_leaf:
        predictions.append(root.data)
        return
    if data[root.attr] < root.data:
        predict(root.left, data, predictions)
    else:
        predict(root.right, data, predictions)

def predictForest(roots, data, accuracy):
    predictions = []
    threads = []
    for i in range(0, len(roots)):
        threads.append(threading.Thread(target=predict, args=(roots[i], data, predictions,)))
        threads[i].start()
    for thread in threads:
        thread.join()
    if vote(predictions) == data[len(data)-1]:
        accuracy['correct'] += 1

def validate(roots, data_set):
    accuracy = {"total": 0, "correct": 0}
    threads = []
    for i in range(0, len(data_set)):
        accuracy['total'] += 1
        threads.append(threading.Thread(target=predictForest, args=(roots, data_set[i], accuracy, )))
        threads[i].start()
    for thread in threads:
        thread.join()

    return float(accuracy['correct'])/float(accuracy['total'])

def main(filename, accuracy):
    data_set = readFile(filename)
    train_data, test_data = splitDataSet(data_set)
    roots = []
    threads = []
    start_build = time.time()
    for i in range(0, NUM_OF_TREE):
        roots.append(Node())
        threads.append(threading.Thread(target=buildTree,
                                        args=(roots[i], train_data, [i for i in range(0, len(data_set[0]) - 1)],)))
        threads[i].start()
    for i in range(0, NUM_OF_TREE):
        threads[i].join()
    end_build = time.time()
    start_validate = time.time()
    accuracy[FILE_LIST[filename]] = validate(roots, test_data)
    end_validate = time.time()
    print(FILE_LIST[filename]+" Build time: ", end_build - start_build)
    print(FILE_LIST[filename]+" Validate time: ", end_validate - start_validate)

if __name__ == '__main__':
    threads = []
    accuracy = {}
    i = 0
    for key, value in FILE_LIST.items():
        threads.append(threading.Thread(target=main, args=(key, accuracy, )))
        threads[i].start()
        i += 1
    for thread in threads:
        thread.join()
    print("\n\n")
    print("Train-Test Ratio: ", TRAIN_RATIO)
    print("Number of tree in forest: ", NUM_OF_TREE)
    print("Extreme random: ", EXTREME_RANDOM)
    if TREE_DEPTH_LIMIT >= 64:
        tree_depth = 'Unlimited'
    else:
        tree_depth = TREE_DEPTH_LIMIT
    print("Tree depth limit: ", tree_depth)
    print("\n")
    average = 0
    for key, value in accuracy.items():
        average += value
        print(key, ": ", value)
    print("Average accuracy: ", average / len(accuracy))
