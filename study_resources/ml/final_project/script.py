import pandas as pd, matplotlib.pyplot as plt, numpy as np, itertools as it

from   sklearn.preprocessing    import StandardScaler     as stdScaler
from   sklearn.cross_validation import train_test_split   as cvSplit
from   sklearn.linear_model     import LogisticRegression as lReg
from   sklearn.cross_validation import KFold              as kFold
from   sklearn.cross_validation import cross_val_score    as cValScore
from   sklearn.metrics          import confusion_matrix   as confMat
from   sklearn.metrics          import precision_recall_curve as precRecallCurve
from   sklearn.metrics          import auc                    as auc
from   sklearn.metrics          import roc_auc_score          as rocAucScore
from   sklearn.metrics          import roc_curve              as rocCurve
from   sklearn.metrics          import recall_score           as recallScore
from   sklearn.metrics          import classification_report  as classificationReport

def drawImblalancedTrainingSet(countClasses, plotHelper):
    countClasses.plot(kind='bar')
    plotHelper.title("Fraud class histogram")
    plotHelper.xlabel("Class")
    plotHelper.ylabel("Frequency")

def drawConfusionMatrix(yData, yPred, plotHelper=plt, verbose=False):
    cnfMatrix = confMat(yData, yPred)
    np.set_printoptions(precision=2)
    if verbose: print("Recall metric in the testing dataset: ", cnfMatrix[1,1]/(cnfMatrix[1,0]+cnfMatrix[1,1]))
    classNames = [0,1]
    plotHelper.figure()
    plotConfusionMatrix(cnfMatrix, classes=classNames, title='Confusion Matrix')

def drawRocCurve(yData, yPredScore):
    fpr, tpr, thresholds  = rocCurve(yData.values.ravel(), yPredScore)
    rocAuc                = auc(fpr, tpr)
    plt.title('Receiver Operating Characteristic')
    plt.plot(fpr, tpr, 'b', label='AUC = %0.2f' % rocAuc)
    plt.legend(loc='lower right')
    plt.plot([0,1], [0,1], 'r--')
    plt.xlim([-0.1, 1.0])
    plt.ylim([-0.1, 1.01])
    plt.ylabel('True Positive Rate')
    plt.xlabel('False Positive Rate')

def printing_Kfold_scores(x_train_data,y_train_data):
    fold = kFold(len(y_train_data),5,shuffle=False)

    # Different C parameters
    c_param_range = [0.01,0.1,1,10,100]

    results_table = pd.DataFrame(index = range(len(c_param_range),2), columns = ['C_parameter','Mean recall score'])
    results_table['C_parameter'] = c_param_range

    # the k-fold will give 2 lists: train_indices = indices[0], test_indices = indices[1]
    j = 0
    for c_param in c_param_range:
        print('-------------------------------------------')
        print('C parameter: ', c_param)
        print('-------------------------------------------')
        print('')

        recall_accs = []
        for iteration, indices in enumerate(fold,start=1):

            # Call the logistic regression model with a certain C parameter
            lr = lReg(C = c_param, penalty = 'l1')

            # Use the training data to fit the model. In this case, we use the portion of the fold to train the model
            # with indices[0]. We then predict on the portion assigned as the 'test cross validation' with indices[1]
            lr.fit(x_train_data.iloc[indices[0],:],y_train_data.iloc[indices[0],:].values.ravel())

            # Predict values using the test indices in the training data
            yPredUnderSample = lr.predict(x_train_data.iloc[indices[1],:].values)

            # Calculate the recall score and append it to a list for recall scores representing the current c_parameter
            recall_acc = recallScore(y_train_data.iloc[indices[1],:].values, yPredUnderSample)
            recall_accs.append(recall_acc)
            print('Iteration ', iteration,': recall score = ', recall_acc)

        # The mean value of those recall scores is the metric we want to save and get hold of.
        results_table.ix[j,'Mean recall score'] = np.mean(recall_accs)
        j += 1
        print('')
        print('Mean recall score ', np.mean(recall_accs))
        print('')

    bestC = results_table.loc[results_table['Mean recall score'].idxmax()]['C_parameter']

    # Finally, we can check which C parameter is the best amongst the chosen.
    print('*********************************************************************************')
    print('Best model to choose from cross validation is with C parameter = ', bestC)
    print('*********************************************************************************')

    return bestC

def plotConfusionMatrix(cm, classes,
                          normalize=False,
                          title='Confusion matrix',
                          cmap=plt.cm.Blues):
    """
    This function prints and plots the confusion matrix.
    Normalization can be applied by setting `normalize=True`.
    """
    plt.imshow(cm, interpolation='nearest', cmap=cmap)
    plt.title(title)
    plt.colorbar()
    tick_marks = np.arange(len(classes))
    plt.xticks(tick_marks, classes, rotation=0)
    plt.yticks(tick_marks, classes)

    if normalize:
        cm = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis]
        #print("Normalized confusion matrix")
    else:
        1#print('Confusion matrix, without normalization')

    #print(cm)

    thresh = cm.max() / 2.
    for i, j in it.product(range(cm.shape[0]), range(cm.shape[1])):
        plt.text(j, i, cm[i, j],
                 horizontalalignment="center",
                 color="white" if cm[i, j] > thresh else "black")

    plt.tight_layout()
    plt.ylabel('True label')
    plt.xlabel('Predicted label')


# --------------------------------------------------


data               = pd.read_csv("./creditcard.csv")
data.head()
countClasses       = pd.value_counts(data['Class'], sort = True).sort_index()
data['normAmount'] = stdScaler().fit_transform(data['Amount'].reshape(-1, 1))
data               = data.drop(['Time','Amount'], axis=1)
data.head()
X                  = data.ix[:, data.columns != 'Class']
Y                  = data.ix[:, data.columns == 'Class']
noOfFrauds         = len(data[data.Class == 1])
fraudIndices       = np.array(data[data.Class == 1].index)
normalIndices      = data[data.Class == 0].index
rNormalIndices     = np.random.choice(normalIndices, noOfFrauds, replace = False)
rNormalIndices     = np.array(rNormalIndices)
underSampleIndices = np.concatenate([fraudIndices, rNormalIndices])
underSampleData    = data.iloc[underSampleIndices,:]
xUnderSample       = underSampleData.ix[:, underSampleData.columns != 'Class']
yUnderSample       = underSampleData.ix[:, underSampleData.columns == 'Class']

xTrain, xTest, yTrain, yTest = cvSplit(X, Y, test_size = 0.3, random_state = 0)
xTrainUnderSample, xTestUnderSample, yTrainUnderSample, yTestUnderSample = cvSplit(xUnderSample,    \
                                                                                   yUnderSample,    \
                                                                                   test_size=0.3,   \
                                                                                   random_state=0   )

print("Percentage of normal transactions: ", len(underSampleData[underSampleData.Class == 0]) / len(underSampleData))
print("Percentage of fraud transactions: ", len(underSampleData[underSampleData.Class == 1]) / len(underSampleData))
print("Total number of transactions in resampled data: ", len(underSampleData))
print("Number transactions train dataset: ", len(xTrain))
print("Number transactions test dataset: ", len(xTest))
print("Total number of transactions: ", len(xTrain)+len(xTest))
print("")
print("Number transactions train dataset: ", len(xTrainUnderSample))
print("Number transactions test dataset: ", len(xTestUnderSample))
print("Total number of transactions: ", len(yTestUnderSample)+len(xTestUnderSample))

bestC                 = printing_Kfold_scores(xTrainUnderSample, yTrainUnderSample)
lr                    = lReg(C=bestC, penalty='l1')
lr.fit(xTrainUnderSample, yTrainUnderSample.values.ravel())
yPredUnderSample      = lr.predict(xTestUnderSample.values)
yPred                 = lr.predict(xTest.values)
yPredUnderSampleScore = lr.fit(xTrainUnderSample, yTrainUnderSample.values.ravel()).decision_function(xTestUnderSample.values)

drawConfusionMatrix(yTestUnderSample, yPredUnderSample)
drawConfusionMatrix(yTest, yPred)
drawRocCurve(yTestUnderSample, yPredUnderSampleScore)

# See all the plots (Imbalanced training set, COnfusion matrix of training and test
# plt.show()
